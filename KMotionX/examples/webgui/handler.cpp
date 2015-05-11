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

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <dirent.h>
//#include <unistd.h>
#include "dbg.h"
#include "handler.h"
#include "frozen.h"
#include "json.h"

enum cb_type {
  CB_STATUS, //Non blocking callback. Called from the interpreter in different thread
  CB_COMPLETE, //Non blocking callback. Called from the interpreter in different thread
  CB_ERR_MSG,      //Non blocking callback
  CB_CONSOLE,      //Non blocking callback, event though it has return value??
  CB_USER, //Blocking callback. Called from the interpreter in different thread
  CB_USER_M_CODE, //Blocking callback. Called from the interpreter in different thread
  CB_STATE,
  CB_MESSAGEBOX
};
enum cb_status {
  CBS_ENQUEUED, CBS_WAITING, CBS_ACKNOWLEDGED, CBS_STATE_IDLE
};

void setInterpreterParams(struct json_token *paramtoken, int indexOffset, int count, const char* pathTemplate);
void setMotionParams(struct json_token *paramtoken);
void interpret(struct json_token *paramtoken);
void loadGlobalFile(struct json_token *paramtoken);
void listDir(struct json_token *paramtoken);
int invokeAction(struct json_token *paramtoken);
void setSimulationMode(bool enable);

int handle_api(struct mg_connection *conn, enum mg_event ev);
int handleJson(struct mg_connection *conn,const char *object, const char *func);
void push_message(const char *msg);
void push_status();
void delete_callback(struct callback *node);
void ErrMsgHandler(const char *msg);
int ConsoleHandler(const char *msg);
int MessageBoxHandler(const char *, const char *, int );
int MUserCallback(int);
int UserCallback(const char *);
void enqueueState();
int enqueueCallback(const char * msg, enum cb_type type);
int readStatus();

struct callback * alloc_callback();
//struct callback * set_callback(struct callback *last, const char * message, enum cb_type type);

