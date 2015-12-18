/*
 * WebController.cpp
 *
 *  Created on: 18 dec. 2015
 *      Author: parhansson
 */

#include "WebController.h"
#include <dirent.h>
#include "json.h"
#include "utils.h"

//Emits json resonse
#define EMIT_RESPONSE_PARAM(fmt,...) w += json_emit(gp_response+w, MAX_LINE-w, fmt, ##__VA_ARGS__);
#define EMIT_RESPONSE(fmt, ...) int w = 0; w = json_emit(gp_response+w, MAX_LINE, "{ s: s, s: s, s: ", "api", object,"func",func, "params"); \
    EMIT_RESPONSE_PARAM(fmt,##__VA_ARGS__)  \
    w += json_emit(gp_response+w, MAX_LINE-w, ", s: i }", "ret",ret);

//Check function signature
#define FUNC_SIGP(name, nr_of_params) !strcmp(name,func) && params == nr_of_params

WebController::WebController(CGCodeInterpreter *interpreter, MessageQueue *message_queue, mg_server *serv)
:KmxController(interpreter,message_queue) {
  server = serv;
}

WebController::~WebController() {
  // TODO Auto-generated destructor stub
}

void WebController::PrintInfo(){
  mq->PrintInfo();


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
  fprintf(stdout, "State        :%s\n", this->interpreting ? "Interpreting" : "Stopped");
  fprintf(stdout, "Config       :%s\n", this->current_machine);
  fprintf(stdout, "File         :%s\n", this->current_file);
  fprintf(stdout, "Line         :%d\n", this->currentLine);
  fprintf(stdout, "FeedHold     :%d\n", this->main_status.StopImmediateState);
  fprintf(stdout, "Simulating   :%d\n", this->simulate);
}


//
/**
 *It is not allowed to call any function that takes mg_server from other thread than poll thread
 *Hence this function must only be called from poll thread
 */
int WebController::PushClientData(const char *data){
  return PushClientData(WEBSOCKET_OPCODE_TEXT, data, strlen(data));
}
int WebController::PushClientData(const char *data , size_t data_len){
  return PushClientData(WEBSOCKET_OPCODE_BINARY, data, data_len);
}

int WebController::OnEventRequest(struct mg_connection *conn) {
  if (conn->is_websocket) {
    char * content = strndup(conn->content, conn->content_len);
    //debug("MG_REQUEST websocket. content:\"%s\"",content);
    this->OnReceiveClientData(content);
    free(content);
    return MG_TRUE;
  } else if (isUploadRequest(conn)) {
    HandleUploadRequest(conn);
    return MG_TRUE; // Tell Mongoose we're done with this request
  } else if (isApiRequest(conn)) {
    //Starts with /api but can be anything
    return HandleApiRequest(conn);
  }
  return MG_FALSE;
}

// Mongoose sends MG_RECV for every received POST chunk.
// When last POST chunk is received, Mongoose sends MG_REQUEST, then MG_CLOSE.
int WebController::OnEventRecv(struct mg_connection *conn) {
  if (conn->is_websocket) {
    return 0; //MG_FALSE Do not discard anything from receive buffer;
  } else {

    if (isUploadRequest(conn)) {
      //returns written bytes to discard from receive buffer
      return HandleUploadReceiveEvent(conn);
    }
    return 0; //MG_FALSE Do not discard anything from receive buffer;
  }
}

int WebController::OnEventClose(struct mg_connection *conn) {
  // Make sure we free all allocated resources
  if (conn->connection_param != NULL) {
    fclose((FILE *) conn->connection_param);
    conn->connection_param = NULL;
    debug("handle close upload");
  }
  return MG_TRUE;
}

int WebController::OnEventPoll(struct mg_connection *conn) {
  this->Poll();
  //printf("event %d\n", ev);
  return MG_FALSE; //Return value is said to be ignored on MG_POLL but does'nt seem to be
}
int WebController::OnEventWsConnect(struct mg_connection *conn) {
  //if websocket /ws is param here
  //printf("event %d param: %s\n", ev, conn->uri);
  this->ClientConnect();
  return MG_FALSE;
}

int WebController::PushClientData(int opCode, const char *data , size_t data_len){
  struct mg_connection *c = NULL;
  int nrOfClients = 0;
  // Iterate over all connections, and push current time message to websocket ones.

  for (c = mg_next(server, c); c != NULL; c = mg_next(server, c)) {
    if (c->is_websocket) {
      mg_websocket_write(c, opCode, data, data_len);
      nrOfClients++;
    }
  }
  return nrOfClients;
}

bool WebController::isUploadRequest(struct mg_connection *conn){
  //strcmp(conn->uri, "/upload") == conn->uri;
  return strstr(conn->uri, "/upload") == conn->uri;
}

bool WebController::isApiRequest(struct mg_connection *conn){
  return strstr(conn->uri, "/api") == conn->uri;
}

int WebController::HandleUploadReceiveEvent(struct mg_connection *conn){
  FILE *fp = (FILE *) conn->connection_param;

  // Open temporary file where we going to write data
  if (fp == NULL && ((conn->connection_param = fp = tmpfile())) == NULL) {
    return -1;  // Close connection on error
  }
  // Return number of bytes written to a temporary file: that is how many
  // bytes we want to discard from the receive buffer
  return fwrite(conn->content, 1, conn->content_len, fp);
}
void WebController::HandleUploadRequest(struct mg_connection *conn){
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
          this->DoErrorMessage("Error writing to file");
          //ErrMsgHandler(strerror(errno));
        } else{
          filesize += written;
        }
        if(fclose (pFile) == EOF){
          this->DoErrorMessage("Error closing file");
        }
      } else {
        this->DoErrorMessage("Error saving file");
        this->DoErrorMessage(strerror(errno));
      }

    }

    if(filesize == 0 /*filesize != tmpfilesize*/){
      char msg[256];
      snprintf(msg,256, "Bytes written %ld of %ld",filesize, mmFile.filesize);
      this->DoErrorMessage(msg);
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
  OnEventClose(conn);
}






