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
#define MAX_RESPONSE MAX_LINE*4 
enum cb_type {
  CB_STATUS, //Non blocking callback. Called from the interpreter in different thread
  CB_COMPLETE, //Non blocking callback. Called from the interpreter in different thread
  CB_ERR_MSG,      //Non blocking callback
  CB_CONSOLE,      //Non blocking callback, event though it has return value??
  CB_USER, //Blocking callback. Called from the interpreter in different thread
  CB_USER_M_CODE, //Blocking callback. Called from the interpreter in different thread
  CB_MESSAGEBOX //Message box and AfxMessageBox
};

class WebController: public KmxController {
public:
  WebController(CGCodeInterpreter *interpreter, mg_server *serv);
  virtual ~WebController();

  void PrintInfo();
  int OnEventPoll(struct mg_connection *conn);
  int OnEventWsConnect(struct mg_connection *conn);
  int OnEventClose(struct mg_connection *conn);
  int OnEventRecv(struct mg_connection *conn);
  int OnEventRequest(struct mg_connection *conn);

  virtual void OnReceiveClientData(const char * content);
  virtual int DoCallback(const char *data);

protected:
  virtual int Setup();
  virtual void UpdateClient();
  virtual int CreateMessageBoxCallbackData(const char *title, const char *msg, int options, bool blocking, char **buf);
  virtual int CreateCompleteCallbackData(int status, int line_no, int sequence_number,const char *err, bool blocking, char **buf);
  virtual int CreateStatusCallbackData(int line_no, const char *msg, bool blocking, char **buf);
  virtual int CreateErrorMessageCallbackData(const char *msg, bool blocking, char **buf);
  virtual int CreateConsoleCallbackData(const char *msg, bool blocking, char **buf);
  virtual int CreateUserCallbackData(const char *msg, bool blocking, char **buf);
  virtual int CreateMcodeUserCallbackData(int mCode, bool blocking, char **buf);
  virtual void SetMotionParams(const char *buf, size_t len);

private:
  mg_server *server;
  int callback_counter = 0; //Callback id counter.
  //global poll thread response array, no need to allocate eah time since server is single threaded
  char gp_response[MAX_RESPONSE];
  void ListDir(struct json_token *paramtoken);
  int OpenFile(struct mg_connection *conn, struct json_token *paramtoken);
  int HandleApiRequest(struct mg_connection *conn);
  int HandleJsonRequest(struct mg_connection *conn, const char *object, const char *func);
  void HandleUploadRequest(struct mg_connection *conn);
  int HandleUploadReceiveEvent(struct mg_connection *conn);
  int PushClientData(int opCode, const char *data , size_t data_len);
  bool isRegisteredRoute(struct mg_connection *conn);
  bool isUploadRequest(struct mg_connection *conn);
  bool isApiRequest(struct mg_connection *conn);
  void setInterpreterActionParams(struct json_token *jsontoken, int indexOffset, int count, const char* pathTemplate);

};

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_WEBCONTROLLER_H_ */
