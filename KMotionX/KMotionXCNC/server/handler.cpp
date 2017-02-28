#include "handler.h"
#include "GCodeInterpreterX.h"
#include "KmxController.h"
#include "WebController.h"


WebController *kmxCtrl;
CGCodeInterpreter *Interpreter;
CKMotionDLL *km;
CCoordMotion *CM;
bool running = false;

void initHandler() {
  km = new CKMotionDLL(0);
  CM = new CCoordMotion(km);
  Interpreter = new CGCodeInterpreter(CM);
  kmxCtrl = new WebController(Interpreter,server);
  //Set callbacks after constructing KmxController
  mb_callback = MessageBoxHandler;
  Interpreter->SetUserMCodeCallback(MUserCallback);
  Interpreter->SetUserCallback(UserCallback);
  km->SetErrMsgCallback(ErrMsgHandler);
  km->SetConsoleCallback(ConsoleHandler);
  kmxCtrl->Initialize();
  running = true;
}

int ev_handler(struct mg_connection *conn, enum mg_event ev) {

  //On signal exit there might be a chance we get here when kmxCtrl is deleted since server can execute timeout on select()
  //Shouldn't happen though, since teardown() is executed after server has exited loop and current version is single threaded
  if(kmxCtrl == NULL || !running){
    return MG_FALSE;
  }
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
  //TODO printf should not be used within signal handlers
  kmxCtrl->PrintInfo();
}

void teardown(){
  running = false;
  kmxCtrl->Shutdown();
  delete kmxCtrl;
  kmxCtrl = NULL;
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

//(Afx)MessageBox implementet as callback.
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

