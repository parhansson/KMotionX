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

static const char ACTION_NAMES[][32] = { "M_Action_None", "M_Action_Setbit", "M_Action_SetTwoBits",
      "M_Action_DAC", "M_Action_Program", "M_Action_Program_wait", "M_Action_Program_wait_sync",
      "M_Action_Program_PC", "M_Action_Callback", "M_Action_Waitbit", "UNAVAILABLE" };
//namespace kmx {

class KmxController {
public:
  KmxController(CGCodeInterpreter *interpreter, MessageQueue *message_queue);
  virtual ~KmxController();

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

  //Call when a new client has connected to force push of state to the new(all) clients
  //(Deprecated) clients will get state with binary protocol
  void ClientConnect();

  //Read settings file from disk and updates motion parameters
  void UpdateMotionParams();

  //Called by application when client sends data, such as acknowledged messages
  void OnReceiveClientData(const char * msg);
  virtual void PushClientData(const char *data);
  virtual void PushClientData(const char *data , size_t data_len);

  int OnMessageBoxCallback(const char *title, const char *msg, int options);
  int OnUserCallback(const char *msg);
  int OnMcodeUserCallback(int mCode);
  int OnConsoleCallback(const char *msg);
  void OnErrorMessageCallback(const char *msg);
  void OnStatusCallback(int line_no, const char *msg);
  void OnCompleteCallback(int status, int line_no, int sequence_number,const char *err);

  //Loads a new machine configuration file. File name will be pushed to clients in state update
  void LoadMachineConfiguration(const char* path);

  //Loads a new gcode file. File name will be pushed to clients in state update
  void LoadGcode(const char* path);


protected:
  //status variables
  bool performPostHaltCommand;
  int currentLine;
  MAIN_STATUS main_status;
  char current_machine[256];
  char current_file[256];
  bool connected;
  bool interpreting;
  bool simulate;
  MessageQueue *mq;

private:

  struct timeval tval_status;
  struct timeval tval_service_console;
  struct timeval tval_poll_callbacks;
  CGCodeInterpreter *Interpreter;
  CKMotionDLL *km;
  CCoordMotion *CM;

  void interpret(int BoardType,char *InFile,int start,int end,bool restart);
  void setInterpreterActionParams(struct json_token *jsontoken, int indexOffset, int count, const char* pathTemplate);
  void setMotionParams(struct json_token *jsontoken);
  void setSimulationMode(bool enable);

  int readStatus();
  void readSetup();
  void push_status();

  //Will be removed or reborn in another shape
  void _enqueueState();
  bool msPast(struct timeval *tval_last, int ms);



};

//} /* namespace KMX */

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_KMXCONTROLLER_H_ */
