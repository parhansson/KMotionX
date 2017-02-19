/*
 * WebController.cpp
 *
 *  Created on: 18 dec. 2015
 *      Author: parhansson
 */

#include "WebController.h"
#include <dirent.h>
#include "json.h"
#include "utils.h"

//Emits json resonse
#define EMIT_RESPONSE_PARAM(fmt,...) w += json_emit(gp_response+w, MAX_RESPONSE-w, fmt, ##__VA_ARGS__);
#define EMIT_RESPONSE(fmt, ...) int w = 0; w = json_emit(gp_response+w, MAX_RESPONSE, "{ s: s, s: s, s: ", "api", object,"func",func, "params"); \
    EMIT_RESPONSE_PARAM(fmt,##__VA_ARGS__)  \
    w += json_emit(gp_response+w, MAX_RESPONSE-w, ", s: i }", "ret",ret);

//Check function signature
#define FUNC_SIGP(name, nr_of_params) !strcmp(name,func) && params == nr_of_params

static const char CB_NAMES[][24] = {"STATUS", "COMPLETE", "ERR_MSG", "CONSOLE",
    "USER", "USER_M_CODE", "MESSAGEBOX" };

/*
//Used for debugging.
static const char ACTION_NAMES[][32] = { "M_Action_None", "M_Action_Setbit", "M_Action_SetTwoBits",
      "M_Action_DAC", "M_Action_Program", "M_Action_Program_wait", "M_Action_Program_wait_sync",
      "M_Action_Program_PC", "M_Action_Callback", "M_Action_Waitbit", "UNAVAILABLE" };
*/

WebController::WebController(CGCodeInterpreter *interpreter, mg_server *serv)
:KmxController(interpreter) {
  server = serv;
}

WebController::~WebController() {

}

int WebController::Setup(){
  log_info("Initialize");
  char fileName[256];
  char absolute_path[256];
  absolutePath("settings/setup.cnf", fileName);
  MappedFile mmFile;
  if(mmapNamedFile(mmFile, fileName)){
    log_info("Failed to read setup file");
    return -1;
  }

  json_token *setup = NULL;
  setup = parse_json2(mmFile.addr, mmFile.filesize);

  json_str(setup,"machine",fileName);
  absolutePath(fileName, absolute_path);
  settings_file->SetFile(absolute_path);

  json_str(setup,"gcodefile",fileName);
  absolutePath(fileName, absolute_path);
  current_gcode_file->SetFile(absolute_path);

  free(setup);
  unmapFile(mmFile);
  return 0;
}


void WebController::PrintInfo(){
  mq->PrintInfo();


  struct mg_connection *c = NULL;
  int websockets = 0;
  int connections = 0;
  // Iterate over all connections, and push current time message to websocket ones.
  for (c = mg_next(server, c); c != NULL; c = mg_next(server, c)) {
    if (c->is_websocket) {
      websockets++;
    } else {
      connections++;
    }
  }

  fprintf(stdout, "Websockets   :%d\n", websockets);
  fprintf(stdout, "Connections  :%d\n", connections);
  fprintf(stdout, "State        :%s\n", this->interpreting ? "Interpreting" : "Stopped");
  fprintf(stdout, "Config       :%s (%d)\n", this->settings_file->path, this->settings_file->timestamp);
  fprintf(stdout, "File         :%s (%d)\n", this->current_gcode_file->path, this->current_gcode_file->timestamp);
  fprintf(stdout, "Line         :%d\n", this->currentLine);
  fprintf(stdout, "FeedHold     :%d\n", this->main_status.StopImmediateState);
  fprintf(stdout, "Simulating   :%d\n", this->simulate);
}



#define MEMCPY(to, from, size) memcpy(to, from, size); to += size;
void WebController::UpdateClient() {

  CCoordMotion *CM = Interpreter->CoordMotion;
  int current_file_len = strlen(current_gcode_file->path);
  int current_machine_len = strlen(settings_file->path);
  //copy struct and avoid padding wich may differ on platforms
  size_t size = 456 + 6*8 + 4*4 + current_file_len+4 +current_machine_len+4 + 8;//sizeof(main_status);
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
    //Timestamp is not acually used other than to see if backend is connected. Send seconds since midnight
    time_t service_console_time = time(NULL);
    double timeStamp = service_console_time % 86400;
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

  MEMCPY(msgPtr, &current_file_len, 4);
  MEMCPY(msgPtr, current_gcode_file->path, current_file_len);
  MEMCPY(msgPtr, &current_gcode_file->timestamp, 4);

  MEMCPY(msgPtr, &current_machine_len, 4);
  MEMCPY(msgPtr, settings_file->path, current_machine_len);
  MEMCPY(msgPtr, &settings_file->timestamp, 4);

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
  //needs to be a call back or implemented in subclass
  PushClientData(WEBSOCKET_OPCODE_BINARY, msg, size);

  free(msg);
}

//
/**
 *It is not allowed to call any function that takes mg_server from other thread than poll thread
 *Hence this function must only be called from poll thread
 */
int WebController::DoCallback(const char *data){
  return PushClientData(WEBSOCKET_OPCODE_TEXT, data, strlen(data));
}

int WebController::OnEventRequest(struct mg_connection *conn) {
  if (conn->is_websocket) {
    char * content = strndup(conn->content, conn->content_len);
    //debug("MG_REQUEST websocket. content:\"%s\"",content);
    this->OnReceiveClientData(content);
    free(content);
    return MG_TRUE;
  } else if (isUploadRequest(conn)) {
    HandleUploadRequest(conn);
    return MG_TRUE; // Tell Mongoose we're done with this request
  } else if (isApiRequest(conn)) {
    //Starts with /api but can be anything
    return HandleApiRequest(conn);
  } else if(isRegisteredRoute(conn)){
    const char * file = "ng2/index.html";
    const char * headers ="";
    mg_send_file(conn, file,headers);
    return MG_MORE;
  }
  return MG_FALSE;
}

// Mongoose sends MG_RECV for every received POST chunk.
// When last POST chunk is received, Mongoose sends MG_REQUEST, then MG_CLOSE.
int WebController::OnEventRecv(struct mg_connection *conn) {
  if (conn->is_websocket) {
    return 0; //MG_FALSE Do not discard anything from receive buffer;
  } else {

    if (isUploadRequest(conn)) {
      //returns written bytes to discard from receive buffer
      return HandleUploadReceiveEvent(conn);
    }
    return 0; //MG_FALSE Do not discard anything from receive buffer;
  }
}

int WebController::OnEventClose(struct mg_connection *conn) {
  // Make sure we free all allocated resources
  if (conn->connection_param != NULL) {
    fclose((FILE *) conn->connection_param);
    conn->connection_param = NULL;
    debug("handle close upload");
  }
  return MG_TRUE;
}

int WebController::OnEventPoll(struct mg_connection *conn) {
  this->Poll();
  return MG_FALSE; //Return value is said to be ignored on MG_POLL but does'nt seem to be
}
int WebController::OnEventWsConnect(struct mg_connection *conn) {
  //if websocket /ws is param here
  //printf("event %d param: %s\n", ev, conn->uri);

  //We need to send something to the websocket
  DoCallback("KMotionX");
  return MG_FALSE;
}

int WebController::PushClientData(int opCode, const char *data, size_t data_len){
  struct mg_connection *c = NULL;
  int nrOfClients = 0;
  char * dst = NULL;
  const char * buf;
  if(opCode == WEBSOCKET_OPCODE_TEXT){
    //We need to ensure that client can decode as UTF-8
    data_len = expand_non_ascii(&dst,data,data_len);
    buf = dst;
  } else {
    buf = data;
  }

  // Iterate over all connections, and push current time message to websocket ones.
  for (c = mg_next(server, c); c != NULL; c = mg_next(server, c)) {
    if (c->is_websocket) {
      mg_websocket_write(c, opCode, buf, data_len);
      nrOfClients++;
    }
  }
  free(dst);
  return nrOfClients;
}

bool WebController::isUploadRequest(struct mg_connection *conn){
  //strcmp(conn->uri, "/upload") == conn->uri;
  return strstr(conn->uri, "/upload") == conn->uri;
}

bool WebController::isApiRequest(struct mg_connection *conn){
  return strstr(conn->uri, "/api") == conn->uri;
}
bool WebController::isRegisteredRoute(struct mg_connection *conn){
  if(strstr(conn->uri, "/ng2/gcode") == conn->uri || 
      strstr(conn->uri, "/ng2/debug") == conn->uri || 
      strstr(conn->uri, "/ng2/ccode") == conn->uri ||
      strstr(conn->uri, "/ng2/laser-calc") == conn->uri ||
      strstr(conn->uri, "/ng2/settings") == conn->uri){
      //printf("%s\n", conn->uri);
      return true;
  }
  return false;
}

int WebController::HandleUploadReceiveEvent(struct mg_connection *conn){
  FILE *fp = (FILE *) conn->connection_param;

  // Open temporary file where we going to write data
  if (fp == NULL && ((conn->connection_param = fp = tmpfile())) == NULL) {
    return -1;  // Close connection on error
  }
  // Return number of bytes written to a temporary file: that is how many
  // bytes we want to discard from the receive buffer
  return fwrite(conn->content, 1, conn->content_len, fp);
}
void WebController::HandleUploadRequest(struct mg_connection *conn){

  char msg[256];
  FILE *fp = (FILE *) conn->connection_param;
  if (fp != NULL) {

    //Sends HTTP/1.1 200 OK
    mg_send_header(conn, "Content-Type", "text/plain");

    // Temp file will be destroyed after fclose(), do something with the
    // data here -- for example, parse it and extract uploaded files.
    // As an example, we just echo the whole POST buffer back to the client.
    rewind(fp);
    MappedFile mmFile;
    if(mmapFile(mmFile, fp)){
      debug("Failed to read setup file");
      return;
    }
    size_t filesize=0;

    const char *data;
    int data_len, ofs = 0, mapOffset = 0;
    char var_name[100], file_name[256];
    char absoluteFile[256];
    FILE * pFile; 
    while ((ofs = mg_parse_multipart(mmFile.addr + mapOffset, mmFile.mapsize - mapOffset,
                                     var_name, sizeof(var_name),
                                     file_name, sizeof(file_name),
                                     &data, &data_len)) > 0) {
      mapOffset += ofs;
     
      absolutePath(file_name, absoluteFile);
      
      //TODO open file only when file_name changes 
      pFile = fopen (absoluteFile, "w+");

      if(pFile){
        size_t written = fwrite (data , sizeof(char), data_len, pFile);
        if(written != data_len){
          this->DoErrorMessage("Error writing to file");
          //ErrMsgHandler(strerror(errno));
        } else{
          filesize += written;
        }
        if(fclose (pFile) == EOF){
          snprintf(msg,sizeof(msg)-1, "Failed to close file %s\n",absoluteFile);
          this->DoErrorMessage(msg);
          this->DoErrorMessage(strerror(errno));
        }
      } else {
        snprintf(msg,sizeof(msg)-1, "Failed to open file %s\n",absoluteFile);
        this->DoErrorMessage(msg);
        this->DoErrorMessage(strerror(errno));
      }
    }
    unmapFile(mmFile);
    if(filesize == 0 /*filesize != tmpfilesize*/){
      snprintf(msg,sizeof(msg)-1, "Bytes written %ld of %ld\n",filesize, mmFile.filesize);
      this->DoErrorMessage(msg);
    }
    //TODO updating timestamp should be made in poll loop to detect outside changes to file.
    if(strcmp(absoluteFile, current_gcode_file->path) == 0){
      current_gcode_file->Update();
    }
    //need to send response back to client to avoid wating connection
    mg_printf_data(conn,
        "Written %ld bytes to file: %s\n\n",
        filesize,
        absoluteFile);
  } else {
    mg_printf_data(conn, "%s", "Had no data to write...\n\n");
  }
  //MG_CLOSE event is not raised by mongoose as expected. Need to close file
  OnEventClose(conn);
}

int WebController::HandleApiRequest(struct mg_connection *conn) {

  if (conn->content == NULL) {
    //Content is NULL if request method is GET
    debug("GET is not supported.");
    return MG_FALSE;
  }
  int result;

  char **ap, *argv[3], *input, *tofree;
  tofree = input = strdup(conn->uri); // should check for NULL

  for (ap = argv; (*ap = strsep(&input, "/")) != NULL;)
          if (**ap != '\0')
                  if (++ap >= &argv[3])
                          break;
  if (!strcmp("api", argv[0])) {
    debug("%s/%s/%s\n", argv[0],argv[1],argv[2]);
    result = HandleJsonRequest(conn,argv[1],argv[2]);
  } else {
    mg_send_header(conn, "Content-Type", "text/plain");
    mg_printf_data(conn, "KMotionCNC API # query_string %s\n", (char *) conn->query_string);
    mg_printf_data(conn, "KMotionCNC API # uri %s\n", (char *) tofree);
    result = MG_TRUE;
  }
  free(tofree);
  return result;
}


//TODO To ensure no blocking callbacks are made from poll thread this
//function should be called from another thread.

int WebController::HandleJsonRequest(struct mg_connection *conn, const char *object, const char *func) {

  json_token *data = NULL;
  json_token *paramtoken;
  char *content;
  int params;
  int ret;

  content = strndup(conn->content, conn->content_len);
  debug("%s",content);

  free(content);

  data = parse_json2(conn->content, conn->content_len);
  if (data == NULL) {
    debug("parsing api request failed");
    return MG_FALSE;
  }

  params = 0;

  paramtoken = find_json_token(data, "params");
  if (paramtoken != NULL) {
    if (paramtoken->type == JSON_TYPE_OBJECT) {
      params = 1;

    } else if (paramtoken->type == JSON_TYPE_ARRAY) {
      params = paramtoken->num_desc;
    } else {
      params = 1;
    }
  }

  //Reset global response string
  gp_response[0] = '\0';
  //gp_response = (char*) malloc(sizeof(char) * ( MAX_RESPONSE));
  ret = 0;
  if (!strcmp("kmx", object)) {
    if(FUNC_SIGP("setGcodeFile", 1)){
      char * file = NULL;
      toks(paramtoken, &file, 0);
      this->LoadGcode(file);
      free(file);
    } else if(FUNC_SIGP("setMachineFile", 1)){
      char * file = NULL;
      toks(paramtoken, &file, 0);
      this->LoadMachineConfiguration(file);
      free(file);
    } else if(FUNC_SIGP("listDir", 1)){
      ListDir(paramtoken);
    } else if(FUNC_SIGP("openFile", 1)){
      return OpenFile(conn, paramtoken);
    } else if (FUNC_SIGP("jog", 2)) {
      int axis;
      int speed;
      toki(paramtoken + 1, &axis);
      toki(paramtoken + 2, &speed);
      this->Jog(axis, speed);
    } else if (FUNC_SIGP("onFeedhold", 0)) {
      this->Feedhold();
    } else if (FUNC_SIGP("onSimulate", 0)) {
      this->Simulate();
    } else if (FUNC_SIGP("onEmergencyStop", 0)) {
      this->EmergencyStop();
    } else if (FUNC_SIGP("onHalt", 0)) {
      this->Halt();
    } else if (FUNC_SIGP("onStep", 0)) {
      this->Step();
    } else if (FUNC_SIGP("onReset", 0)) {
      this->Reset();
    } else if (FUNC_SIGP("onCycleStart", 0)) {
      this->CycleStart();
    } else if(FUNC_SIGP("onUpdateMotionParams", 0)) {
      this->UpdateMotionParams();
    } else if (FUNC_SIGP("onInvokeAction", 1)) {
      BOOL FlushBeforeUnbufferedOperation = FALSE;
      int action;
      toki(paramtoken, &action);
      ret = this->InvokeAction(action,FlushBeforeUnbufferedOperation);
    } else if (FUNC_SIGP("onDoErrorMessage", 1)) {
      char *p1 = NULL;
      toks(paramtoken, &p1, 0);
      this->DoErrorMessage(p1);
      EMIT_RESPONSE("[S]", p1);
      free(p1);
    } else {
      log_info("Function request is not part of API %s",func);
    }
  } else {
    log_info("API not implemented %s",object);
  }

  mg_send_header(conn, "Content-Type", "application/json");

  //Need to send some data or connection will not be closed
  if (gp_response[0] == '\0') {
    EMIT_RESPONSE("N");
  }

  mg_printf_data(conn, "%s", gp_response);

  //free(gp_response);
  free(data);

  return MG_TRUE;
}

int WebController::OpenFile(struct mg_connection *conn, struct json_token *paramtoken){
    char * file = NULL;//"ng2/index.html";
    toks(paramtoken, &file, 0);
    if (file != NULL) {
      const char * headers = "";
      mg_send_file(conn, file, headers);
      free(file);
      return MG_MORE;
    }
  return MG_TRUE;
}

void WebController::ListDir(struct json_token *paramtoken){
    char *dir = NULL;
    char absoluteDir[256];
    toks(paramtoken, &dir, 0);
    if (dir) {
      DIR *dp;
      struct dirent *ep;
      int w = 0,first=1;
      absolutePath(dir, absoluteDir);
      dp = opendir (absoluteDir);
      if (dp != NULL){
          w += json_emit(gp_response+w, MAX_RESPONSE-w, "{s: s, s: [", "dir", absoluteDir,"files");
          //w += snprintf(gp_response+w, MAX_RESPONSE-w, "[");
          while ((ep = readdir(dp))){
            //printf("%d %s\n", ep->d_type, ep->d_name);
            if(first){
              first = 0;
              w += json_emit(gp_response+w, MAX_RESPONSE-w, "{s: s, s: i}", "name", ep->d_name,"type",ep->d_type );
            } else {
              w += json_emit(gp_response+w, MAX_RESPONSE-w, ",{s: s, s: i}", "name", ep->d_name,"type",ep->d_type );
            }
          }
          w += snprintf(gp_response+w, MAX_RESPONSE-w, "]}");
          (void) closedir (dp);
      } else {
        w += json_emit(gp_response+w, MAX_RESPONSE-w, "{s: s, s: []}", "dir", absoluteDir,"files");
        this->DoErrorMessage(strerror(errno));
      }
    }
    free(dir);
}

void WebController::OnReceiveClientData(const char * content) {
  int id = -1;
  int returnValue = -1;
  //we have a connection.
  //incoming answer from client
  if (strlen(content) > 39 && strstr(content, "CB_ACK") == content+9) {
    // answer looks like this {"type":"CB_ACK","id":<integer>,"returnValue":<integer>}
    if(sscanf(content, "{\"type\":\"CB_ACK\",\"id\":%d,\"returnValue\":%d}", &id, &returnValue) == 2){
      //only poll poll callbacks in here
      Acknowledge(id, returnValue);
    }
  }
}

void WebController::SetMotionParams(const char *buf, size_t len) {
  log_info("SetMotionParams");
  json_token *jsontoken = NULL;
  jsontoken = parse_json2(buf, len);

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


  //printf("X %lf, %lf, %lf \n",p->CountsPerInchX,p->MaxAccelX, p->MaxVelX );
  //printf("Y %lf, %lf, %lf \n",p->CountsPerInchY,p->MaxAccelY, p->MaxVelY );
  //printf("Z %lf, %lf, %lf \n",p->CountsPerInchZ,p->MaxAccelZ, p->MaxVelZ );
  //printf("A %lf, %lf, %lf \n",p->CountsPerInchA,p->MaxAccelA, p->MaxVelA );
  //printf("B %lf, %lf, %lf \n",p->CountsPerInchB,p->MaxAccelB, p->MaxVelB );
  //printf("C %lf, %lf, %lf \n",p->CountsPerInchC,p->MaxAccelC, p->MaxVelC );

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

  free(jsontoken);
}
void WebController::setInterpreterActionParams(struct json_token *jsontoken, int indexOffset, int count, const char* pathTemplate) {
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
        char absoluteFile[256];
        absolutePath(file, absoluteFile);
        strcpy(Interpreter->McodeActions[actionIndex].String, absoluteFile);
      }

    } else {
      printf("Failed %s\n", pathTemplate);
    }
  }
  free(file);
  free(name);
}

