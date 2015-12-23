/*
 * KMXController.cpp
 *
 *  Created on: 17 dec. 2015
 *      Author: parhansson
 */

#include "KmxController.h"

#include <dirent.h>
#include "utils.h"
#include "handler.h"
//namespace kmx {

//TODO Implement support for blocking callback in poll thread

KmxController::KmxController(CGCodeInterpreter *interpreter)
/*:AbstractController()*/{
  Interpreter = interpreter;

  CM = Interpreter->CoordMotion;
  km = CM->KMotionDLL;
  mq = new MessageQueue(this);

  performPostHaltCommand = false;
  currentLine = 0;
  settings_file_[0] = '\0';
  current_gcode_file_[0] = '\0';
  connected = false;
  interpreting = false;
  simulate = Interpreter->CoordMotion->m_Simulate;

  //TODO init struct before first read. memset?
  main_status.StopImmediateState = 0;

}

KmxController::~KmxController() {
  // release token since server might still be running
  km->ReleaseToken();
  if (Interpreter) delete Interpreter;
  if (CM) delete CM;
  if (km) delete km;

}
int KmxController::Initialize(){

  int type = BOARD_TYPE_UNKNOWN;
  if(km->CheckKMotionVersion(&type,false)){
    //enter simulation mode?
    exit(1);
  }

  if(Setup()){
    printf("Setup failed.\n");
    return -1;
  }

  UpdateMotionParams();

  //Try without simulation on startup
  setSimulationMode(false);

  if(!readStatus()){
  //if (FirstStartup)
  //{
    //FirstStartup=false;
    if (Interpreter->InvokeAction(ACTION_PROG_START,FALSE))  // Special Command
    {
      AfxMessageBox("Unable to perform Startup Action");
    }
  //}

  }
  return 0;
}

int KmxController::readStatus(){
  int result = km->GetStatus(main_status,false); //already locked
  if(result){
      connected = false;
      setSimulationMode(true);
      Interpreter->Abort();
      // error reading status
      km->Failed();
      debug("GetStatus failed\n");
   } else {
      connected = true;
   }
  return result;
}


void KmxController::setSimulationMode(bool enable){
    simulate = Interpreter->CoordMotion->m_Simulate = enable;
}

int KmxController::InvokeAction(int action, bool FlushBeforeUnbufferedOperation){
    return Interpreter->InvokeAction(action,FlushBeforeUnbufferedOperation);  // invoke action number
}

void KmxController::LoadMachineConfiguration(const char* path){
  if (strcmp(settings_file_, path) != 0) {
      strcpy(settings_file_, path);
  }
}
void KmxController::LoadGcode(const char* path){
  if(!interpreting){
      if (strcmp(current_gcode_file_, path) != 0) {
          strcpy(current_gcode_file_, path);
          currentLine = 0;
      }
  }
}

void KmxController::Simulate() {
  if (!interpreting) {
    setSimulationMode(!simulate);
  }
}
void KmxController::Step(){
  if (!interpreting/* && Interpreter->GetHalt()*/) {
    interpret(2, current_gcode_file_ , currentLine, currentLine, false);
  }
}
void KmxController::Reset(){
  currentLine = 0;
  //Interpreter->CoordMotion->m_PreviouslyStopped = STOPPED_NONE;
}

void KmxController::CycleStart() {
  if (interpreting) {
    Halt();
  } else {
    interpret(2, current_gcode_file_ , currentLine, -1, true);
  }

}
void KmxController::Halt() {
  if (interpreting) {
    performPostHaltCommand=true;
    Interpreter->Halt();
  }
}

void KmxController::EmergencyStop()
{
  if(km->WaitToken(false, 1000) == KMOTION_LOCKED){
    char s[MAX_LINE];
    int i;

    for (i=1;i<N_USER_THREADS;i++)   // kill all user programs except Thread #1
    {
      sprintf(s, "Kill %d", i+1);

      if (km->WriteLine(s)){
        log_info("Command failed: %s\n", s);
        return;
      }
    }

    Interpreter->Halt();

    Interpreter->Abort();

    for (i=0;i<N_CHANNELS;i++)             // disable all servo channels
    {
      sprintf(s, "DISABLEAXIS%d",i);
      if (km->WriteLine(s)){
        log_info("Command failed: %s\n", s);
        return;
      }
    }
    km->ReleaseToken();
  }
  Interpreter->InvokeAction(ACTION_STOP,FALSE);  // Do Special Action
}
void KmxController::DoErrorMessage(const char * msg){
  km->DoErrMsg(msg);
}
void KmxController::Feedhold(){
  //TODO wait token should be done globaly when handling requests that interact with board
  //if(km->WaitToken(false,100) == KMOTION_LOCKED)){
    char res[MAX_LINE];
    //WriteLineReadLine waits for token
    if(km->WriteLineReadLine("GetStopState", res)){
      debug("Read stop state failed");
    } else  {
      //int feedHold;
      if (res[0] == '0') {
        //Not stopping
        //feedHold = 1;
        Interpreter->InvokeAction(ACTION_FEEDHOLD,FALSE);  // Do Special Action
        km->WriteLine("StopImmediate0");
      } else {
        //Stopping or stopped
        //feedHold = 0;
        Interpreter->InvokeAction(ACTION_RESUME,FALSE);  // Do Special Action
        km->WriteLine("StopImmediate1");
      }
    }
    //km->ReleaseToken();
  //}
}

