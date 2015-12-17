/*
 * KMXController.cpp
 *
 *  Created on: 17 dec. 2015
 *      Author: parhansson
 */

#include "KmxController.h"

#include <dirent.h>
#include "frozen.h"
#include "utils.h"
#include "json.h"
#include "callbackhandler.h"

//namespace kmx {

KmxController::KmxController(CGCodeInterpreter *interpreter) {
  //TODO Need to send
  mb_callback = MessageBoxHandler;
  //TODO Reimplement callback handler in C++ and take constructor parameter
  cbh_init(push_to_clients);
  Interpreter = interpreter;
  CM = Interpreter->CoordMotion;
  km = CM->KMotionDLL;


  int type = BOARD_TYPE_UNKNOWN;
  if(km->CheckKMotionVersion(&type,false)){
    //enter simulation mode?
    exit(1);
  }
  readSetup();
  UpdateMotionParams();

  //Try without simulation on startup
  setSimulationMode(false); //enques state

  if(readStatus()){
  //if (FirstStartup)
  //{
    //FirstStartup=false;
    if (Interpreter->InvokeAction(ACTION_PROG_START,FALSE))  // Special Command
    {
      AfxMessageBox("Unable to perform Startup Action");
    }
  //}

  }

}

KmxController::~KmxController() {
  // release token since server might still be running
  km->ReleaseToken();
  if (Interpreter) delete Interpreter;
  if (CM) delete CM;
  if (km) delete km;

}


void KmxController::readSetup(){

  char fileName[256];
  absolutePath("settings/setup.cnf", fileName);
  MappedFile mmFile;
  if(mmapNamedFile(mmFile, fileName)){
    debug("Failed to read setup file");
    return;
  }

  json_token *setup = NULL;
  setup = parse_json2(mmFile.addr, mmFile.filesize);

  json_str(setup,"machine",current_machine);
  json_str(setup,"gcodefile",current_file);

  performPostHaltCommand=false;
  interpreting = false;
  currentLine = 0; //Store current line in setup file? need to implement store file first
  connected = false;
  simulate = false;

  free(setup);
  unmapFile(mmFile);
}