//Emits json resonse
#define EMIT_RESPONSE_PARAM(fmt,...) w += json_emit(gp_response+w, MAX_LINE-w, fmt, ##__VA_ARGS__);
#define EMIT_RESPONSE(fmt, ...) int w = 0; w = json_emit(gp_response+w, MAX_LINE, "{ s: s, s: s, s: ", "clazz", object,"func",func, "params"); \
    EMIT_RESPONSE_PARAM(fmt,##__VA_ARGS__)  \
    w += json_emit(gp_response+w, MAX_LINE-w, ", s: i }", "ret",ret);



//state of gui.
struct state {
  bool connected;
  bool simulate;
  int feedHold;
  bool interpreting;
  int currentLine;
  char current_file[256];
  char current_machine[256];
};

//Check function signature
#define FUNC_SIGP(name, nr_of_params) !strcmp(name,func) && params == nr_of_params

#define PARAM_OFFSET 7
struct operation_param {
  int type;  //DoErrMSG //in out, pointer int char array
  char name[24]; //kmotion
  char params[10]; // *i i *s *dDv (Ljava/lang/String;)V
};
struct operation {
  char object[24]; //kmotion
  char name[24];  //DoErrMSG
  int nr_params;
  int returnType;
  operation_param params[10]; // *i i *s *dDv (Ljava/lang/String;)V
};
struct callback {
  int id;
  enum cb_type type;
  enum cb_status status;
  time_t sent_time;
  int ret;
  char msg[512]; //TODO malloc correct size
  struct callback *next;
};
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t con = PTHREAD_COND_INITIALIZER;

CGCodeInterpreter *gci;
CKMotionDLL *km;
CCoordMotion *cm;
long int pollTID; //Poll thread id
time_t service_console_time;
state gstate;
MAIN_STATUS main_status;

struct callback *callbacks = NULL; // head of this list is always state callback
//Callback id counter.
int callbackCounter = 0;

//global poll thread response array, no need to allocate eah time since server is single threaded
char gp_response[MAX_LINE];

const char STATUS_NAMES[][24] = { "CBS_ENQUEUED", "CBS_WAITING",
    "CBS_ACKNOWLEDGED", "CBS_STATE_IDLE" };

const char CB_NAMES[][24] = { "STATUS", "COMPLETE", "ERR_MSG",
    "CONSOLE", "USER", "USER_M_CODE", "STATE", "MESSAGEBOX" };

const char ACTION_NAMES[][32] = { "M_Action_None", "M_Action_Setbit", "M_Action_SetTwoBits",
    "M_Action_DAC", "M_Action_Program", "M_Action_Program_wait", "M_Action_Program_wait_sync",
    "M_Action_Program_PC", "M_Action_Callback", "M_Action_Waitbit", "UNAVAILABLE" };

void initHandler() {
  km = new CKMotionDLL(0);
  cm = new CCoordMotion(km);
  gci = new CGCodeInterpreter(cm);
  gstate.interpreting = false;
  gstate.currentLine = 0;
  gstate.connected = false;
  gstate.simulate = false;
  gstate.feedHold = 0; // todo read from kflop
  //Enable messagebox callback by not setting this callback

  //gci->McodeActions[3].Action = M_Action_Callback;
  //gci->McodeActions[4].Action = M_Action_Program_PC;

  pollTID = getThreadId();
  //debug("Poll thread id %ld", pollTID);
  service_console_time = time(NULL);
  mb_callback = MessageBoxHandler;
  //Enqueue state as first callback. This first entry will never be removed
  callbacks = (struct callback *) malloc(sizeof(struct callback));
  callbacks->next = NULL;

  //Try without simulation on startup
  setSimulationMode(false); //enques state

  //Set callbacks after initialising.
  gci->SetUserMCodeCallback(MUserCallback);
  gci->SetUserCallback(UserCallback);
  km->SetErrMsgCallback(ErrMsgHandler);
  km->SetConsoleCallback(ConsoleHandler);
  readStatus();
}

void info_handler(int signum) {
  struct callback *last;
  int cb_count = 0;
  int statCounts[3][8][1];
  memset(statCounts, 0, sizeof(statCounts));
  last = callbacks;
  while (last->next != NULL) {
    statCounts[last->status][last->type][0]++;
    cb_count++;
    last = last->next;
  }
  for (int status = 0; status < 3; status++) {
    for (int type = 0; type < 8; type++) {
      int count = statCounts[status][type][0];
      if (count > 0) {
        fprintf(stdout, "%s, %s %d\n", STATUS_NAMES[status], CB_NAMES[type],
            count);
      }
    }
  }
  struct mg_connection *c = NULL;
  int websockets = 0;
  int connections = 0;
  // Iterate over all connections, and push current time message to websocket ones.
  for (c = mg_next(server, c); c != NULL; c = mg_next(server, c)) {
    if (c->is_websocket) {
      websockets++;
    } else {
      connections++;
      /*
      fprintf(stdout, "Connection URI :%s\n", c->uri);
      char * content = strndup(c->content, c->content_len);
      fprintf(stdout, "Connection Content :%s\n", content);
      free(content);
      */
    }
  }
  //debug("Pushed to %d sockets", sockets);

  fprintf(stdout, "Total callbacks in queue :%d\n", cb_count);
  fprintf(stdout, "Websockets   :%d\n", websockets);
  fprintf(stdout, "Connections  :%d\n", connections);

  fprintf(stdout, "State        :%s\n",
      gstate.interpreting ? "Interpreting" : "Stopped");
  fprintf(stdout, "Config       :%s\n", gstate.current_machine);
  fprintf(stdout, "File         :%s\n", gstate.current_file);
  fprintf(stdout, "Line         :%d\n", gstate.currentLine);
  fprintf(stdout, "FeedHold     :%d\n", gstate.feedHold);
  fprintf(stdout, "Simulating   :%d\n", gstate.simulate);

}

//TODO Implement support for blocking callback in poll thread
//#define isBlocking(type) (type == CB_USER || type == CB_USER_M_CODE || type ==  CB_MESSAGEBOX)
#define isBlocking(type) (type == CB_USER || type == CB_USER_M_CODE)

//
/**
 *It is not allowed to call any function that takes mg_server from other thread than poll thread
 *Hence this function must only be called from poll thread
 */
void push_message(const char *msg) {

  struct mg_connection *c = NULL;
  int len = strlen(msg);
  int sockets = 0;
  // Iterate over all connections, and push current time message to websocket ones.
  for (c = mg_next(server, c); c != NULL; c = mg_next(server, c)) {
    if (c->is_websocket) {
      sockets++;
      mg_websocket_write(c, WEBSOCKET_OPCODE_TEXT, msg, len);
    }
  }
  //debug("Pushed to %d sockets", sockets);
}
int readStatus(){
  int result = km->GetStatus(main_status,true);
  if(result){
      gstate.connected = false;
      setSimulationMode(true);
      gci->Abort();
      // error reading status
      km->Failed();
      debug("GetStatus failed\n");
   } else {
      gstate.connected = true;
   }
  return result;
}
void push_status() {

  if(!gstate.simulate){

    //printf("MAIN_STATUS size %d\n", sizeof(status)); // 464?

    if(readStatus()){
      return;
    }
    //TODO handle struct padding wich may differ on platforms
    int len = 464;
    char msg[464];
    memcpy(msg, &main_status, 464);
    struct mg_connection *c = NULL;
    int sockets = 0;
    // Iterate over all connections, and push current time message to websocket ones.
    for (c = mg_next(server, c); c != NULL; c = mg_next(server, c)) {
      if (c->is_websocket) {
        sockets++;
        mg_websocket_write(c, WEBSOCKET_OPCODE_BINARY, msg, len);
      }
    }

  }

  //debug("Pushed to %d sockets", sockets);
}

void pollCallback(struct callback *cb, int id, int ret) {
  if (cb->status == CBS_ENQUEUED) {
    cb->sent_time = time(NULL);
    push_message(cb->msg);
    cb->status = CBS_WAITING;
  } else if (cb->status == CBS_WAITING) {

    if (cb->id == id) {
      cb->status = CBS_ACKNOWLEDGED;
      cb->ret = ret;
      if (cb->type == CB_STATE) {
        cb->status = CBS_STATE_IDLE;
      } else {
        //Only signal waiting thread if blocking callback
        if (isBlocking(cb->type)) {
          pthread_cond_signal(&con); //wake up waiting thread with condition variable
        } else {
          //printf("Non blocking callback acked and deleted\n");
          delete_callback(cb);
        }
      }
    } else {
      //resend message if not acked for 30 seconds
      //This will prevent hanging threads if browser is closed.
      //When a new browser is connected the message will show up and can be acked
      time_t current_time = time(NULL);
      if (current_time - cb->sent_time > 30) {
        cb->status = CBS_ENQUEUED;
      }

    }
  }
}
#define DUMP_CON if(conn->content_len > 0){ \
    char *_msg; \
    _msg = strndup(conn->content, conn->content_len); \
    printf("Raw: %s\n",_msg); \
    free(_msg); \
}