void KmxController::interpret(int BoardType,char *InFile,int start,int end,bool restart) {
  if (!interpreting) {
    //These are not needed if not CheckForResume is implemented
    //Interpreter->CoordMotion->ClearAbort();
    //Interpreter->CoordMotion->m_AxisDisabled=false;
    //Interpreter->CoordMotion->ClearHalt();
    //TODO CheckForResume
    Interpreter->InvokeAction(ACTION_CYCLE_START,FALSE);  // Do Special Action
    if (!Interpreter->Interpret(BoardType, InFile, start, end, restart,
         ::StatusCallback, ::CompleteCallback)) {
      interpreting = true;
      //enqueueState();
    }
  }
}

void KmxController::UpdateMotionParams(){
  char fileName[256];
  absolutePath(settings_file_, fileName);

  MappedFile mmFile;
  if(mmapNamedFile(mmFile, fileName)){
    log_info("Failed to map file: %s", fileName);
    return;
  }
  SetMotionParams(mmFile.addr, mmFile.filesize);

  unmapFile(mmFile);
}


void KmxController::OnStatusCallback(int line_no, const char *msg) {
  //This is a non blocking call from other thread to be enqueued in poll thread
  char buf[512];
  bool blocking = false;
  memset(buf, 0,512);
  currentLine = line_no;
  int id = CreateStatusCallbackData(line_no, msg, blocking, buf, 512);
  mq->EnqueueCallback(id, buf, blocking);
}


void KmxController::OnCompleteCallback(int status, int line_no, int sequence_number,const char *err) {
  //This is a non blocking call from other thread to be enqueued in poll thread
  char buf[256];
  bool blocking = false;
  interpreting = false;
  currentLine = line_no;

  if (performPostHaltCommand)
  {
    performPostHaltCommand=false;
    InvokeAction(ACTION_HALT,FALSE);  // Do Special Action
  }

  int id = CreateCompleteCallbackData(status, line_no, sequence_number, err, blocking, buf, 256);
  if(strlen(err)>0){
    OnErrorMessageCallback(err);
  }

  mq->EnqueueCallback(id, buf, blocking);
}

void KmxController::OnErrorMessageCallback(const char *msg) {
  //This is a non blocking call enqueue in poll thread
  char buf[256];
  bool blocking = false;
  int id = CreateErrorMessageCallbackData(msg, blocking, buf, 256);
  mq->EnqueueCallback(id, buf, false);

}

int KmxController::OnConsoleCallback(const char *msg) {
  //This is a non blocking call even though it has a return value.. strange
  //Not even sure this happens in another thread
  char buf[256];
  bool blocking = false;
  int id = CreateConsoleCallbackData(msg, blocking, buf, 256);
  return mq->EnqueueCallback(id, buf, blocking);
}

int KmxController::OnMessageBoxCallback(const char *title, const char *msg, int options) {
  char buf[512];
  bool blocking = false;
  //non blocking MB_OK?
  if ((options & (MB_YESNO | MB_OKCANCEL)) == (MB_YESNO | MB_OKCANCEL)) {
    blocking = true;
  }

  //TODO Messages might be pretty long check this and alloc
  int id = CreateMessageBoxCallbackData(title, msg, options, blocking, buf, 512);
  //TODO handle blocking messagebox
  return mq->EnqueueCallback(id, buf, blocking);
}


int KmxController::OnUserCallback(const char *msg) {
  //This is a blocking call. A bit trickier
  char buf[256];
  bool blocking = true;
  int id = CreateUserCallbackData(msg, blocking, buf, 256);
  int result = mq->EnqueueCallback(id, buf, blocking);
  debug("UserCallback result: %d", result);
  return result;
}

int KmxController::OnMcodeUserCallback(int mCode) {
  //This is a blocking call. A bit trickier
  //Currently not supported.
  //1 start thread
  //2 enqueu callback to poll thread
  //3 wait for answer in poll thread
  //4 poll thread gets answer form client
  //5 mutex or whatever wait lock used is relased
  //thred
  //wait for thread join;
  //or just sleep short amount of time in a loop until condition true (answer i poll thred)
  //mutex in poll to update linked list of callbacks waiting for answer
  //the thread needs to wait for timeout and then issue the request again in poll thread
  //if gui is closed and reopened
  //instantiate a struct and wait for a signal/variable in that struct
  char buf[256];
  bool blocking = true;
  int id = CreateMcodeUserCallbackData(mCode, blocking, buf, 256);
  int result = mq->EnqueueCallback(id, buf, blocking);
  debug("UserMCallback result: %d", result);
  return result;

}

void KmxController::Acknowledge(int id, int returnValue) {
  mq->Acknowledge(id, returnValue);
}

void KmxController::Poll() {
  //poll is called every at least every 100 ms but can be alot more often
  if(msPast(&tval_poll_callbacks,200)){
    //debug("polling callbacks");
    mq->PollCallbacks();
  }

  //only perform poll when locked
  //Timeout must be short enough not to be reentering if multiple threads are calling poll
  if(km->WaitToken(false,50) == KMOTION_LOCKED){
    if(msPast(&tval_status,200)){

      if(!simulate){
        if(readStatus()){
          //return;
        }
      }
      UpdateClient();
    }

    if(!simulate){
      if(msPast(&tval_service_console,1000)){
        //debug("serving console");
        if(km->ServiceConsole()){
          //TODO not verified that this works.
          DoErrorMessage(">ServiceConsole Failed\n");
        }
      }
    }
    km->ReleaseToken();
  }
}
bool KmxController::msPast(struct timeval *tval_last, int ms){
  struct timeval tval_current, tval_result;
  if(tval_last == NULL){
    gettimeofday(tval_last, NULL);
    return true;
  }
  gettimeofday(&tval_current, NULL);
  timersub(&tval_current, tval_last, &tval_result);

  if(tval_result.tv_sec*1000000+tval_result.tv_usec > ms*1000){
    gettimeofday(tval_last, NULL);
    return true;
  }
  return false;
}

//} /* namespace KMX */
