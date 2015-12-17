/*
 * callbackhandler.h
 *
 *  Created on: 15 dec. 2015
 *      Author: parhansson
 */

#ifndef KMOTIONX_KMOTIONXCNC_SERVER_CALLBACKHANDLER_H_
#define KMOTIONX_KMOTIONXCNC_SERVER_CALLBACKHANDLER_H_
#include <stdlib.h>
#include <time.h>
#include "handler.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

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

struct callback {
  int id;
  enum cb_type type;
  enum cb_status status;
  time_t sent_time;
  int ret;
  char msg[512]; //TODO malloc correct size
  struct callback *next;
};

const char CB_NAMES[][24] = { "STATUS", "COMPLETE", "ERR_MSG",
    "CONSOLE", "USER", "USER_M_CODE", "STATE", "MESSAGEBOX" };

void cbh_init(PUSH_TO_CLIENTS *push_cb);
void cbh_print_info();
void cbh_enqueue_state(const char * content);
int cbh_enqueue_callback(const char * msg, enum cb_type type);
void cbh_poll_callbacks(const char * content);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_CALLBACKHANDLER_H_ */
