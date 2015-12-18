/*
 * MessageQueue.h
 *
 *  Created on: 18 dec. 2015
 *      Author: parhansson
 */

#ifndef KMOTIONX_KMOTIONXCNC_SERVER_MESSAGEQUEUE_H_
#define KMOTIONX_KMOTIONXCNC_SERVER_MESSAGEQUEUE_H_
#include <pthread.h>

typedef void PUSH_TO_CLIENTS(int opCode, const char *data, size_t data_len);

enum cb_type {
  CB_STATUS, //Non blocking callback. Called from the interpreter in different thread
  CB_COMPLETE, //Non blocking callback. Called from the interpreter in different thread
  CB_ERR_MSG,      //Non blocking callback
  CB_CONSOLE,      //Non blocking callback, event though it has return value??
  CB_USER, //Blocking callback. Called from the interpreter in different thread
  CB_USER_M_CODE, //Blocking callback. Called from the interpreter in different thread
  CB_STATE, // contains loaded machine configuration(trajectory planning etc) and loaded gcode file
  CB_MESSAGEBOX //Message box and AfxMessageBox
};
enum cb_status {
  CBS_ENQUEUED, CBS_WAITING, CBS_ACKNOWLEDGED, CBS_STATE_IDLE
};

static const char CB_NAMES[][24] = { "STATUS", "COMPLETE", "ERR_MSG",
    "CONSOLE", "USER", "USER_M_CODE", "STATE", "MESSAGEBOX" };

static const char STATUS_NAMES[][24] = {
  "CBS_ENQUEUED",
  "CBS_WAITING",
  "CBS_ACKNOWLEDGED",
  "CBS_STATE_IDLE" };

struct callback {
  int id;
  enum cb_type type;
  enum cb_status status;
  time_t sent_time;
  int ret;
  char msg[512]; //TODO malloc correct size
  struct callback *next;
};


class MessageQueue {
public:
  MessageQueue(PUSH_TO_CLIENTS *push_cb);
  virtual ~MessageQueue();

  void PollCallbacks(const char * content);
  void EnqueueState(const char* msg);
  //msg needs to be quoted if string
  int EnqueueCallback(const char * msg, enum cb_type type);
  void PrintInfo();

private:

  struct callback * InitCallback(struct callback *last, const char * message, enum cb_type type);
  void PollCallback(struct callback *cb, int id, int ret);
  void DeleteCallback(struct callback *node);
  struct callback * AllocCallback();

  pthread_mutex_t mut;
  pthread_cond_t con;
  struct callback *callbackQueue; // head of this list is always state callback
  int callback_counter = 0; //Callback id counter.
  long int poll_TID; //Poll thread id

  PUSH_TO_CLIENTS *push_callback;


};

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_MESSAGEQUEUE_H_ */
