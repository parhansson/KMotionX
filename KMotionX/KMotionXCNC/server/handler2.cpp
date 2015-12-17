#include <sys/stat.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>
#include "dbg.h"
#include "handler.h"
#include "frozen.h"
#include "json.h"
#include "utils.h"
#include "callbackhandler.h"
#include "KmxController.h"


//Emits json resonse
#define EMIT_RESPONSE_PARAM(fmt,...) w += json_emit(gp_response+w, MAX_LINE-w, fmt, ##__VA_ARGS__);
#define EMIT_RESPONSE(fmt, ...) int w = 0; w = json_emit(gp_response+w, MAX_LINE, "{ s: s, s: s, s: ", "api", object,"func",func, "params"); \
    EMIT_RESPONSE_PARAM(fmt,##__VA_ARGS__)  \
    w += json_emit(gp_response+w, MAX_LINE-w, ", s: i }", "ret",ret);

//Check function signature
#define FUNC_SIGP(name, nr_of_params) !strcmp(name,func) && params == nr_of_params

KmxController *kmxCtrl;

//global poll thread response array, no need to allocate eah time since server is single threaded
static char gp_response[MAX_LINE];

CGCodeInterpreter *Interpreter;
CKMotionDLL *km;
CCoordMotion *CM;

void initHandler() {
  km = new CKMotionDLL(0);
  CM = new CCoordMotion(km);
  Interpreter = new CGCodeInterpreter(CM);
  kmxCtrl = new KmxController(Interpreter);
  //Set callbacks after initialising KmxController
  Interpreter->SetUserMCodeCallback(MUserCallback);
  Interpreter->SetUserCallback(UserCallback);
  km->SetErrMsgCallback(ErrMsgHandler);
  km->SetConsoleCallback(ConsoleHandler);
}

void info_handler(int signum) {
  //TODO might not work
  cbh_print_info();
  struct mg_connection *c = NULL;
  int websockets = 0;
  int connections = 0;
  // Iterate over all connections, and push current time message to websocket ones.
  for (c = mg_next(server, c); c != NULL; c = mg_next(server, c)) {
    if (c->is_websocket) {
      websockets++;
    } else {
      connections++;
    }
  }

  fprintf(stdout, "Websockets   :%d\n", websockets);
  fprintf(stdout, "Connections  :%d\n", connections);

  fprintf(stdout, "State        :%s\n", kmxCtrl->interpreting ? "Interpreting" : "Stopped");
  fprintf(stdout, "Config       :%s\n", kmxCtrl->current_machine);
  fprintf(stdout, "File         :%s\n", kmxCtrl->current_file);
  fprintf(stdout, "Line         :%d\n", kmxCtrl->currentLine);
  fprintf(stdout, "FeedHold     :%d\n", kmxCtrl->main_status.StopImmediateState);
  fprintf(stdout, "Simulating   :%d\n", kmxCtrl->simulate);

}

//
/**
 *It is not allowed to call any function that takes mg_server from other thread than poll thread
 *Hence this function must only be called from poll thread
 */
void push_to_clients(int opCode, const char *data , size_t data_len) {

  struct mg_connection *c = NULL;
  // Iterate over all connections, and push current time message to websocket ones.
  for (c = mg_next(server, c); c != NULL; c = mg_next(server, c)) {
    if (c->is_websocket) {
      mg_websocket_write(c, opCode, data, data_len);
    }
  }
}

void CompleteCallback(int status, int line_no, int sequence_number,const char *err) {
  kmxCtrl->OnCompleteCallback(status, line_no, sequence_number, err);
}

void StatusCallback(int line_no, const char *msg) {
  kmxCtrl->OnStatusCallback(line_no, msg);
}

void ErrMsgHandler(const char *msg) {
  kmxCtrl->OnErrorMessageCallback(msg);
}

int ConsoleHandler(const char *msg) {
  return kmxCtrl->OnConsoleCallback(msg);
}

int MessageBoxHandler(const char *title, const char *msg, int options) {
  return kmxCtrl->OnMessageBoxCallback(title, msg, options);
}

int UserCallback(const char *msg) {
  return kmxCtrl->OnUserCallback(msg);
}
int MUserCallback(int mCode) {
  return kmxCtrl->OnMcodeUserCallback(mCode);
}
//(Afx)MessageBox should be implementet as callback.
//only canon_stand_alone.cpp is using the message box result value
//hence maybe blocking call to this is unneccessary




int handle_poll(struct mg_connection *conn) {
  kmxCtrl->Poll();
  //printf("event %d\n", ev);
  return MG_FALSE; //Return value is said to be ignored on MG_POLL but does'nt seem to be
}
int handle_ws_connect(struct mg_connection *conn) {
  //if websocket /ws is param here
  //printf("event %d param: %s\n", ev, conn->uri);
  kmxCtrl->ClientConnect();
  return MG_FALSE;
}

bool isUploadRequest(struct mg_connection *conn){
  //strcmp(conn->uri, "/upload") == conn->uri;
  return strstr(conn->uri, "/upload") == conn->uri;
}

bool isApiRequest(struct mg_connection *conn){
  return strstr(conn->uri, "/api") == conn->uri;
}

int handleUploadReceive(struct mg_connection *conn){
  FILE *fp = (FILE *) conn->connection_param;

  // Open temporary file where we going to write data
  if (fp == NULL && ((conn->connection_param = fp = tmpfile())) == NULL) {
    return -1;  // Close connection on error
  }
  // Return number of bytes written to a temporary file: that is how many
  // bytes we want to discard from the receive buffer
  return fwrite(conn->content, 1, conn->content_len, fp);
}
void handleUploadRequest(struct mg_connection *conn){
  FILE *fp = (FILE *) conn->connection_param;
  if (fp != NULL) {

    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n"
              "Connection: close\r\n");

    // Temp file will be destroyed after fclose(), do something with the
    // data here -- for example, parse it and extract uploaded files.
    // As an example, we just echo the whole POST buffer back to the client.
    rewind(fp);
    MappedFile mmFile;
    if(mmapFile(mmFile, fp)){
      debug("Failed to read setup file");
      return;
    }
    size_t filesize=0;

    const char *data;
    int data_len, ofs = 0;
    char var_name[100], file_name[100];
    while ((ofs = mg_parse_multipart(mmFile.addr + ofs, mmFile.mapsize - ofs,
                                     var_name, sizeof(var_name),
                                     file_name, sizeof(file_name),
                                     &data, &data_len)) > 0) {

      FILE * pFile;
      //Add one to skip first slash and make path relative
      if(file_name[0] == '/'){
        pFile = fopen (file_name+1, "w+");
      } else {
        pFile = fopen (file_name, "w+");
      }
      //ConsoleHandler("Writing to file");
      //ConsoleHandler(file_name);
      if(pFile){
        size_t written = fwrite (data , sizeof(char), data_len, pFile);
        if(written != data_len){
          kmxCtrl->DoErrorMessage("Error writing to file");
          //ErrMsgHandler(strerror(errno));
        } else{
          filesize += written;
        }
        if(fclose (pFile) == EOF){
          kmxCtrl->DoErrorMessage("Error closing file");
        }
      } else {
        kmxCtrl->DoErrorMessage("Error saving file");
        kmxCtrl->DoErrorMessage(strerror(errno));
      }

    }

    if(filesize == 0 /*filesize != tmpfilesize*/){
      char msg[256];
      snprintf(msg,256, "Bytes written %ld of %ld",filesize, mmFile.filesize);
      kmxCtrl->DoErrorMessage(msg);
    }
    //need to send response back to client to avoid wating connection
    mg_printf_data(conn,
        "Written %ld bytes to file: %s\n\n",
        filesize,
        file_name);
  } else {
    mg_printf_data(conn, "%s", "Had no data to write...");
  }
  //MG_CLOSE event is not raised by mongoose as expected. Need to close file
  handle_close(conn);
}