void pollCallbacks(char * content) {

  int id = -1;
  char type[16];
  memset(type,0,16);

  int ret = -1;
  if (content != NULL) {
    //we have a connection.
    //incoming answer from client
    if (strlen(content) > 12 && memcmp(content, "CB_ACK:", 7) == 0) {
      sscanf(content, "CB_ACK: %d  %s %d", &id, type, &ret);

/*
       char *msg;
       msg = strndup(conn->content, conn->content_len);
       debug("CB_ACK id: %s type: %s    raw: %s\n",id,type,msg);
       free(msg);
*/
    }
  }

  pthread_mutex_lock(&mut);

  struct callback *node, *current;

  node = callbacks;
  do{

    current = node;
    // pollCallback might free the current node in this case node->next is not valid.
    node = node->next;
    pollCallback(current, id, ret);
  } while (node != NULL);

  pthread_mutex_unlock(&mut);
}
struct callback * alloc_callback() {
  struct callback *last;
  last = callbacks;
  int size = 0;
  while (last->next != NULL) {
    last = last->next;
    size++;
  }
  last->next = (struct callback *) malloc(sizeof(struct callback));
  last->next->next = NULL;
  return last->next;
}
struct callback * init_callback(struct callback *last, const char * message, enum cb_type type) {

  if (last == NULL) {
    last = alloc_callback();
  }

  last->id = callbackCounter++;
  last->status = CBS_ENQUEUED;
  last->ret = -1;
  last->type = type;
  //{ "id": 5, "type": 67, data: ""|6776|{}|[]|null|true|false}
  json_emit(last->msg, 512, "{ s: i, s: s, s: S, s: S}",
      "id", last->id,
      "type", CB_NAMES[type],
      "block",isBlocking(type)?"true":"false",
      "data", message);

  //debug("Enqueued %s message: %s",CB_NAMES[type], last->msg);
  return last;
}
void delete_callback(struct callback *node) {
  // When node to be deleted is head node
  if (callbacks == node) {
    log_err("Not allowed to remove head of callback list");
    return;
  } else {

    // When not first node, follow the normal deletion process
    // find the previous node
    struct callback *prev = callbacks;
    while (prev->next != NULL && prev->next != node) {
      prev = prev->next;
    }

    // Check if node really exists in Linked List
    if (prev->next == NULL) {
      log_err("Given node is not present in Linked List ");
      return;
    }

    // Remove node from Linked List
    prev->next = prev->next->next;
  }
  // Free memory
  free(node);
}
//msg needs to be quoted if string
int enqueueCallback(const char * msg, enum cb_type type) {

  //Enqueued messages are held in list (callbacks)
  //Callbacks are created here and sent to clients in next poll

  struct callback * cb;
  long int curTID = getThreadId();
  int result = 0;
  //debug("Poll thread: %ld. Current thread: %ld", pollTID, curTID);
  if (pollTID == curTID) {
    //if this is a blocking call. then we are in trouble.
    //we can not stop and wait in poll/main thread
    //TODO mutex lock here is new and not very well tested
    pthread_mutex_lock(&mut);
    cb = init_callback(NULL, msg, type);
    if (isBlocking(cb->type)) {
      log_err("-------ERROR: Blocking callback in poll thread %ld", pollTID);
      result = cb->ret;
      delete_callback(cb);
    }
    pthread_mutex_unlock(&mut);
  } else {
    pthread_mutex_lock(&mut);
    cb = init_callback(NULL, msg, type);
    if (isBlocking(cb->type)) {
      debug("wait called");
      pthread_cond_wait(&con, &mut); //wait for the signal with con as condition variable
      debug("wait done signal received");
      result = cb->ret;
      delete_callback(cb);
    }
    pthread_mutex_unlock(&mut);
  }

  return result;
}

void CompleteCallback(int status, int line_no, int sequence_number,
    const char *err) {
  //This is a non blocking call from other thread to be enqueued in poll thread
  char buf[256];
  json_emit(buf, 256, "{ s: i, s: i, s: i, s: s }",
      "line", line_no,
      "status", status,
      "sequence", sequence_number,
      "message", err);
  gstate.interpreting = false;
  gstate.currentLine = line_no;
  enqueueState();
  enqueueCallback(buf, CB_COMPLETE);
}

void StatusCallback(int line_no, const char *msg) {
  //This is a non blocking call from other thread to be enqueued in poll thread
  char buf[256];
  if(gstate.currentLine != line_no){
    gstate.currentLine = line_no;
    enqueueState();
  }
  json_emit(buf, 256, "{ s: i, s: s }",
      "line", line_no,
      "message", msg);
  enqueueCallback(buf, CB_STATUS);
}

