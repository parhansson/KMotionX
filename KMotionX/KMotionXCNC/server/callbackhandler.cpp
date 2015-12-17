/*
 * callbackhandler.cpp
 *
 *  Created on: 15 dec. 2015
 *      Author: parhansson
 */
#include "callbackhandler.h"
#include <string.h>
#include <pthread.h>
#include "dbg.h"


static void cbh_poll_callback(struct callback *cb, int id, int ret);
static void cbh_delete_callback(struct callback *node);
static struct callback * cbh_alloc_callback();

static const char STATUS_NAMES[][24] = {
    "CBS_ENQUEUED",
    "CBS_WAITING",
    "CBS_ACKNOWLEDGED",
    "CBS_STATE_IDLE" };
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t con = PTHREAD_COND_INITIALIZER;
static struct callback *callbacks = NULL; // head of this list is always state callback
static int callback_counter = 0; //Callback id counter.
static long int poll_TID; //Poll thread id
static PUSH_TO_CLIENTS *push_callback;
//TODO Implement support for blocking callback in poll thread
//#define isBlocking(type) (type == CB_USER || type == CB_USER_M_CODE || type ==  CB_MESSAGEBOX)
#define IS_BLOCKING(type) (type == CB_USER || type == CB_USER_M_CODE)

void cbh_init(PUSH_TO_CLIENTS *push_cb){
  //Enqueue state as first callback. This first entry will never be removed
  push_callback = push_cb;
  callbacks = (struct callback *) malloc(sizeof(struct callback));
  callbacks->next = NULL;
  poll_TID = getThreadId();

}

void cbh_poll_callbacks(const char * content) {

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
    cbh_poll_callback(current, id, ret);
  } while (node != NULL);

  pthread_mutex_unlock(&mut);
}

// if last is null new call back will be last on queue
// if last equals to "callbacks" object it will be put first on queue (head)
struct callback * cbh_init_callback(struct callback *last, const char * message, enum cb_type type) {

  if (last == NULL) {
    last = cbh_alloc_callback();
  }

  last->id = callback_counter++;
  last->status = CBS_ENQUEUED;
  last->ret = -1;
  last->type = type;
  //{ "id": 5, "type": 67, data: ""|6776|{}|[]|null|true|false}
  json_emit(last->msg, 512, "{ s: i, s: s, s: S, s: S}",
      "id", last->id,
      "type", CB_NAMES[type],
      "block",IS_BLOCKING(type)?"true":"false",
      "data", message);

  //debug("Enqueued %s message: %s",CB_NAMES[type], last->msg);
  return last;
}

//msg needs to be quoted if string
int cbh_enqueue_callback(const char * msg, enum cb_type type) {

  //Enqueued messages are held in list (callbacks)
  //Callbacks are created here and sent to clients in next poll

  struct callback * cb;
  long int curTID = getThreadId();
  int result = 0;
  //debug("Poll thread: %ld. Current thread: %ld", pollTID, curTID);
  if (poll_TID == curTID) {
    //if this is a blocking call. then we are in trouble.
    //we can not stop and wait in poll/main thread
    //TODO mutex lock here is new and not very well tested
    if (IS_BLOCKING(type)) {
        log_info("-------ERROR: Unsupported blocking callback %s in poll thread %ld\n%s\n", CB_NAMES[type], poll_TID, msg);
    }

    pthread_mutex_lock(&mut);
    cb = cbh_init_callback(NULL, msg, type);
    if (IS_BLOCKING(cb->type)) {
      result = cb->ret;
      cbh_delete_callback(cb);
    }
    pthread_mutex_unlock(&mut);
  } else {
    pthread_mutex_lock(&mut);
    cb = cbh_init_callback(NULL, msg, type);
    if (IS_BLOCKING(cb->type)) {
      debug("wait called");
      pthread_cond_wait(&con, &mut); //wait for the signal with con as condition variable
      debug("wait done signal received");
      result = cb->ret;
      cbh_delete_callback(cb);
    }
    pthread_mutex_unlock(&mut);
  }

  return result;
}
//TODO use enqueue callback instead. it's the same if callbacks is sent as hed of list.
void cbh_enqueue_state(const char* msg) {
  //Lock mutex when enqueing state.
  pthread_mutex_lock(&mut);
  //debug("enquing current file: %s", gstate.current_file);
  //this could be implemented as callback to one client (connection) only
  cbh_init_callback(callbacks, msg, CB_STATE);
  pthread_mutex_unlock(&mut);
}

void cbh_print_info(){
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
  fprintf(stdout, "Total callbacks in queue :%d\n", cb_count);
}

static void cbh_poll_callback(struct callback *cb, int id, int ret) {
  if (cb->status == CBS_ENQUEUED) {
    cb->sent_time = time(NULL);
    push_callback(/*WEBSOCKET_OPCODE_TEXT */0x1,cb->msg, strlen(cb->msg));
    cb->status = CBS_WAITING;
  } else if (cb->status == CBS_WAITING) {

    if (cb->id == id) {
      cb->status = CBS_ACKNOWLEDGED;
      cb->ret = ret;
      if (cb->type == CB_STATE) {
        cb->status = CBS_STATE_IDLE;
      } else {
        //Only signal waiting thread if blocking callback
        if (IS_BLOCKING(cb->type)) {
          pthread_cond_signal(&con); //wake up waiting thread with condition variable
        } else {
          //printf("Non blocking callback acked and deleted\n");
          cbh_delete_callback(cb);
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

static void cbh_delete_callback(struct callback *node) {
  // When node to be deleted is head node
  if (callbacks == node) {
    log_info("Not allowed to remove head of callback list");
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
      log_info("Given node is not present in Linked List ");
      return;
    }

    // Remove node from Linked List
    prev->next = prev->next->next;
  }
  // Free memory
  free(node);
}

static struct callback * cbh_alloc_callback() {
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