int handle_request(struct mg_connection *conn,enum mg_event ev) {
  if (conn->is_websocket) {
    char * content = strndup(conn->content, conn->content_len);
    //debug("MG_REQUEST websocket. content:\"%s\"",content);
    kmxCtrl->OnReceiveClientData(content);
    free(content);
    return MG_TRUE;
  } else if (isUploadRequest(conn)) {
    handleUploadRequest(conn);
    return MG_TRUE; // Tell Mongoose we're done with this request
  } else if (isApiRequest(conn)) {
    //Starts with /api but can be anything
    return handle_api(conn, ev);
  }
  return MG_FALSE;
}

// Mongoose sends MG_RECV for every received POST chunk.
// When last POST chunk is received, Mongoose sends MG_REQUEST, then MG_CLOSE.
int handle_recv(struct mg_connection *conn) {
  if (conn->is_websocket) {
    return 0; //MG_FALSE Do not discard anything from receive buffer;
  } else {

    if (isUploadRequest(conn)) {
      //returns written bytes to discard from receive buffer
      return handleUploadReceive(conn);
    }
    return 0; //MG_FALSE Do not discard anything from receive buffer;
  }
}

int handle_close(struct mg_connection *conn) {
  // Make sure we free all allocated resources
  if (conn->connection_param != NULL) {
    fclose((FILE *) conn->connection_param);
    conn->connection_param = NULL;
    debug("handle close upload");
  }
  return MG_TRUE;
}