void ErrMsgHandler(const char *msg) {
  //This is a non blocking call enqueue in poll thread
  char buf[256];
  json_emit(buf, 256, "s", msg);
  enqueueCallback(buf, CB_ERR_MSG);

}

int ConsoleHandler(const char *msg) {
  //This is a non blocking call even though it has a return value.. strange
  //Not even sure this happens in another thread
  char buf[256];
  json_emit(buf, 256, "s", msg);
  return enqueueCallback(buf, CB_CONSOLE);
}

int MessageBoxHandler(const char *title, const char *msg, int options) {
  int blocking = 0;
  //non blocking MB_OK?
  if ((options & (MB_YESNO | MB_OKCANCEL)) == (MB_YESNO | MB_OKCANCEL)) {
    blocking = 1;
  }

  //TODO Messages might be pretty long check this and alloc
  char buf[512];
  json_emit(buf, 512, "{ s: i, s: s, s: s }",
      "options", options,
      "title", title,
      "message", msg);
  //todo handle blocking messagebox
  return enqueueCallback(buf, CB_MESSAGEBOX);
}

int UserCallback(const char *msg) {
  //This is a blocking call. A bit trickier
  char buf[256];
  json_emit(buf, 256, "s", msg);
  int result = enqueueCallback(buf, CB_USER);
  debug("UserCallback result: %d", result);
  return result;
}
int MUserCallback(int mCode) {
  //This is a blocking call. A bit trickier
  //Currently not supported.
  //1 start thread
  //2 enqueu callback to poll thread
  //3 wait for answer in poll thread
  //4 poll thread gets answer form client
  //5 mutex or whatever wait lock used is relased
  //thred
  //wait for thread join;
  //or just sleep short amount of time in a loop until condition true (answer i poll thred)
  //mutex in poll to update linked list of callbacks waiting for answer
  //the thread needs to wait for timeout and then issue the request again in poll thread
  //if gui is closed and reopened
  //instantiate a struct and wait for a signal/variable in that struct
  char buf[10];
  json_emit(buf, 10, "i", mCode);
  int result = enqueueCallback(buf, CB_USER_M_CODE);
  debug("UserMCallback result: %d", result);
  return result;

}
//(Afx)MessageBox should be implementet as callback.
//only canon_stand_alone.cpp is using the message box result value
//hence maybe blocking call to this is unneccessary

void enqueueState() {
  //Lock mutex when enqueing state.
  pthread_mutex_lock(&mut);
  char stateBuf[512];
  //debug("enquing current file: %s", gstate.current_file);
  //this could be implemented as callback to one client (connection) only
  json_emit(stateBuf, 512, "{ s: S, s: S, s: S, s: i, s: s, s: s }",
      "feedHold", gstate.feedHold?"true":"false",
      "interpreting", gstate.interpreting?"true":"false",
      "simulate", gstate.simulate?"true":"false",
      "line", gstate.currentLine,
      "file", gstate.current_file,
      "machine", gstate.current_machine);
  init_callback(callbacks, stateBuf, CB_STATE);
  pthread_mutex_unlock(&mut);
}

int handle_poll(struct mg_connection *conn) {
  //poll is called every 200 ms
  pollCallbacks(NULL);

  if(!gstate.simulate){
    if(gstate.connected){
      push_status();
    }
    time_t current_time = time(NULL);

    if (current_time - service_console_time > 1) {
      if(!gstate.connected){
        push_status();
      }
      if(km->ServiceConsole()){
        //TODO not verified that this works.
        ErrMsgHandler(">ServiceConsole Failed\n");
      }
      service_console_time = current_time;
    }
  }

  //printf("event %d\n", ev);
  return MG_FALSE; //Return value is ignored on MG_POLL
}
int handle_ws_connect(struct mg_connection *conn) {
  //if websocket /ws is param here
  //printf("event %d param: %s\n", ev, conn->uri);
  enqueueState();
  return MG_FALSE;
}

bool isUploadRequest(struct mg_connection *conn){
  //strcmp(conn->uri, "/upload") == 0;
  bool result = strstr(conn->uri, "/upload") == conn->uri;
  //debug("isUpload %d", result);
  return result;
}