int WebController::HandleApiRequest(struct mg_connection *conn) {

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
    result = HandleJsonRequest(conn,argv[1],argv[2]);
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

int WebController::HandleJsonRequest(struct mg_connection *conn, const char *object, const char *func) {

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
            this->LoadGcode(file);
          } else if(fileType == 2){
            this->LoadMachineConfiguration(file);
          }
        }
        free(file);
      }
    } else if(FUNC_SIGP("listDir", 1)){
      ListDir(paramtoken);
    } else if (FUNC_SIGP("onFeedhold", 0)) {
      this->Feedhold();
    } else if (FUNC_SIGP("onSimulate", 0)) {
      this->Simulate();
    } else if (FUNC_SIGP("onEmergencyStop", 0)) {
      this->EmergencyStop();
    } else if (FUNC_SIGP("onHalt", 0)) {
      this->Halt();
    } else if (FUNC_SIGP("onStep", 0)) {
      this->Step();
    } else if (FUNC_SIGP("onReset", 0)) {
      this->Reset();
    } else if (FUNC_SIGP("onCycleStart", 0)) {
      this->CycleStart();
    } else if(FUNC_SIGP("onUpdateMotionParams", 0)) {
      this->UpdateMotionParams();
    } else if (FUNC_SIGP("onInvokeAction", 1)) {
      BOOL FlushBeforeUnbufferedOperation = FALSE;
      int action;
      toki(paramtoken + 1, &action);
      ret = this->InvokeAction(action,FlushBeforeUnbufferedOperation);
    } else if (FUNC_SIGP("onDoErrorMessage", 1)) {
      char *p1 = NULL;
      toks(paramtoken, &p1, 0);
      this->DoErrorMessage(p1);
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


void WebController::ListDir(struct json_token *paramtoken){
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
        this->DoErrorMessage(strerror(errno));
      }
    }
    free(dir);
}