#define MALLOC_CHAR_BUF(BUF, BUF_LEN) BUF = (char*) malloc (BUF_LEN * sizeof(char)); \
  if(BUF == NULL) { \
    printf("Error allocating memory"); \
    exit (1); \
  } \
  BUF[0] = '\0';

int WebController::CreateMessageBoxCallbackData(const char *title, const char *msg, int options, bool blocking, char **buf) {

  int buf_len = 128 + strlen(title) + strlen(msg);
  MALLOC_CHAR_BUF(*buf, buf_len);

  int id = callback_counter++;
  json_emit(*buf, buf_len, "{ s: i, s: { s: S, s: s, s: i, s: s, s: s }}",
      "id", id,
      "payload",
      "block",blocking?"true":"false",
      "type", CB_NAMES[CB_MESSAGEBOX],
      "options", options,
      "title", title,
      "message", msg);
  return id;
}

int WebController::CreateCompleteCallbackData(int status, int line_no, int sequence_number, const char *err, bool blocking, char **buf){

  int buf_len = 128 + strlen(err);
  MALLOC_CHAR_BUF(*buf, buf_len);

  int id = callback_counter++;
  json_emit(*buf, buf_len, "{ s: i, s: { s: S, s: s, s: i, s: i, s: i, s: s }}",
      "id", id,
      "payload",
      "block",blocking?"true":"false",
      "type", CB_NAMES[CB_COMPLETE],
      "line", line_no,
      "status", status,
      "sequence", sequence_number,
      "message", err);
  return id;

}
int WebController::CreateStatusCallbackData(int line_no, const char *msg, bool blocking, char **buf){

  int buf_len = 128 + strlen(msg);
  MALLOC_CHAR_BUF(*buf, buf_len);

  int id = callback_counter++;
  json_emit(*buf, buf_len, "{ s: i, s: { s: S, s: s, s: i, s: s }}",
      "id", id,
      "payload",
      "block", blocking?"true":"false",
      "type", CB_NAMES[CB_STATUS],
      "line", line_no,
      "message", msg);
  return id;
}