bool isApiRequest(struct mg_connection *conn){
  bool result = strstr(conn->uri, "/api") == conn->uri;
  //debug("isApi %d", result);
  return result;
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
    //This row was in the example. But should not be here
    //fwrite(conn->content, 1, conn->content_len, fp); // Write last bits


    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n"
              "Connection: close\r\n");

    // Temp file will be destroyed after fclose(), do something with the
    // data here -- for example, parse it and extract uploaded files.
    // As an example, we just echo the whole POST buffer back to the client.


    int pagesize, offset, fd;
    size_t tmpfilesize, mapsize,filesize;
    caddr_t addr;

    filesize=0;
    tmpfilesize = ftell(fp);
    rewind(fp);
    offset = 0;
    fd = fileno(fp);
    pagesize = getpagesize();
    mapsize = (tmpfilesize/pagesize)*pagesize+pagesize; // align memory allocation with pagesize
    //debug("FileSize: %ld\nPageSize: %ld\nMapSize:  %ld\n",tmpfilesize, pagesize, mapsize);

    //memory map tmp file and parse it.
    addr = (char*)mmap((caddr_t)0, mapsize, PROT_READ, MAP_PRIVATE, fd,offset);

    const char *data;
    int data_len, ofs = 0;
    char var_name[100], file_name[100];
    while ((ofs = mg_parse_multipart(addr + ofs, mapsize - ofs,
                                     var_name, sizeof(var_name),
                                     file_name, sizeof(file_name),
                                     &data, &data_len)) > 0) {
      //ErrMsgHandler("Parse multipart");
      //fprintf(stdout, "var: %s, file_name: %s, size: %d bytes\n",var_name, file_name, data_len);

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
          ErrMsgHandler("Error writing to file");
          //ErrMsgHandler(strerror(errno));
        } else{
          filesize += written;
        }
        if(fclose (pFile) == EOF){
          ErrMsgHandler("Error closing file");
        }
      } else {
        ErrMsgHandler("Error saving file");
        ErrMsgHandler(strerror(errno));
      }

    }

    munmap(addr, mapsize);
    if(filesize == 0 /*filesize != tmpfilesize*/){
      char msg[256];
      snprintf(msg,256, "Bytes written %d of %d",filesize, tmpfilesize);
      ErrMsgHandler(msg);
    }
    //need to send response back to client to avoid wating connection
    mg_printf_data(conn,
        "Written %ld bytes to file: %s\n\n",
        filesize,
        file_name);
  } else {
    mg_printf_data(conn, "%s", "Had no data to write...");
  }
}


int handle_request(struct mg_connection *conn,enum mg_event ev) {
  if (conn->is_websocket) {
    char * content = strndup(conn->content, conn->content_len);
    //debug("MG_REQUEST websocket. content:\"%s\"",content);
    pollCallbacks(content);
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
    //char * content = strndup(conn->content, conn->content_len);
    //debug("MG_RECV websocket. content:\"%s\"",content);
    //free(content);
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
  }
  return MG_TRUE;
}

