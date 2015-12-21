/*
 * MessageQueue.h
 *
 *  Created on: 18 dec. 2015
 *      Author: parhansson
 */

#ifndef KMOTIONX_KMOTIONXCNC_SERVER_MESSAGEQUEUE_H_
#define KMOTIONX_KMOTIONXCNC_SERVER_MESSAGEQUEUE_H_
#include <pthread.h>
#include <vector>
#include "AbstractController.h"


enum cb_status {
  CBS_ENQUEUED, CBS_WAITING, CBS_ACKNOWLEDGED, CBS_COMPLETE
};


static const char STATUS_NAMES[][24] = {
  "CBS_ENQUEUED",
  "CBS_WAITING",
  "CBS_ACKNOWLEDGED",
  "CBS_COMPLETE"
};

struct callback {
  int id;
  enum cb_status status;
  bool blocking;
  time_t sent_time;
  int ret;
  char payload[512]; //TODO malloc correct size
};

//template<class T>
class MessageQueue {
public:
  MessageQueue(AbstractController *controller);
  virtual ~MessageQueue();

  void PollCallbacks(int id = -1, int ret = -1);
  int EnqueueCallback(int id, const char * payload, bool blocking);
  void PrintInfo();

private:
  AbstractController *controller;
  std::vector<callback> queue;
  struct callback * InitCallback(struct callback *cb, int id,  bool blocking, const char * payload);
  pthread_mutex_t mut;
  pthread_cond_t con;
  long int poll_TID; //Poll thread id


};

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_MESSAGEQUEUE_H_ */