int KmxController::readStatus(){
  int result = km->GetStatus(main_status,true);
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
#define MEMCPY(to, from, size) memcpy(to, from, size); to += size;

void KmxController::_enqueueState() {
  //TODO gcode file and machine should be sent in regular update
  char stateBuf[512];
  json_emit(stateBuf, 512, "{s: s, s: s }",
      "file", current_file,
      "machine", current_machine);
  cbh_enqueue_state(stateBuf);
}

void KmxController::push_status() {

  if(!simulate){
    if(readStatus()){
      //return;
    }
  }

  //copy struct and avoid padding wich may differ on platforms
  size_t size = 456 + 6*8 + 4*4;//sizeof(main_status);
  char * msg = (char*)malloc(size);
  char * msgPtr = msg;
  //client does not expect sizeof(int) but exaclyt 4 bytes for an int
  MEMCPY(msgPtr, &main_status.VersionAndSize, 4);
  MEMCPY(msgPtr, &main_status.ADC, 96);
  MEMCPY(msgPtr, &main_status.DAC, 32);
  MEMCPY(msgPtr, &main_status.PWM, 64);
  //struct padding 4 avoided here
  MEMCPY(msgPtr, &main_status.Position, 64);
  MEMCPY(msgPtr, &main_status.Dest, 64);
  MEMCPY(msgPtr, &main_status.OutputChan0, 8);
  MEMCPY(msgPtr, &main_status.InputModes, 4);
  MEMCPY(msgPtr, &main_status.InputModes2, 4);
  MEMCPY(msgPtr, &main_status.OutputModes, 4);
  MEMCPY(msgPtr, &main_status.OutputModes2, 4);
  MEMCPY(msgPtr, &main_status.Enables, 4);
  MEMCPY(msgPtr, &main_status.AxisDone, 4);
  MEMCPY(msgPtr, &main_status.BitsDirection, 8);
  MEMCPY(msgPtr, &main_status.BitsState, 8);
  MEMCPY(msgPtr, &main_status.SnapBitsDirection0, 4);
  MEMCPY(msgPtr, &main_status.SnapBitsDirection1, 4);
  MEMCPY(msgPtr, &main_status.SnapBitsState0, 4);
  MEMCPY(msgPtr, &main_status.SnapBitsState1, 4);
  MEMCPY(msgPtr, &main_status.KanalogBitsStateInputs, 4);
  MEMCPY(msgPtr, &main_status.KanalogBitsStateOutputs, 4);
  MEMCPY(msgPtr, &main_status.RunOnStartUp, 4);
  MEMCPY(msgPtr, &main_status.ThreadActive, 4);
  MEMCPY(msgPtr, &main_status.StopImmediateState, 4);
  //struct padding 4 avoided here
  if(connected && !simulate){
    MEMCPY(msgPtr, &main_status.TimeStamp, 8);
  } else {
    time_t service_console_time = time(NULL);
    double timeStamp = service_console_time;
    MEMCPY(msgPtr, &timeStamp, 8);
  }
  MEMCPY(msgPtr, &main_status.PC_comm, 32);
  MEMCPY(msgPtr, &main_status.VirtualBits, 4);
  MEMCPY(msgPtr, &main_status.VirtualBitsEx0, 4);
  //total size 456 instead of 464 with padding

  double x,y,z,a,b,c;
  if(connected && !simulate){

    double ActsDest[MAX_ACTUATORS];

    for (int i=0; i<MAX_ACTUATORS; i++) ActsDest[i]=0.0;

    if (CM->x_axis >=0) ActsDest[CM->x_axis] = main_status.Dest[CM->x_axis];
    if (CM->y_axis >=0) ActsDest[CM->y_axis] = main_status.Dest[CM->y_axis];
    if (CM->z_axis >=0) ActsDest[CM->z_axis] = main_status.Dest[CM->z_axis];
    if (CM->a_axis >=0) ActsDest[CM->a_axis] = main_status.Dest[CM->a_axis];
    if (CM->b_axis >=0) ActsDest[CM->b_axis] = main_status.Dest[CM->b_axis];
    if (CM->c_axis >=0) ActsDest[CM->c_axis] = main_status.Dest[CM->c_axis];

    CM->Kinematics->TransformActuatorstoCAD(ActsDest,&x,&y,&z,&a,&b,&c);

    bool MachineCoords = true;
    if (MachineCoords){
      Interpreter->ConvertAbsoluteToMachine(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);
    } else {
      Interpreter->ConvertAbsoluteToInterpreterCoord(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);
    }

  } else {
    x = CM->current_x;
    y = CM->current_y;
    z = CM->current_z;
    a = CM->current_a;
    b = CM->current_b;
    c = CM->current_c;
    Interpreter->ConvertAbsoluteToMachine(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);
  }

  MEMCPY(msgPtr, &x, 8);
  MEMCPY(msgPtr, &y, 8);
  MEMCPY(msgPtr, &z, 8);
  MEMCPY(msgPtr, &a, 8);
  MEMCPY(msgPtr, &b, 8);
  MEMCPY(msgPtr, &c, 8);
  MEMCPY(msgPtr, &connected, 1);
  MEMCPY(msgPtr, &simulate, 1);
  MEMCPY(msgPtr, &interpreting, 1);
  MEMCPY(msgPtr, &currentLine, 4);
/*
  if (Interpreter->p_setup->distance_mode==MODE_ABSOLUTE)
    CheckRadioButton(IDC_Rel,IDC_Abs,IDC_Abs);
  else
    CheckRadioButton(IDC_Rel,IDC_Abs,IDC_Rel);

  if (Interpreter->p_setup->length_units==CANON_UNITS_INCHES)
    CheckRadioButton(IDC_mm,IDC_inch,IDC_inch);
  else if (Interpreter->p_setup->length_units==CANON_UNITS_MM)
    CheckRadioButton(IDC_mm,IDC_inch,IDC_mm);
*/
  //TODO needs to be a call back or implemented in subclass
  push_to_clients(WEBSOCKET_OPCODE_BINARY, msg, size);

  free(msg);
}



void KmxController::setSimulationMode(bool enable){
  bool changed = enable != simulate;
  simulate = Interpreter->CoordMotion->m_Simulate = enable;
  if(changed){
    //enqueueState();
  }
}

int KmxController::InvokeAction(int action, bool FlushBeforeUnbufferedOperation){
    return Interpreter->InvokeAction(action,FlushBeforeUnbufferedOperation);  // invoke action number
}

void KmxController::LoadMachineConfiguration(const char* path){
  if(current_machine != NULL){
    if (strcmp(current_machine, path) != 0) {
        strcpy(current_machine, path);
        //when file changes all clients will e noticed.
        _enqueueState();
    }
  }
}
void KmxController::LoadGcode(const char* path){
  if(!interpreting){
    if(current_file != NULL){
      if (strcmp(current_file, path) != 0) {
          strcpy(current_file, path);
          currentLine = 0;
          //when file changes all clients will e noticed.
          _enqueueState();
      }
    }
  }
}

void KmxController::ClientConnect(){
  _enqueueState();
}

void KmxController::Simulate() {
  if (!interpreting) {
    setSimulationMode(!simulate);
  }
}
void KmxController::Step(){
  if (!interpreting/* && Interpreter->GetHalt()*/) {
    interpret(2, current_file , currentLine, currentLine, false);
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
    interpret(2, current_file , currentLine, -1, true);
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

  Interpreter->InvokeAction(ACTION_STOP,FALSE);  // Do Special Action
}
void KmxController::DoErrorMessage(const char * msg){
  km->DoErrMsg(msg);
}
void KmxController::Feedhold(){
  //TODO wait token should be done globaly when handling requests that interact with board
  //if(km->WaitToken(false,100.0 == KMOTION_LOCKED)){
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
    km->ReleaseToken();
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
        StatusCallback, CompleteCallback)) {
      interpreting = true;
      //enqueueState();
    }
  }
}

