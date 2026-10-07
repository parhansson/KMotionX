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

//A callback consists of an id, payload and returnvalue. The rest is only internal states
//I meant to make this class a template class and let users decide the payload.For now the payload is a simple char array
struct callback {
  int id;
  enum cb_status status;
  bool blocking;
  time_t sent_time;
  int ret;
  char * payload;
};

//template<class T> //In the future this might be implemented
class MessageQueue {
public:
  MessageQueue(AbstractController *controller);
  virtual ~MessageQueue();

  //Checks all states of all callbacks in the queue and maintaines the queue by sending, resending and deleting callbacks
  void PollCallbacks();

  //Clears a waiting callback state. If the callback was blocking the calling thread to EnqueueCallback will awakened.
  void Acknowledge(int id, int results);

  //Enqueues a callback to be sent on next poll event, if blocking is true.
  //This method will block current thread until the response has come back and PollCallbacks
  //has been called with the same message id.
  //Enqueued callbacks that has not been acknowledged in 30 seconds will be resent.
  int EnqueueCallback(int id, const char * payload, bool blocking);

  //Prints info about the queue.
  void PrintInfo();

private:
  AbstractController *controller; //Controller that is used to actually send callbacks to clients or wherever they might be needed-

  std::vector<callback> queue; //The queue i realized in a vector. Not sure if this is the best type for this.
  pthread_mutex_t mut;  //Mutex to controll access to queue.
  pthread_cond_t con; //Condition variable to keep threads waiting if a blocking callback is enqueud
  long int poll_TID; //Poll thread id

  struct callback * InitCallback(struct callback *cb, int id,  bool blocking, const char * payload);
  void MaintainQueue(int id = -1, int results = -1);


};

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_MESSAGEQUEUE_H_ */