int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  //if(ev != MG_POLL){
  //  debug("Event %d %s", ev,conn->uri);
  //}
  switch (ev) {
    case MG_AUTH:       return MG_TRUE;
    case MG_POLL:       return handle_poll(conn);
    case MG_REQUEST:    return handle_request(conn,ev);
    case MG_WS_CONNECT: return handle_ws_connect(conn);
    case MG_RECV:       return handle_recv(conn);
    case MG_CLOSE:      return handle_close(conn);
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

  char * content = strndup(conn->content, conn->content_len);
  debug("%s",content);
  //fprintf(stderr, "%s\n",content);
  free(content);

  data = parse_json2(conn->content, conn->content_len);
  if (data == NULL) {
    debug("parsing api request failed");
    return MG_FALSE;
  }
  //char * gp_response = NULL;
  json_token *paramtoken;

  int params = 0;

  //debug("object: %s method: %s",object, func);

  paramtoken = find_json_token(data, "params");
  if (paramtoken != NULL) {
    if (paramtoken->type == JSON_TYPE_OBJECT) {
      params = 1;

    } else if (paramtoken->type == JSON_TYPE_ARRAY) {
      params = paramtoken->num_desc;
      /*
       int ival = 0;

       for(int i = 1; i <= params;i++){
       token = (paramtoken+i);
       json_type type = token->type;
       if(type == JSON_TYPE_STRING){
       char *value = NULL;
       toks(token,&value);
       debug("param %d string: %s", i,value);
       free(value);

       } else if(type == JSON_TYPE_NUMBER){
       toki(token,&ival);
       debug("param %d number: %d", i,ival);

       }
       }
       */
    } else {
      params = 1;
    }
  }
  //debug("params %d",params);

  //Reset global response string
  gp_response[0] = '\0';
  //gp_response = (char*) malloc(sizeof(char) * ( MAX_LINE));
  int ret = 0;
  if (!strcmp("aux", object)) {
    if (FUNC_SIGP("loadGlobalFile", 2)) {
      loadGlobalFile(paramtoken);
    /*
     } else if (FUNC_SIGP("saveMachine", 1)) {
      char *machineFile = NULL;
      char *machineData = NULL;
      toks(paramtoken+1, &machineFile);
      toks(paramtoken+2, &machineData);

      if (machineData && machineFile) {
        FILE *fp;
        fp = fopen(machineFile, "w");
        if (fp != NULL) {
          fputs(machineData, fp);
          fclose(fp);
        } else {
          ErrMsgHandler(strerror(errno));
        }

      }

      free(machineFile);
      free(machineData);
     */
    } else if(FUNC_SIGP("listDir", 1)){
      listDir(paramtoken);
    }

  } else if (!strcmp("kmotion", object)) {
    if (FUNC_SIGP("feedHold", 0)) {

      char res[MAX_LINE];
      if (!km->WriteLineReadLine("GetStopState", res)) {
        if (res[0] == '0') {
          gstate.feedHold = 1;
          km->WriteLine("StopImmediate0");
        } else {
          gstate.feedHold = 0;
          km->WriteLine("StopImmediate1");
        }
        enqueueState();
        EMIT_RESPONSE("[i]", gstate.feedHold);
      }
    } else if (FUNC_SIGP("CheckCoffSize", 0)) {

    } else if (FUNC_SIGP("CheckForReady", 0)) {
      ret = km->CheckForReady();
      EMIT_RESPONSE("[]");
    } else if (FUNC_SIGP("CheckKMotionVersion", 0)) {
      ret = km->CheckKMotionVersion();
      EMIT_RESPONSE("[]");
    } else if (FUNC_SIGP("CheckKMotionVersion", 1)) {
      int type;
      toki(paramtoken + 1, &type);
      ret = km->CheckKMotionVersion(&type);
      EMIT_RESPONSE("[i]", type);
    } else if (FUNC_SIGP("CheckKMotionVersion", 2)) {
      int type;
      bool GetBoardTypeOnly;
      toki(paramtoken + 1, &type);
      tokb(paramtoken + 2, &GetBoardTypeOnly);
      ret = km->CheckKMotionVersion(&type, GetBoardTypeOnly);
      EMIT_RESPONSE("[i]", type);

    } else if (FUNC_SIGP("Compile", 6)) {
      char *Name = NULL;
      char *OutFile = NULL;
      int BoardType;
      int Thread;
      char *Err = NULL;
      int MaxErrLen;
      //toks(&data[PARAM_OFFSET],&Name);
      toks(paramtoken + 1, &Name);
      toks(paramtoken + 2, &OutFile);
      toki(paramtoken + 3, &BoardType);
      toki(paramtoken + 4, &Thread);
      toki(paramtoken + 6, &MaxErrLen);
      toks(paramtoken + 5, &Err, MaxErrLen);

      ret = km->Compile(Name, OutFile, BoardType, Thread, Err, MaxErrLen);
      EMIT_RESPONSE("[S, S, i, i, S, i]", Name, OutFile, BoardType, Thread, Err,
          MaxErrLen);
      free(Name);
      free(OutFile);
      free(Err);

    } else if (FUNC_SIGP("CompileAndLoadCoff", 2)) {
      char *Name = NULL;
      int Thread;
      toks(paramtoken + 1, &Name);
      toki(paramtoken + 2, &Thread);
      char err[512];
      if(km->CompileAndLoadCoff(Name, Thread, err, 511)){
        if(strlen(err)>0)ErrMsgHandler(err);
      }
      free(Name);
    } else if (FUNC_SIGP("ConvertToOut", 4)) {

      int p1;
      char *p2 = NULL;
      char *p3 = NULL;
      int p4;
      toki(paramtoken + 1, &p1);
      toks(paramtoken + 2, &p2);
      toki(paramtoken + 4, &p4);
      toks(paramtoken + 3, &p3, p4);

      km->ConvertToOut(p1, p2, p3, p4);
      ret = 0;
      EMIT_RESPONSE("[i, S, S, i]", p1, p2, p3, p4);
      free(p2);
      free(p3);

    } else if (FUNC_SIGP("Disconnect", 0)) {
      ret = km->Disconnect();
      EMIT_RESPONSE("[]");
    } else if (FUNC_SIGP("DoErrMsg", 1)) {
      char *p1 = NULL;
      toks(paramtoken, &p1);
      ret = 0;
      km->DoErrMsg(p1);
      EMIT_RESPONSE("[S]", p1);
      free(p1);
    } else if (FUNC_SIGP("ExtractCoffVersionString", 2)) {
    } else if (FUNC_SIGP("Failed", 0)) {

    } else if (FUNC_SIGP("FirmwareVersion", 0)) {
      ret = km->FirmwareVersion();
      EMIT_RESPONSE("[]");
    } else if (FUNC_SIGP("GetLoadAddress", 2)) {
    } else if (FUNC_SIGP("GetStatus", 2)) {
    } else if (FUNC_SIGP("KMotionLock", 0)) {
    } else if (FUNC_SIGP("KMotionLockRecovery", 0)) {
    } else if (FUNC_SIGP("ListLocations", 2)) {
    } else if (FUNC_SIGP("LoadCoff", 2)) {
    } else if (FUNC_SIGP("LoadCoff", 3)) {
    } else if (FUNC_SIGP("ReadLineTimeOut", 1)) {
    } else if (FUNC_SIGP("ReadLineTimeOut", 2)) {
    } else if (FUNC_SIGP("ReleaseToken", 0)) {
    } else if (FUNC_SIGP("ServiceConsole", 0)) {
    } else if (FUNC_SIGP("SetConsoleCallback", 1)) {
      km->SetConsoleCallback(ConsoleHandler);
    } else if (FUNC_SIGP("SetErrMsgCallback", 1)) {
    } else if (FUNC_SIGP("USBLocation", 0)) {
    } else if (FUNC_SIGP("WaitToken", 0)) {
    } else if (FUNC_SIGP("WaitToken", 1)) {
    } else if (FUNC_SIGP("WaitToken", 2)) {
    } else if (FUNC_SIGP("WriteLine", 1)) {
    } else if (FUNC_SIGP("WriteLineReadLine", 2)) {
      char p1[200];
      char p2[200];
      ret = km->WriteLineReadLine(p1, p2);
      EMIT_RESPONSE("[S, S]", p1, p2);
    } else if (FUNC_SIGP("WriteLineWithEcho", 1)) {
    }

  } else if (!strcmp("interpreter", object)) {
    if (FUNC_SIGP("InvokeAction", 1)) {
      ret = invokeAction(paramtoken);
    } else if (FUNC_SIGP("Interpret", 5)) {

      interpret(paramtoken);
      // test message from poll thread
      //blocking message from pull thread should be implementet to work.
      //km->DoErrMsg("Error message from poll thread");


//    } else if (FUNC_SIGP("SetUserMCallback", 1)) {
//    } else if (FUNC_SIGP("SetUserMCodeCallback", 1)) {


    } else if (!strcmp("setMotionParams",func) /*FUNC_SIGP("setMotionParams", 54)*/) {

      setMotionParams(paramtoken);

    } else if (FUNC_SIGP("simulate", 1)) {
      if (!gstate.interpreting) {
        bool enable;
        tokb(paramtoken, &enable);
        setSimulationMode(enable);
      }
      EMIT_RESPONSE("[S]", gstate.simulate?"true":"false");
    }
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

void setSimulationMode(bool enable){
  gstate.simulate = gci->CoordMotion->m_Simulate = enable;
  if(enable != gstate.simulate){
    enqueueState();
  }
}
int invokeAction(struct json_token *paramtoken){
    BOOL FlushBeforeUnbufferedOperation = FALSE;
    int action;
    toki(paramtoken + 1, &action);
    //Interpreter->InvokeAction(10,FlushBeforeUnbufferedOperation);  // resend new Speed
    return gci->InvokeAction(action,FlushBeforeUnbufferedOperation);  // invoke action number
}

void loadGlobalFile(struct json_token *paramtoken){


    int fileType = -1;
    char * file = NULL;
    char * globalFile = NULL;
    toki(paramtoken + 1, &fileType);
    toks(paramtoken + 2, &file);
    if (file != NULL) {
      if(strlen(file) > 0){
        if(fileType == 1){
          globalFile = gstate.current_file;
        } else if(fileType == 2){
          globalFile = gstate.current_machine;
        }
        if(globalFile != NULL){
          if (strcmp(globalFile, file) != 0) {
              strcpy(globalFile, file);
              //when file changes all clients will e noticed.
              enqueueState();
          }
        }
      }
      free(file);
    }
}


void listDir(struct json_token *paramtoken){

    char *dir = NULL;
    toks(paramtoken, &dir);
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
        ErrMsgHandler(strerror(errno));
      }
    }
    free(dir);
}