void KmxController::UpdateMotionParams(){
  char fileName[256];
  absolutePath(current_machine, fileName);

  MappedFile mmFile;
  if(mmapNamedFile(mmFile, fileName)){
    return;
  }

  json_token *json = NULL;
  json = parse_json2(mmFile.addr, mmFile.filesize);
  setMotionParams(json);
  free(json);
  unmapFile(mmFile);
}
void KmxController::setMotionParams(struct json_token *jsontoken) {

    char * name = NULL;
    json_token *token;

    MOTION_PARAMS *p=Interpreter->GetMotionParams();
    double maxAccel,maxVel,countsPerUnit;
    double breakAngle,collinearTol,cornerTol,facetAngle,tpLookahead;

    breakAngle = collinearTol = cornerTol = facetAngle = tpLookahead = 0;
    json_double(jsontoken,"tplanner.breakangle",&breakAngle);
    json_double(jsontoken,"tplanner.cornertolerance",&cornerTol);
    json_double(jsontoken,"tplanner.lookahead",&tpLookahead);
    json_double(jsontoken,"tplanner.collineartolerance",&collinearTol);
    json_double(jsontoken,"tplanner.facetangle",&facetAngle);

    p->BreakAngle = breakAngle;
    p->CollinearTol = collinearTol;
    p->CornerTol = cornerTol;
    p->FacetAngle = facetAngle;
    p->TPLookahead = tpLookahead;

    //TODO
    //p->RadiusA = m_RadiusA;
    //p->RadiusB = m_RadiusB;
    //p->RadiusC = m_RadiusC;

    char path[64];
    for(int i= 0;i<6;i++){

      sprintf(path,"axes[%i]",i);
      token = find_json_token(jsontoken, path);
      if(token){
        maxAccel = maxVel = countsPerUnit = 0.0;
        json_str(token,"name",&name,1);
        json_double(token,"countsPerUnit",&countsPerUnit);
        json_double(token,"maxAccel",&maxAccel);
        json_double(token,"maxVel",&maxVel);

        // default values form KMotionCNCDlg.c
        maxAccel = maxAccel == 0.0?0.01:maxAccel;
        maxVel = maxVel == 0.0?0.1:maxVel;
        //Zero on countPerUnit will abort GCode
        countsPerUnit = countsPerUnit == 0.0?100.0:countsPerUnit;
        switch (name[0]) {
          case 'X':
            p->CountsPerInchX = countsPerUnit;
            p->MaxAccelX = maxAccel;
            p->MaxVelX = maxVel;
            break;
          case 'Y':
            p->CountsPerInchY = countsPerUnit;
            p->MaxAccelY = maxAccel;
            p->MaxVelY = maxVel;
            break;
          case 'Z':
            p->CountsPerInchZ = countsPerUnit;
            p->MaxAccelZ = maxAccel;
            p->MaxVelZ = maxVel;
            break;
          case 'A':
            p->CountsPerInchA = countsPerUnit;
            p->MaxAccelA = maxAccel;
            p->MaxVelA = maxVel;
            break;
          case 'B':
            p->CountsPerInchB = countsPerUnit;
            p->MaxAccelB = maxAccel;
            p->MaxVelB = maxVel;
            break;
          case 'C':
            p->CountsPerInchC = countsPerUnit;
            p->MaxAccelC = maxAccel;
            p->MaxVelC = maxVel;
            break;
        }

      } else {
        printf("Failed %s\n",path);
      }

    }
    free(name);
    //M2-M9,S index 2-9
    //userButtons index 11-20
    //M100-M119 index 21 -39
    //Special actions index 41 -48
    setInterpreterActionParams(jsontoken, 0,MAX_MCODE_ACTIONS_M1,"actions[%i]");
    setInterpreterActionParams(jsontoken, MCODE_ACTIONS_SPECIAL_OFFSET,MAX_MCODE_ACTIONS_SPECIAL,"specialActions[%i]");
    setInterpreterActionParams(jsontoken, MAX_MCODE_ACTIONS_M1,MAX_MCODE_ACTIONS_BUTTONS,"userActions[%i]");
    setInterpreterActionParams(jsontoken, MCODE_ACTIONS_M100_OFFSET,MAX_MCODE_ACTIONS_M100,"extendedActions[%i]");

    printf("X %lf, %lf, %lf \n",p->CountsPerInchX,p->MaxAccelX, p->MaxVelX );
    printf("Y %lf, %lf, %lf \n",p->CountsPerInchY,p->MaxAccelY, p->MaxVelY );
    printf("Z %lf, %lf, %lf \n",p->CountsPerInchZ,p->MaxAccelZ, p->MaxVelZ );
    printf("A %lf, %lf, %lf \n",p->CountsPerInchA,p->MaxAccelA, p->MaxVelA );
    printf("B %lf, %lf, %lf \n",p->CountsPerInchB,p->MaxAccelB, p->MaxVelB );
    printf("C %lf, %lf, %lf \n",p->CountsPerInchC,p->MaxAccelC, p->MaxVelC );

    //TODO
    //strcpy(Interpreter->ToolFile,m_ToolFile);
    //strcpy(Interpreter->SetupFile,m_SetupFile);
    //strcpy(Interpreter->GeoFile,m_GeoFile);
    //strcpy(Interpreter->VarsFile,m_VarsFile);
    //p->DegreesA = m_DegreesA!=0;
    //p->DegreesB = m_DegreesB!=0;
    //p->DegreesC = m_DegreesC!=0;
    p->ArcsToSegs = true;;

    Interpreter->CoordMotion->SetTPParams();

}
void KmxController::setInterpreterActionParams(struct json_token *jsontoken, int indexOffset, int count, const char* pathTemplate) {
  double dParam0, dParam1, dParam2, dParam3, dParam4;
  int action;
  char path[64];
  char *file = NULL;
  char *name = NULL;
  json_token *token;

  for(int i= 0;i<count;i++){

    sprintf(path,pathTemplate,i);
    token = find_json_token(jsontoken, path);
    if(token){
      action = dParam0 = dParam1 = dParam2 = dParam3 = dParam4 = 0;
      json_int(token,"action",&action);
      if(action > 0){
        json_str(token,"name",&name,64);
        json_str(token,"file",&file,256);
        json_double(token,"dParam0",&dParam0);
        json_double(token,"dParam1",&dParam1);
        json_double(token,"dParam2",&dParam2);
        json_double(token,"dParam3",&dParam3);
        json_double(token,"dParam4",&dParam4);

      }
      //M2-M9,S index 2-9
      //userButtons index 11-20
      //M100-M119 index 21 -39
      //Special actions index 41 -48
      int actionIndex = indexOffset + i;
      int actionNameIndex = action > 0 && action < 10?action:10;
      if(actionNameIndex != 10){
        //printf("Set action index %d to %s\n",actionIndex, ACTION_NAMES[actionNameIndex]);

      }
      Interpreter->McodeActions[actionIndex].Action = action;
      Interpreter->McodeActions[actionIndex].dParams[0] = dParam0;
      Interpreter->McodeActions[actionIndex].dParams[1] = dParam1;
      Interpreter->McodeActions[actionIndex].dParams[2] = dParam2;
      Interpreter->McodeActions[actionIndex].dParams[3] = dParam3;
      Interpreter->McodeActions[actionIndex].dParams[4] = dParam4;


      //file should be nulled if not set.
      if(file == NULL || action == 0){
        Interpreter->McodeActions[actionIndex].String[0] = 0;
      } else {
        if(file[0]=='/'){
          //path is absolute
          strcpy(Interpreter->McodeActions[actionIndex].String, file);
        } else {
          //path is relative. make it absolute
          char absoluteFile[256];
          absolutePath(file, absoluteFile);
          strcpy(Interpreter->McodeActions[actionIndex].String, absoluteFile);
        }
      }

    } else {
      printf("Failed %s\n", pathTemplate);
    }
  }
  free(file);
  free(name);
}

