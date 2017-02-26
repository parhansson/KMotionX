/*
 * MessageQueue.cpp
 *
 *  Created on: 18 dec. 2015
 *      Author: parhansson
 */

#include "MessageQueue.h"
#include "CMutex.h"
#include <stdlib.h>
#include "dbg.h"


MessageQueue::MessageQueue(AbstractController *ctrl) {
  mut = PTHREAD_MUTEX_INITIALIZER;
  con = PTHREAD_COND_INITIALIZER;
  controller = ctrl;
  poll_TID = getThreadId("MessageQueue::MessageQueue");
}

MessageQueue::~MessageQueue() {

}


struct callback * MessageQueue::InitCallback(struct callback *item, int mid, bool blocking, const char * payload) {
  item->id = mid;
  item->status = CBS_ENQUEUED;
  item->blocking = blocking;
  item->ret = -1;
  strcpy(item->payload, payload);
  return item;

}

void MessageQueue::PollCallbacks(){
  MaintainQueue();
}

//Clears a waiting callback state. If the callback was blocking the calling thread to EnqueueCallback will awakened.
void MessageQueue::Acknowledge(int id, int results){
  MaintainQueue(id, results);
}

int MessageQueue::EnqueueCallback(int id, const char * payload, bool blocking) {

  //Enqueued messages are held in list (callbacks)
  //Callbacks are created here and sent to clients in next poll

  int result = 0;
  bool sameThread = poll_TID == getThreadId("MessageQueue::EnqueueCallback");
  bool waitForResult = blocking;

  if (sameThread && waitForResult) {
    log_info("WARNING: Blocking callback in poll thread is unsupported and will be set to nonblocking %ld\nPayload: %s\n",  poll_TID, payload);
    waitForResult = false;
  }

  pthread_mutex_lock(&mut);
  callback cb = callback();
  InitCallback(&cb, id, waitForResult, payload);
  queue.push_back(cb);
  if (waitForResult) {
    debug("wait called");
    pthread_cond_wait(&con, &mut); //wait for the signal with con as condition variable
    debug("wait done signal received");
    //vector::push_back copies our object hence we need to find the actual object in vector.
    //mayby a vector of pointers should be used instead
    //then again this happens for blocking calls only and is not a bigg issue.
    for (std::vector<callback>::iterator it = queue.begin() ; it != queue.end();++it)
    {
      if(it->id == id){
        it->status = CBS_COMPLETE;
        result = it->ret;
        break;
      }
    }
  }
  pthread_mutex_unlock(&mut);


  return result;
}

void MessageQueue::MaintainQueue(int id, int results) {

  pthread_mutex_lock(&mut);
  for (std::vector<callback>::iterator it = queue.begin() ; it != queue.end();)
  {
    switch ( it->status ) {
    case CBS_ENQUEUED:
      it->sent_time = time(NULL);
      if(controller->DoCallback(it->payload)) {
        it->status = CBS_WAITING;
      }
      break;
    case CBS_WAITING:
      if (it->id == id) {
        it->status = CBS_ACKNOWLEDGED;
        it->ret = results;
        //Only signal waiting thread if blocking callback
        if (it->blocking) {
          pthread_cond_signal(&con); //wake up waiting thread with condition variable
        } else {
          it->status = CBS_COMPLETE; //We are done with this item
        }
      } else {
        //resend message if not acked for 30 seconds
        //This will prevent hanging threads if client is closed.
        //When a new client is connected the message will show up and can be acked
        time_t current_time = time(NULL);
        if (current_time - it->sent_time > 30) {
          it->status = CBS_ENQUEUED;
        }
      }
      break;
    case CBS_ACKNOWLEDGED:
      //Do nothing. we need this state if the same message is acked more than once due to multiple clients
      //we are not allowed to block thread again.
      break;
    case CBS_COMPLETE:
        // will be deleted
        break;
    default:
      break;
    }

    //Delete callback if completed
    if(it->status == CBS_COMPLETE){
      it = queue.erase(it);
    } else {
      ++it;
    }
  }
  pthread_mutex_unlock(&mut);
}

void MessageQueue::PrintInfo(){

  int cb_count = 0;
  int statCounts[4][1];
  memset(statCounts, 0, sizeof(statCounts));
  //Need to lock mutex here or print might crash
  pthread_mutex_lock(&mut);
  memset(statCounts, 0, sizeof(statCounts));
  for (std::vector<callback>::iterator it = queue.begin() ; it != queue.end(); ++it)
  {
    statCounts[it->status][0]++;
    cb_count++;
  }


  pthread_mutex_unlock(&mut);
  for (int status = 0; status < 4; status++) {
      int count = statCounts[status][0];
      if (count > 0) {
        fprintf(stdout, "Number of callbacks %d in state %s, \n", count, STATUS_NAMES[status]);
      }
  }
  fprintf(stdout, "Total callbacks in queue :%d\n", cb_count);
}



