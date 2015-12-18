#include "handler.h"
#include "KmxController.h"
#include "WebController.h"
#include "MessageQueue.h"


WebController *kmxCtrl;
CGCodeInterpreter *Interpreter;
CKMotionDLL *km;
CCoordMotion *CM;
MessageQueue *mq;

void initHandler() {
  mq = new MessageQueue(push_to_clients);
  km = new CKMotionDLL(0);
  CM = new CCoordMotion(km);
  Interpreter = new CGCodeInterpreter(CM);

  kmxCtrl = new WebController(Interpreter,mq, server);
  //Set callbacks after initialising KmxController
  mb_callback = MessageBoxHandler;
  Interpreter->SetUserMCodeCallback(MUserCallback);
  Interpreter->SetUserCallback(UserCallback);
  km->SetErrMsgCallback(ErrMsgHandler);
  km->SetConsoleCallback(ConsoleHandler);
}

int ev_handler(struct mg_connection *conn, enum mg_event ev) {

  switch (ev) {
    case MG_AUTH:       return MG_TRUE;
    case MG_POLL:       return kmxCtrl->OnEventPoll(conn);
    case MG_REQUEST:    return kmxCtrl->OnEventRequest(conn);
    case MG_WS_CONNECT: return kmxCtrl->OnEventWsConnect(conn);
    case MG_RECV:       return kmxCtrl->OnEventRecv(conn);
    case MG_CLOSE:      return kmxCtrl->OnEventClose(conn); // MG_CLOSE event is not raised for upload post, Called manually
    default:            return MG_FALSE;
  }

}

void info_handler(int signum) {
  kmxCtrl->PrintInfo();
}

void CompleteCallback(int status, int line_no, int sequence_number,const char *err) {
  kmxCtrl->OnCompleteCallback(status, line_no, sequence_number, err);
}

void StatusCallback(int line_no, const char *msg) {
  kmxCtrl->OnStatusCallback(line_no, msg);
}

void ErrMsgHandler(const char *msg) {
  kmxCtrl->OnErrorMessageCallback(msg);
}

int ConsoleHandler(const char *msg) {
  return kmxCtrl->OnConsoleCallback(msg);
}

//(Afx)MessageBox e implementet as callback.
//only canon_stand_alone.cpp is using the message box result value
//hence maybe blocking call to this is unneccessary
int MessageBoxHandler(const char *title, const char *msg, int options) {
  return kmxCtrl->OnMessageBoxCallback(title, msg, options);
}

int UserCallback(const char *msg) {
  return kmxCtrl->OnUserCallback(msg);
}
int MUserCallback(int mCode) {
  return kmxCtrl->OnMcodeUserCallback(mCode);
}

void push_to_clients(int opCode, const char *data , size_t data_len) {
  //We know this is text;
  kmxCtrl->PushClientData(data);
}