void KmxController::OnStatusCallback(int line_no, const char *msg) {
  //This is a non blocking call from other thread to be enqueued in poll thread
  char buf[256];
  //if(gstate.currentLine != line_no){
  currentLine = line_no;
    //enqueueState();
  //}
  json_emit(buf, 256, "{ s: i, s: s }",
      "line", line_no,
      "message", msg);
  cbh_enqueue_callback(buf, CB_STATUS);
}


void KmxController::OnCompleteCallback(int status, int line_no, int sequence_number,const char *err) {
  //This is a non blocking call from other thread to be enqueued in poll thread
  interpreting = false;

  if (performPostHaltCommand)
  {
    performPostHaltCommand=false;
    InvokeAction(ACTION_HALT,FALSE);  // Do Special Action
  }
  currentLine = line_no;

  char buf[256];
  json_emit(buf, 256, "{ s: i, s: i, s: i, s: s }",
      "line", line_no,
      "status", status,
      "sequence", sequence_number,
      "message", err);
  //enqueueState();
  cbh_enqueue_callback(buf, CB_COMPLETE);
}
void KmxController::OnErrorMessageCallback(const char *msg) {
  //This is a non blocking call enqueue in poll thread
  char buf[256];
  json_emit(buf, 256, "s", msg);
  cbh_enqueue_callback(buf, CB_ERR_MSG);

}

