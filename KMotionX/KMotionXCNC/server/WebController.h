/*
 * WebController.h
 *
 *  Created on: 18 dec. 2015
 *      Author: parhansson
 */

#ifndef KMOTIONX_KMOTIONXCNC_SERVER_WEBCONTROLLER_H_
#define KMOTIONX_KMOTIONXCNC_SERVER_WEBCONTROLLER_H_

#include "KmxController.h"
#include "mongoose.h"
#include "MessageQueue.h"

class WebController: public KmxController {
public:
  WebController(CGCodeInterpreter *interpreter, MessageQueue *message_queue, mg_server *serv);
  virtual ~WebController();
  void PrintInfo();
  int OnEventPoll(struct mg_connection *conn);
  int OnEventWsConnect(struct mg_connection *conn);
  int OnEventClose(struct mg_connection *conn);
  int OnEventRecv(struct mg_connection *conn);
  int OnEventRequest(struct mg_connection *conn);

  virtual int PushClientData(const char *data);
  virtual int PushClientData(const char *data , size_t data_len);
private:
  mg_server *server;
  //global poll thread response array, no need to allocate eah time since server is single threaded
  char gp_response[MAX_LINE];
  void ListDir(struct json_token *paramtoken);
  int HandleApiRequest(struct mg_connection *conn);
  int HandleJsonRequest(struct mg_connection *conn, const char *object, const char *func);
  void HandleUploadRequest(struct mg_connection *conn);
  int HandleUploadReceiveEvent(struct mg_connection *conn);
  int PushClientData(int opCode, const char *data , size_t data_len);
  bool isUploadRequest(struct mg_connection *conn);
  bool isApiRequest(struct mg_connection *conn);

};

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_WEBCONTROLLER_H_ */
