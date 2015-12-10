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


int toks(const struct json_token *tk, void *value, const int size = 0);
int toki(const struct json_token *tk, int * value);
double tokd(const struct json_token *tk, double * value);
bool tokb(const struct json_token *tk, bool * value);
int handle_api(struct mg_connection *conn, enum mg_event ev, char *uri);
int handleJson(struct mg_connection *conn);
void push_message(const char *msg);
void delete_callback(struct callback *node);
void ErrMsgHandler(const char *msg);
int ConsoleHandler(const char *msg);
int MessageBoxHandler(const char *, const char *, int );
int MUserCallback(int);
int UserCallback(const char *);
void enqueueState();
struct callback * alloc_callback();
//struct callback * set_callback(struct callback *last, const char * message, enum cb_type type);

//Emits json resonse
#define EMIT_RESPONSE_PARAM(fmt,...) w += json_emit(gp_response+w, MAX_LINE-w, fmt, ##__VA_ARGS__);
#define EMIT_RESPONSE(fmt, ...) int w = 0; w = json_emit(gp_response+w, MAX_LINE, "{ s: s, s: s, s: ", "clazz", object,"func",func, "params"); \
    EMIT_RESPONSE_PARAM(fmt,##__VA_ARGS__)  \
    w += json_emit(gp_response+w, MAX_LINE-w, ", s: i }", "ret",ret);

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

//state of gui.
struct state {
  int simulate;
  int feedHold;
  int interpreting; // = 0;
  int last_line; // = 0;
  char current_file[256];
  state(): interpreting(0), last_line(0) {}
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

struct callback *callbacks = NULL; // head of this list is always state callback
//Callback id counter.
int callbackCounter = 0;

//global poll thread response array, no need to allocate eah time since server is single threaded
char gp_response[MAX_LINE];

const char STATUS_NAMES[][24] = { "CBS_ENQUEUED", "CBS_WAITING",
    "CBS_ACKNOWLEDGED", "CBS_STATE_IDLE" };

const char CB_NAMES[][24] = { "STATUS", "COMPLETE", "ERR_MSG",
    "CONSOLE", "USER", "USER_M_CODE", "STATE", "MESSAGEBOX" };


void initHandler() {
  km = new CKMotionDLL(0);
  cm = new CCoordMotion(km);
  gci = new CGCodeInterpreter(cm);
  //Enable messagebox callback by not setting this callback

  gci->McodeActions[3].Action = M_Action_Callback;
  //gci->McodeActions[4].Action = M_Action_Program_PC;
  gci->SetUserMCodeCallback(MUserCallback);
  gci->SetUserCallback(UserCallback);
  km->SetErrMsgCallback(ErrMsgHandler);

  km->SetConsoleCallback(ConsoleHandler);

  pollTID = getThreadId();
  //debug("Poll thread id %ld", pollTID);
  service_console_time = time(NULL);
  mb_callback = MessageBoxHandler;
  //Enqueue state as first callback. This first entry will never be removed
  callbacks = (struct callback *) malloc(sizeof(struct callback));
  callbacks->next = NULL;
  enqueueState();
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
    }
  }
  //debug("Pushed to %d sockets", sockets);

  fprintf(stdout, "Total callbacks in queue :%d\n", cb_count);
  fprintf(stdout, "Websockets   :%d\n", websockets);
  fprintf(stdout, "Connections  :%d\n", connections);

  fprintf(stdout, "State        :%s\n",
      gstate.interpreting ? "Interpreting" : "Stopped");
  fprintf(stdout, "File         :%s\n", gstate.current_file);
  fprintf(stdout, "Line         :%d\n", gstate.last_line);
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

void pollCallbacks(struct mg_connection *conn) {

  int id = -1;
  char type[16];
  memset(type,0,16);

  int ret = -1;
  if (conn != NULL) {
    //we have a connection.
    //incoming answer from client
    if (conn->content_len > 12 && memcmp(conn->content, "CB_ACK:", 7) == 0) {
      sscanf(conn->content, "CB_ACK: %d  %s %d", &id, type, &ret);

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
struct callback * init_callback(struct callback *last, const char * message,
    enum cb_type type) {
  if (last == NULL) {
    last = alloc_callback();
  }

  last->id = callbackCounter++;
  last->status = CBS_ENQUEUED;
  last->ret = -1;
  last->type = type;
  //{ "id": 5, "type": 67, data: ""|6776|{}|[]|null|true|false}
  json_emit(last->msg, 512, "{ s: i, s: s, s: S, s: S}", "id", last->id, "type", CB_NAMES[type],
      "block",isBlocking(type)?"true":"false" ,"data", message);

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
    cb = init_callback(NULL, msg, type);
    if (isBlocking(cb->type)) {
      log_err("-------ERROR: Blocking callback in poll thread %ld", pollTID);
      result = cb->ret;
      delete_callback(cb);
    }
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
  json_emit(buf, 256, "{ s: i, s: i, s: i, s: s }", "line", line_no, "status",
      status, "sequence", sequence_number, "message", err);
  gstate.interpreting = 0;
  gstate.last_line = line_no;
  enqueueState();
  enqueueCallback(buf, CB_COMPLETE);
}

void StatusCallback(int line_no, const char *msg) {
  //This is a non blocking call from other thread to be enqueued in poll thread
  char buf[256];
  gstate.last_line = line_no;
  json_emit(buf, 256, "{ s: i, s: s }", "line", line_no, "message", msg);
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
  int blocking;
  //non blocking MB_OK?
  if ((options & (MB_YESNO | MB_OKCANCEL)) == (MB_YESNO | MB_OKCANCEL)) {
    blocking = 1;
  }


  char buf[256];
  json_emit(buf, 256, "{ s: i, s: s, s: s }", "options", options, "title",
      title, "message", msg);
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
  char stateBuf[512];
  //debug("enquing current file: %s", gstate.current_file);
  //this could be implementedas callback to one client (connection) only
  json_emit(stateBuf, 512, "{ s: i, s: i, s: i, s: s }", "feedHold",
      gstate.feedHold, "interpreting", gstate.interpreting, "simulate",
      gstate.simulate, "file", gstate.current_file);
  init_callback(callbacks, stateBuf, CB_STATE);
}

int handle_poll(struct mg_connection *conn) {
  pollCallbacks(NULL);

  time_t current_time = time(NULL);
  if (current_time - service_console_time > 5) {
    /*
     if (km->KMotionLock() == KMOTION_LOCKED)  // see if we can get access
     {
     km->ServiceConsole();
     km->ReleaseToken();
     printf("ServiceConsole\n");
     } else {
     printf("Failed to lock for console\n");
     }
     */
    service_console_time = current_time;
  }

  //printf("event %d\n", ev);
  return MG_FALSE;
}
int handle_ws_connect(struct mg_connection *conn) {
  //if websocket /ws is param here
  //printf("event %d param: %s\n", ev, conn->uri);
  enqueueState();
  return MG_FALSE;
}
int handle_request(struct mg_connection *conn,enum mg_event ev) {
  if (conn->is_websocket) {
    pollCallbacks(conn);
    return MG_TRUE;
  } else {
    char uri[256];
    strcpy(uri, conn->uri);

    //if (strcmp(conn->uri, "/upload") == 0) {
    if (strstr(uri, "/upload") == uri) {
      FILE *fp = (FILE *) conn->connection_param;
      if (fp != NULL) {
        //This row was in the example. But should not be here
        //fwrite(conn->content, 1, conn->content_len, fp); // Write last bits

        mg_printf(conn, "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/plain\r\n"
                  "Connection: close\r\n\r\n"
                  "Written %ld of POST data to a temp file:\n\n",
                  (long) ftell(fp));

        // Temp file will be destroyed after fclose(), do something with the
        // data here -- for example, parse it and extract uploaded files.
        // As an example, we just echo the whole POST buffer back to the client.

        rewind(fp);

        int pagesize, offset, fd;
        size_t filesize, mapsize;
        caddr_t addr;

        offset = 0;
        fd = fileno(fp);
        filesize = ftell(fp);
        pagesize = getpagesize();
        mapsize = (filesize/pagesize)+pagesize; // align memory allocation with pagesize

        //memory map tmp file and parse it.
        addr = (char*)mmap((caddr_t)0, mapsize, PROT_READ, MAP_PRIVATE, fd,offset);

        const char *data;
        int data_len, ofs = 0;
        char var_name[100], file_name[100];
        while ((ofs = mg_parse_multipart(addr + ofs, mapsize - ofs,
                                         var_name, sizeof(var_name),
                                         file_name, sizeof(file_name),
                                         &data, &data_len)) > 0) {
          fprintf(stdout, "var: %s, file_name: %s, size: %d bytes\n",
                         var_name, file_name, data_len);
          FILE * pFile;
          //Add one to skip first slash and make path relative
          pFile = fopen (file_name+1, "wb");
          if(pFile){
            fwrite (data , sizeof(char), data_len, pFile);
            fclose (pFile);
          }

        }
        munmap(addr, mapsize);
        /*
        rewind(fp);
        mg_send_file_data(conn, fileno(fp));
        return MG_MORE;  // Tell Mongoose reply is not completed yet
        */
        return MG_TRUE;
      } else {
        mg_printf_data(conn, "%s", "Had no data to write...");
        return MG_TRUE; // Tell Mongoose we're done with this request
      }
    } else if (strstr(uri, "/api") == uri) {
      //Starts with /api but can be anything
      return handle_api(conn, ev, uri);
    }
  }
  return MG_FALSE;
}

// Mongoose sends MG_RECV for every received POST chunk.
// When last POST chunk is received, Mongoose sends MG_REQUEST, then MG_CLOSE.
int handle_recv(struct mg_connection *conn) {
  if (conn->is_websocket) {
    return MG_FALSE;
  } else {
    FILE *fp = (FILE *) conn->connection_param;

    // Open temporary file where we going to write data
    if (fp == NULL && ((conn->connection_param = fp = tmpfile())) == NULL) {
      return -1;  // Close connection on error
    }

    // Return number of bytes written to a temporary file: that is how many
    // bytes we want to discard from the receive buffer
    return fwrite(conn->content, 1, conn->content_len, fp);
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

int handle_api(struct mg_connection *conn, enum mg_event ev, char *uri) {

  if (conn->content == NULL) {
    //Content is NULL if request method is GET
    debug("GET is not supported.");
    return MG_FALSE;
  }

  if (strstr(uri, "/api") == uri) {
    return handleJson(conn);
  } else {
    mg_send_header(conn, "Content-Type", "text/plain");
    mg_printf_data(conn, "KMotionCNC API # %s", (char *) conn->query_string);
  }
  return MG_TRUE;
}

int toks(const struct json_token *tk, void *sptr, const int size) {
  int len = size;
  char ** ptr = (char **) sptr;
  if (tk->type == JSON_TYPE_NULL) {
    return 0;
  }
  if (*ptr == NULL && tk->type != JSON_TYPE_NULL) {
    if (len <= 0) len = tk->len;
    int mal = (sizeof(char) * (len)) + sizeof(char);
    *ptr = (char*) malloc(mal);
  }

  int min = fmin(len, tk->len);
  memcpy(*ptr, tk->ptr, min);
  memset(*ptr + min, 0, 1);

  return 0;
}
int toki(const struct json_token *tk, int * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str);
  *value = atoi(str);
  free(str);
  return *value;
}

double tokd(const struct json_token *tk, double * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str);
  sscanf(str, "%lf", value);
  free(str);
  return *value;
}
bool tokb(const struct json_token *tk, bool * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str);
  if (strcmp(str, "true") == 0) {
    *value = true;
  } else {
    *value = false;
  }
  free(str);
  return *value;
}

#define SET_AXIS(counts,accel,vel,offset)  \
  tokd(paramtoken + offset, &(counts));    \
  tokd(paramtoken + offset+2, &(accel));   \
  tokd(paramtoken + offset+4, &(vel))

//TODO To ensure no blocking callbacks are made from poll thread this
//function should be called from another thread.
int handleJson(struct mg_connection *conn) {

  json_token *data = NULL;

  char * content = strndup(conn->content, conn->content_len);
  debug("%s",content);
  free(content);

  data = parse_json2(conn->content, conn->content_len);
  if (data == NULL) {
    debug("parsing api request failed");
    return MG_FALSE;
  }
  //char * gp_response = NULL;
  const json_token *token;
  json_token *paramtoken;

  int params = 0;

  char *object = NULL;
  char *func = NULL;

  token = find_json_token(data, "clazz");
  if (token != NULL) {
    toks(token, (char**) &object);
  }

  token = find_json_token(data, "func");
  if (token != NULL) {
    toks(token, (char**) &func);
  }
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

//handles file loading. different headers
  /*
  if (!strcmp("aux", object) && FUNC_SIGP("loadGCodeFile", 1)) {
    char * file = NULL;
    toks(paramtoken, &file);
    if (file != NULL) {
      //TODO this may cause never ending loop if client has an error
      if (strlen(file) > 0 && strcmp(gstate.current_file, file) != 0) {
        strcpy(gstate.current_file, file);
        //when file changes all clients will e noticed.
        enqueueState();
      }
      mg_send_file(conn, file, NULL);  // Also could be a dir, or CGI
      free(file);
    }
    free(data);
    return MG_MORE; // It is important to return MG_MORE after mg_send_file!
  }
  */
  //Reset global response string
  gp_response[0] = '\0';
  //gp_response = (char*) malloc(sizeof(char) * ( MAX_LINE));
  int ret = 0;
  if (!strcmp("aux", object)) {
    if (FUNC_SIGP("loadGCodeFile", 1)) {
      char * file = NULL;
      toks(paramtoken, &file);
      if (file != NULL) {
        if (strlen(file) > 0 && strcmp(gstate.current_file, file) != 0) {
          strcpy(gstate.current_file, file);
          //when file changes all clients will e noticed.
          enqueueState();
        }
        free(file);
      }
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
          perror("Va fan");
        }

      }

      free(machineFile);
      free(machineData);
    } else if(FUNC_SIGP("listDir", 1)){
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
          perror ("Couldn't open the directory");
        }
      }
      free(dir);
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
      ret = km->CheckKMotionVersion(&type);
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
      toks(paramtoken + 1, &p1);
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
    if (FUNC_SIGP("Interpret", 5)) {

      // test message from poll thread
      //blocking message from pull thread should be implementet to work.
      //km->DoErrMsg("Error message from poll thread");

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
        if (gstate.interpreting == 0) {
          if (!gci->Interpret(BoardType, InFile, start, end, restart,
              StatusCallback, CompleteCallback)) {
            gstate.interpreting = 1;
            enqueueState();
          }
        }
      }

      free(InFile);

//    } else if (FUNC_SIGP("SetUserMCallback", 1)) {
//    } else if (FUNC_SIGP("SetUserMCodeCallback", 1)) {


    } else if (FUNC_SIGP("setMotionParams", 54)) {

      MOTION_PARAMS *p=gci->GetMotionParams();

      p->BreakAngle = 20;
      p->CollinearTol = 0.01;
      //p->CornerTol = m_CornerTol;
      //p->FacetAngle = m_FacetAngle;
      p->TPLookahead = 1;
      //p->RadiusA = m_RadiusA;
      //p->RadiusB = m_RadiusB;
      //p->RadiusC = m_RadiusC;

      SET_AXIS(p->CountsPerInchX, p->MaxAccelX, p->MaxVelX,5);
      SET_AXIS(p->CountsPerInchY, p->MaxAccelY, p->MaxVelY,14);
      SET_AXIS(p->CountsPerInchZ, p->MaxAccelZ, p->MaxVelZ,23);
      SET_AXIS(p->CountsPerInchA, p->MaxAccelA, p->MaxVelA,32);
      SET_AXIS(p->CountsPerInchB, p->MaxAccelB, p->MaxVelB,41);
      SET_AXIS(p->CountsPerInchC, p->MaxAccelC, p->MaxVelC,50);

      //strcpy(Interpreter->ToolFile,m_ToolFile);
      //strcpy(Interpreter->SetupFile,m_SetupFile);
      //strcpy(Interpreter->GeoFile,m_GeoFile);
      //strcpy(Interpreter->VarsFile,m_VarsFile);
      //p->DegreesA = m_DegreesA!=0;
      //p->DegreesB = m_DegreesB!=0;
      //p->DegreesC = m_DegreesC!=0;
      p->ArcsToSegs = true;;

      gci->CoordMotion->SetTPParams();

/*
#define PERSIST_MCODE(x)                                  \
    INT(Interpreter->McodeActions[x].Action);         \
    DOUBLE(Interpreter->McodeActions[x].dParams[0]);      \
    DOUBLE(Interpreter->McodeActions[x].dParams[1]);      \
    DOUBLE(Interpreter->McodeActions[x].dParams[2]);      \
    DOUBLE(Interpreter->McodeActions[x].dParams[3]);      \
    DOUBLE(Interpreter->McodeActions[x].dParams[4]);      \
    CHARS(Interpreter->McodeActions[x].String);
*/


    } else if (FUNC_SIGP("simulate", 0)) {
      if (gstate.interpreting == 0) {
        gci->CoordMotion->m_Simulate = !gci->CoordMotion->m_Simulate;
        gstate.simulate = gci->CoordMotion->m_Simulate;
        enqueueState();
        EMIT_RESPONSE("[i]", gstate.simulate);
      }
    }
  }
  mg_send_header(conn, "Content-Type", "application/json");

  //Need to send some data or connection will not be closed
  if (gp_response[0] == '\0') {
    EMIT_RESPONSE("N");
  }
  free(object);
  free(func);

  mg_printf_data(conn, "%s", gp_response);

  //free(gp_response);
  free(data);

  return MG_TRUE;
}