int WebController::CreateErrorMessageCallbackData(const char *msg, bool blocking, char **buf) {

  int buf_len = 128 + strlen(msg);
  MALLOC_CHAR_BUF(*buf, buf_len);

  int id = callback_counter++;
  json_emit(*buf, buf_len, "{ s: i, s: { s: S, s: s, s: s }}",
      "id", id,
      "payload",
      "block",blocking?"true":"false",
      "type", CB_NAMES[CB_ERR_MSG],
      "message", msg);
  return id;
}
int WebController::CreateConsoleCallbackData(const char *msg, bool blocking, char **buf){

  int buf_len = 128 + strlen(msg);
  MALLOC_CHAR_BUF(*buf, buf_len);

  int id = callback_counter++;
  json_emit(*buf, buf_len, "{ s: i, s: { s: S, s: s, s: s }}",
      "id", id,
      "payload",
      "block",blocking?"true":"false",
      "type", CB_NAMES[CB_CONSOLE],
      "message", msg);
  return id;

}
int WebController::CreateUserCallbackData(const char *msg, bool blocking, char **buf){
  
  int buf_len = 128 + strlen(msg);
  MALLOC_CHAR_BUF(*buf, buf_len);

  int id = callback_counter++;
  json_emit(*buf, buf_len, "{ s: i, s: { s: S, s: s, s: s }}",
      "id", id,
      "payload",
      "block",blocking?"true":"false",
      "type", CB_NAMES[CB_USER],
      "message", msg);
  return id;

}
int WebController::CreateMcodeUserCallbackData(int mCode, bool blocking, char **buf){

  int buf_len = 128;
  MALLOC_CHAR_BUF(*buf, buf_len);

  int id = callback_counter++;
  json_emit(*buf, buf_len,"{ s: i, s: { s: S, s: s, s: i }}",
      "id", id,
      "payload",
      "block",blocking?"true":"false",
      "type", CB_NAMES[CB_USER_M_CODE],
      "code", mCode);
  return id;
}