int ev_handler(struct mg_connection *conn, enum mg_event ev) {

  switch (ev) {
    case MG_AUTH:       return MG_TRUE;
    case MG_POLL:       return handle_poll(conn);
    case MG_REQUEST:    return handle_request(conn,ev);
    case MG_WS_CONNECT: return handle_ws_connect(conn);
    case MG_RECV:       return handle_recv(conn);
    case MG_CLOSE:      return handle_close(conn); // MG_CLOSE event is not raised for upload post, Called manually
    default:            return MG_FALSE;
  }

}

int handle_api(struct mg_connection *conn, enum mg_event ev) {

  if (conn->content == NULL) {
    //Content is NULL if request method is GET
    debug("GET is not supported.");
    return MG_FALSE;
  }
  int result;

  char **ap, *argv[3], *input, *tofree;
  tofree = input = strdup(conn->uri); // should check for NULL

  for (ap = argv; (*ap = strsep(&input, "/")) != NULL;)
          if (**ap != '\0')
                  if (++ap >= &argv[3])
                          break;
  if (!strcmp("api", argv[0])) {
    debug("%s/%s/%s\n", argv[0],argv[1],argv[2]);
    result = handleJson(conn,argv[1],argv[2]);
  } else {
    mg_send_header(conn, "Content-Type", "text/plain");
    mg_printf_data(conn, "KMotionCNC API # query_string %s\n", (char *) conn->query_string);
    mg_printf_data(conn, "KMotionCNC API # uri %s\n", (char *) tofree);
    result = MG_TRUE;
  }
  free(tofree);
  return result;
}


//TODO To ensure no blocking callbacks are made from poll thread this
//function should be called from another thread.