int KmxController::OnConsoleCallback(const char *msg) {
  //This is a non blocking call even though it has a return value.. strange
  //Not even sure this happens in another thread
  char buf[256];
  json_emit(buf, 256, "s", msg);
  return cbh_enqueue_callback(buf, CB_CONSOLE);
}

int KmxController::OnMessageBoxCallback(const char *title, const char *msg, int options) {
  int blocking = 0;
  //non blocking MB_OK?
  if ((options & (MB_YESNO | MB_OKCANCEL)) == (MB_YESNO | MB_OKCANCEL)) {
    blocking = 1;
  }

  //TODO Messages might be pretty long check this and alloc
  char buf[512];
  json_emit(buf, 512, "{ s: i, s: s, s: s }",
      "options", options,
      "title", title,
      "message", msg);
  //todo handle blocking messagebox
  return cbh_enqueue_callback(buf, CB_MESSAGEBOX);
}

int KmxController::OnUserCallback(const char *msg) {
  //This is a blocking call. A bit trickier
  char buf[256];
  json_emit(buf, 256, "s", msg);
  int result = cbh_enqueue_callback(buf, CB_USER);
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
  char buf[10];
  json_emit(buf, 10, "i", mCode);
  int result = cbh_enqueue_callback(buf, CB_USER_M_CODE);
  debug("UserMCallback result: %d", result);
  return result;

}

void KmxController::OnReceiveClientData(const char * msg) {
  //TODO parse message here and call with parameters
  //Will allow for subclassing

  cbh_poll_callbacks(msg);
}
void KmxController::Poll() {
  //poll is called every at least every 100 ms but can be alot more often
  if(msPast(&tval_poll_callbacks,200)){
    //debug("polling callbacks");
    cbh_poll_callbacks(NULL);
  }

  //only perform poll when locked
  if(km->WaitToken(false,100.0 == KMOTION_LOCKED)){

    if(msPast(&tval_status,200)){
      //debug("pushar status");
      push_status();
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
