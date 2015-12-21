/*
 * KMXController.h
 *
 *  Created on: 17 dec. 2015
 *      Author: parhansson
 */

#ifndef KMOTIONX_KMOTIONXCNC_SERVER_KMXCONTROLLER_H_
#define KMOTIONX_KMOTIONXCNC_SERVER_KMXCONTROLLER_H_

#include "../../../GCodeInterpreter/StdAfx.h"
#include "MessageQueue.h"

//same as in KMotionCNCDlg.c
#define ACTION_CYCLE_START 25
#define ACTION_HALT 26
#define ACTION_STOP 27
#define ACTION_FEEDHOLD 28
#define ACTION_RESUME 29
#define ACTION_PROG_START 30
#define ACTION_PROG_EXIT 31

enum cb_type {
  CB_STATUS, //Non blocking callback. Called from the interpreter in different thread
  CB_COMPLETE, //Non blocking callback. Called from the interpreter in different thread
  CB_ERR_MSG,      //Non blocking callback
  CB_CONSOLE,      //Non blocking callback, event though it has return value??
  CB_USER, //Blocking callback. Called from the interpreter in different thread
  CB_USER_M_CODE, //Blocking callback. Called from the interpreter in different thread
  CB_MESSAGEBOX //Message box and AfxMessageBox
};

static const char ACTION_NAMES[][32] = { "M_Action_None", "M_Action_Setbit", "M_Action_SetTwoBits",
      "M_Action_DAC", "M_Action_Program", "M_Action_Program_wait", "M_Action_Program_wait_sync",
      "M_Action_Program_PC", "M_Action_Callback", "M_Action_Waitbit", "UNAVAILABLE" };
//namespace kmx {

class KmxController: public AbstractController {
public:
  KmxController(CGCodeInterpreter *interpreter);
  virtual ~KmxController();

  //Called after construction to allow for subclassing Setup()
  int Initialize();

  virtual int Setup(){return 0;};
  //Should be called by an intervall of say 100ms by application
  void Poll();

  //Enter simulation mode.
  void Simulate();

  //Toggle cycle start, If cycle is running calling function will halt and if halted cycle will start
  void CycleStart();

  //Halt cycle
  void Halt();

  //Emergency stop kills all active threads, halt, abort, and disable all axes
  void EmergencyStop();

  //Toggle feed hold
  void Feedhold();

  //Execute one line of gcode
  void Step();

  //Go back to line 1 in gcode
  void Reset();

  //Send an error message that will propagate to all connected clients
  void DoErrorMessage(const char * msg);

  //Invoke user defined action, button, M codes etc
  int InvokeAction(int action, bool FlushBeforeUnbufferedOperation);

  //Loads a new machine configuration file. File name will be pushed to clients in state update
  void LoadMachineConfiguration(const char* path);

  //Loads a new gcode file. File name will be pushed to clients in state update
  void LoadGcode(const char* path);

  virtual void Acknowledge(int id, int returnValue);

  //Call when a new client has connected. Subclasses should do whatever they need
  virtual void ClientConnect();

  //Read settings file from disk and updates motion parameters
  void UpdateMotionParams();

  //Called by application when client sends data, such as acknowledged messages
  virtual void OnReceiveClientData(const char * msg) {};
  virtual int PushClientData(const char *data){return 0;};
  virtual int PushClientData(const char *data , size_t data_len){return 0;};


  int OnMessageBoxCallback(const char *title, const char *msg, int options);
  int OnUserCallback(const char *msg);
  int OnMcodeUserCallback(int mCode);
  int OnConsoleCallback(const char *msg);
  void OnErrorMessageCallback(const char *msg);
  void OnStatusCallback(int line_no, const char *msg);
  void OnCompleteCallback(int status, int line_no, int sequence_number,const char *err);


protected:
  MessageQueue *mq;
  CGCodeInterpreter *Interpreter;
  //status variables
  bool performPostHaltCommand;
  int currentLine;
  MAIN_STATUS main_status;
  char current_machine[256];
  char current_file[256];
  bool connected;
  bool interpreting;
  bool simulate;

  virtual void SetMotionParams(const char *buf, size_t len){};

  virtual int CreateMessageBoxCallbackData(const char *title, const char *msg, int options, bool blocking, char *buf, size_t buf_len){return 0;};
  virtual int CreateCompleteCallbackData(int status, int line_no, int sequence_number,const char *err, bool blocking, char *buf, size_t buf_len){return 0;};
  virtual int CreateStatusCallbackData(int line_no, const char *msg, bool blocking, char *buf, size_t buf_len){return 0;};
  virtual int CreateErrorMessageCallbackData(const char *msg, bool blocking, char *buf, size_t buf_len){return 0;};
  virtual int CreateConsoleCallbackData(const char *msg, bool blocking, char *buf, size_t buf_len){return 0;};
  virtual int CreateUserCallbackData(const char *msg, bool blocking, char *buf, size_t buf_len){return 0;};
  virtual int CreateMcodeUserCallbackData(int mCode, bool blocking, char *buf, size_t buf_len){return 0;};
private:

  struct timeval tval_status;
  struct timeval tval_service_console;
  struct timeval tval_poll_callbacks;
  CKMotionDLL *km;
  CCoordMotion *CM;

  void interpret(int BoardType,char *InFile,int start,int end,bool restart);
  void setSimulationMode(bool enable);

  int readStatus();

  void push_status();
  bool msPast(struct timeval *tval_last, int ms);



};

//} /* namespace KMX */

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_KMXCONTROLLER_H_ */