int handleJson(struct mg_connection *conn, const char *object, const char *func) {

  json_token *data = NULL;
  json_token *paramtoken;
  char *content;
  int params;
  int ret;

  content = strndup(conn->content, conn->content_len);
  debug("%s",content);

  free(content);

  data = parse_json2(conn->content, conn->content_len);
  if (data == NULL) {
    debug("parsing api request failed");
    return MG_FALSE;
  }

  params = 0;

  paramtoken = find_json_token(data, "params");
  if (paramtoken != NULL) {
    if (paramtoken->type == JSON_TYPE_OBJECT) {
      params = 1;

    } else if (paramtoken->type == JSON_TYPE_ARRAY) {
      params = paramtoken->num_desc;
    } else {
      params = 1;
    }
  }

  //Reset global response string
  gp_response[0] = '\0';
  //gp_response = (char*) malloc(sizeof(char) * ( MAX_LINE));
  ret = 0;
  if (!strcmp("kmx", object)) {
    if (FUNC_SIGP("loadGlobalFile", 2)) {
      int fileType = -1;
      char * file = NULL;
      toki(paramtoken + 1, &fileType);
      toks(paramtoken + 2, &file, 0);
      if (file != NULL) {
        if(strlen(file) > 0){
          if(fileType == 1){
            kmxCtrl->LoadGcode(file);
          } else if(fileType == 2){
            kmxCtrl->LoadMachineConfiguration(file);
          }
        }
        free(file);
      }
    } else if(FUNC_SIGP("listDir", 1)){
      listDir(paramtoken);
    } else if (FUNC_SIGP("onFeedhold", 0)) {
      kmxCtrl->Feedhold();
    } else if (FUNC_SIGP("onSimulate", 0)) {
      kmxCtrl->Simulate();
    } else if (FUNC_SIGP("onEmergencyStop", 0)) {
      kmxCtrl->EmergencyStop();
    } else if (FUNC_SIGP("onHalt", 0)) {
      kmxCtrl->Halt();
    } else if (FUNC_SIGP("onStep", 0)) {
      kmxCtrl->Step();
    } else if (FUNC_SIGP("onReset", 0)) {
      kmxCtrl->Reset();
    } else if (FUNC_SIGP("onCycleStart", 0)) {
      kmxCtrl->CycleStart();
    } else if(FUNC_SIGP("onUpdateMotionParams", 0)) {
      kmxCtrl->UpdateMotionParams();
    } else if (FUNC_SIGP("onInvokeAction", 1)) {
      BOOL FlushBeforeUnbufferedOperation = FALSE;
      int action;
      toki(paramtoken + 1, &action);
      ret = kmxCtrl->InvokeAction(action,FlushBeforeUnbufferedOperation);
    } else if (FUNC_SIGP("onDoErrorMessage", 1)) {
      char *p1 = NULL;
      toks(paramtoken, &p1, 0);
      kmxCtrl->DoErrorMessage(p1);
      EMIT_RESPONSE("[S]", p1);
      free(p1);
    } else {
      log_info("Function request is not part of API %s",func);
    }
  } else {
    log_info("API not implemented %s",object);
  }

  mg_send_header(conn, "Content-Type", "application/json");

  //Need to send some data or connection will not be closed
  if (gp_response[0] == '\0') {
    EMIT_RESPONSE("N");
  }

  mg_printf_data(conn, "%s", gp_response);

  //free(gp_response);
  free(data);

  return MG_TRUE;
}


void listDir(struct json_token *paramtoken){
    char *dir = NULL;
    toks(paramtoken, &dir, 0);
    if (dir) {
      DIR *dp;
      struct dirent *ep;
      int w = 0,first=1;
      dp = opendir (dir);
      if (dp != NULL){
          w += snprintf(gp_response+w, MAX_LINE-w, "[");
          while ((ep = readdir(dp))){
            //printf("%d %s\n", ep->d_type, ep->d_name);
            if(first){
              first = 0;
              w += json_emit(gp_response+w, MAX_LINE-w, "{s: s, s: i}", "name", ep->d_name,"type",ep->d_type );
            } else {
              w += json_emit(gp_response+w, MAX_LINE-w, ",{s: s, s: i}", "name", ep->d_name,"type",ep->d_type );
            }
          }
          w += snprintf(gp_response+w, MAX_LINE-w, "]");
          (void) closedir (dp);
      } else {
        kmxCtrl->DoErrorMessage(strerror(errno));
      }
    }
    free(dir);
}
