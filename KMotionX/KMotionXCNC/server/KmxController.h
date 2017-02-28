/*
 * KMXController.h
 *
 *  Created on: 17 dec. 2015
 *      Author: parhansson
 */

#ifndef KMOTIONX_KMOTIONXCNC_SERVER_KMXCONTROLLER_H_
#define KMOTIONX_KMOTIONXCNC_SERVER_KMXCONTROLLER_H_

#include "GCodeInterpreterX.h"
#include "MessageQueue.h"
#include "FileStatus.h"

//same as in KMotionCNCDlg.c
#define ACTION_CYCLE_START 25
#define ACTION_HALT 26
#define ACTION_STOP 27
#define ACTION_FEEDHOLD 28
#define ACTION_RESUME 29
#define ACTION_PROG_START 30
#define ACTION_PROG_EXIT 31


//namespace kmx {

class KmxController: public AbstractController {
public:
  KmxController(CGCodeInterpreter *interpreter);
  virtual ~KmxController();

  //Should be called by application after construction to allow for subclassing Setup()
  int Initialize();

  virtual void Shutdown();
  //Called by Initialize, Subclasses should implement and set members KmxController::settings_file_
  //and a possible default gcode file KmxController::current_gcode_file_
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

  void Jog(int axis, int speed);

  //Applications should this to acknowledge and return value from callback sent to client
  virtual void Acknowledge(int id, int returnValue);

  //Reads settings file (stored in settings_file_) from disk and calls SetMotionParams()
  //Subclasses should invoke this function whenever appropriate
  void UpdateMotionParams();

  //Called by application when client sends data, such as acknowledged messages
  virtual void OnReceiveClientData(const char * msg) {};

  //Executes callback to client. Subclasses should override and implement.
  virtual int DoCallback(const char *data){return 0;};

  //Handles message boxes. MessageBoxes might be of blocking and modal nature.
  //One must be sure not to call this from the event loop thread.
  int OnMessageBoxCallback(const char *title, const char *msg, int options);
  //Handles user message callbacks from GCode. This is a blocking call and should not be called from event loop.
  int OnUserCallback(const char *msg);
  //Handles user M code callbacks from GCode. This is a blocking call and should not be called from event loop.
  int OnMcodeUserCallback(int mCode);
  //Handles Console message callbacks. This is a non blocking call.
  int OnConsoleCallback(const char *msg);
  //Handles Error message callbacks. This is a non blocking call.
  void OnErrorMessageCallback(const char *msg);
  //Handles Status update callbacks. This is a non blocking call.
  void OnStatusCallback(int line_no, const char *msg);
  //Handles GCode Complete callbacks. This is a non blocking call.
  void OnCompleteCallback(int status, int line_no, int sequence_number,const char *err);


protected:
  MessageQueue *mq;
  CGCodeInterpreter *Interpreter;
  //status variables
  bool performPostHaltCommand;
  int currentLine;
  MAIN_STATUS main_status;
  FileStatus *settings_file;
  FileStatus *current_gcode_file;
  bool connected;
  bool interpreting;
  bool simulate;

  //Gets called by controller and clients should implement to update screen and whatever status they need.
  virtual void UpdateClient(){};

  // Called by UpdateMotionParams. Subclasses may use any format of this data an is responsible for parsing and updating motionparams
  virtual void SetMotionParams(const char *buf, size_t len){};

  //Subclasses should implement to create a meaningful message to their client and return a unique message id
  virtual int CreateMessageBoxCallbackData(const char *title, const char *msg, int options, bool blocking, char **buf){return 0;};
  //Subclasses should implement to create a meaningful message to their client and return a unique message id
  virtual int CreateCompleteCallbackData(int status, int line_no, int sequence_number,const char *err, bool blocking, char **buf){return 0;};
  //Subclasses should implement to create a meaningful message to their client and return a unique message id
  virtual int CreateStatusCallbackData(int line_no, const char *msg, bool blocking, char **buf){return 0;};
  //Subclasses should implement to create a meaningful message to their client and return a unique message id
  virtual int CreateErrorMessageCallbackData(const char *msg, bool blocking, char **buf){return 0;};
  //Subclasses should implement to create a meaningful message to their client and return a unique message id
  virtual int CreateConsoleCallbackData(const char *msg, bool blocking, char **buf){return 0;};
  //Subclasses should implement to create a meaningful message to their client and return a unique message id
  virtual int CreateUserCallbackData(const char *msg, bool blocking, char **buf){return 0;};
  //Subclasses should implement to create a meaningful message to their client and return a unique message id
  virtual int CreateMcodeUserCallbackData(int mCode, bool blocking, char **buf){return 0;};
private:

  struct timeval tval_status;
  struct timeval tval_poll_callbacks;
  CKMotionDLL *km;
  CCoordMotion *CM;

  void interpret(int BoardType,char *InFile,int start,int end,bool restart);
  void setSimulationMode(bool enable);
  int readStatus();
  bool msPast(struct timeval *tval_last, int ms);



};

//} /* namespace KMX */

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_KMXCONTROLLER_H_ */