void interpret(struct json_token *paramtoken) {
  int BoardType;      // = BOARD_TYPE_KFLOP;
  char *InFile = NULL;
  int start;      // = 0;
  int end;      //= -1;
  bool restart;      // = true;

  toki(paramtoken + 1, &BoardType);
  toks(paramtoken + 2, &InFile);
  toki(paramtoken + 3, &start);
  toki(paramtoken + 4, &end);
  tokb(paramtoken + 5, &restart);
  if (InFile) {
    if (!gstate.interpreting) {
      if (!gci->Interpret(BoardType, InFile, start, end, restart,
          StatusCallback, CompleteCallback)) {
        gstate.interpreting = true;
        enqueueState();
      }
    }
  }

  free(InFile);
}

void setMotionParams(struct json_token *paramtoken) {

    char * name = NULL;
    json_token *token;

    MOTION_PARAMS *p=gci->GetMotionParams();
    double maxAccel,maxVel,countsPerUnit;
    double breakAngle,collinearTol,cornerTol,facetAngle,tpLookahead;

    breakAngle = collinearTol = cornerTol = facetAngle = tpLookahead = 0;
    json_double(paramtoken,"tplanner.breakangle",&breakAngle);
    json_double(paramtoken,"tplanner.cornertolerance",&cornerTol);
    json_double(paramtoken,"tplanner.lookahead",&tpLookahead);
    json_double(paramtoken,"tplanner.collineartolerance",&collinearTol);
    json_double(paramtoken,"tplanner.facetangle",&facetAngle);

    p->BreakAngle = breakAngle;
    p->CollinearTol = collinearTol;
    p->CornerTol = cornerTol;
    p->FacetAngle = facetAngle;
    p->TPLookahead = tpLookahead;

    //TODO
    //p->RadiusA = m_RadiusA;
    //p->RadiusB = m_RadiusB;
    //p->RadiusC = m_RadiusC;

    char path[64];
    for(int i= 0;i<6;i++){
      sprintf(path,"axes[%i]",i);
      token = find_json_token(paramtoken, path);
      if(token){
        maxAccel = maxVel = countsPerUnit = 0.0;
        json_str(token,"name",&name,0);
        json_double(token,"countsPerUnit",&countsPerUnit);
        json_double(token,"maxAccel",&maxAccel);
        json_double(token,"maxVel",&maxVel);

        // default values form KMotionCNCDlg.c
        maxAccel = maxAccel == 0.0?0.01:maxAccel;
        maxVel = maxVel == 0.0?0.1:maxVel;
        //Zero on countPerUnit will abort GCode
        countsPerUnit = countsPerUnit == 0.0?100.0:countsPerUnit;
        switch (name[0]) {
          case 'X':
            p->CountsPerInchX = countsPerUnit;
            p->MaxAccelX = maxAccel;
            p->MaxVelX = maxVel;
            break;
          case 'Y':
            p->CountsPerInchY = countsPerUnit;
            p->MaxAccelY = maxAccel;
            p->MaxVelY = maxVel;
            break;
          case 'Z':
            p->CountsPerInchZ = countsPerUnit;
            p->MaxAccelZ = maxAccel;
            p->MaxVelZ = maxVel;
            break;
          case 'A':
            p->CountsPerInchA = countsPerUnit;
            p->MaxAccelA = maxAccel;
            p->MaxVelA = maxVel;
            break;
          case 'B':
            p->CountsPerInchB = countsPerUnit;
            p->MaxAccelB = maxAccel;
            p->MaxVelB = maxVel;
            break;
          case 'C':
            p->CountsPerInchC = countsPerUnit;
            p->MaxAccelC = maxAccel;
            p->MaxVelC = maxVel;
            break;
        }

      } else {
        printf("Failed %s\n",path);
      }

    }

    //M2-M9,S index 2-9
    //userButtons index 11-20
    //M100-M119 index 21 -39
    setInterpreterParams(paramtoken, 0,MAX_MCODE_ACTIONS_M1,"actions[%i]");
    setInterpreterParams(paramtoken, MAX_MCODE_ACTIONS_M1,MAX_MCODE_ACTIONS_BUTTONS,"userActions[%i]");
    setInterpreterParams(paramtoken, MCODE_ACTIONS_M100_OFFSET,MAX_MCODE_ACTIONS_M100,"extendedActions[%i]");

    printf("X %lf, %lf, %lf \n",p->CountsPerInchX,p->MaxAccelX, p->MaxVelX );
    printf("Y %lf, %lf, %lf \n",p->CountsPerInchY,p->MaxAccelY, p->MaxVelY );
    printf("Z %lf, %lf, %lf \n",p->CountsPerInchZ,p->MaxAccelZ, p->MaxVelZ );
    printf("A %lf, %lf, %lf \n",p->CountsPerInchA,p->MaxAccelA, p->MaxVelA );
    printf("B %lf, %lf, %lf \n",p->CountsPerInchB,p->MaxAccelB, p->MaxVelB );
    printf("C %lf, %lf, %lf \n",p->CountsPerInchC,p->MaxAccelC, p->MaxVelC );

    //TODO
    //strcpy(Interpreter->ToolFile,m_ToolFile);
    //strcpy(Interpreter->SetupFile,m_SetupFile);
    //strcpy(Interpreter->GeoFile,m_GeoFile);
    //strcpy(Interpreter->VarsFile,m_VarsFile);
    //p->DegreesA = m_DegreesA!=0;
    //p->DegreesB = m_DegreesB!=0;
    //p->DegreesC = m_DegreesC!=0;
    p->ArcsToSegs = true;;

    free(name);


    gci->CoordMotion->SetTPParams();

}
void setInterpreterParams(struct json_token *paramtoken, int indexOffset, int count, const char* pathTemplate) {
  double dParam0, dParam1, dParam2, dParam3, dParam4;
  int action;
  char path[64];
  char *file = NULL;
  char *name = NULL;
  json_token *token;

  for(int i= 0;i<count;i++){
    sprintf(path,pathTemplate,i);
    token = find_json_token(paramtoken, path);
    if(token){
      action = dParam0 = dParam1 = dParam2 = dParam3 = dParam4 = 0;
      json_int(token,"action",&action);
      if(action > 0){
        json_str(token,"name",&name,0);
        json_str(token,"file",&file,0);
        json_double(token,"dParam0",&dParam0);
        json_double(token,"dParam1",&dParam1);
        json_double(token,"dParam2",&dParam2);
        json_double(token,"dParam3",&dParam3);
        json_double(token,"dParam4",&dParam4);

      }
      //M2-M9,S index 2-9
      //userButtons index 11-20
      //M100-M119 index 21 -39
      int actionIndex = indexOffset + i;
      int actionNameIndex = action > 0 && action < 10?action:10;
      printf("Set action index %d to %s\n",actionIndex, ACTION_NAMES[actionNameIndex]);
      gci->McodeActions[actionIndex].Action = action;
      gci->McodeActions[actionIndex].dParams[0] = dParam0;
      gci->McodeActions[actionIndex].dParams[1] = dParam1;
      gci->McodeActions[actionIndex].dParams[2] = dParam2;
      gci->McodeActions[actionIndex].dParams[3] = dParam3;
      gci->McodeActions[actionIndex].dParams[4] = dParam4;


      //file should be nulled if not set.
      if(file == NULL || action == 0){
        gci->McodeActions[actionIndex].String[0] = 0;
      } else {
        strcpy(gci->McodeActions[actionIndex].String, file);
      }


      //invoke user buttons with a call such as this,
      //Interpreter->InvokeAction(10,FALSE);  // resend new Speed


    } else {
      printf("Failed %s\n", pathTemplate);
    }

  }
  free(file);
  free(name);
}
