#ifndef HANDLER_HEADER_INCLUDED
#define HANDLER_HEADER_INCLUDED

#include "mongoose.h"
#include "frozen.h"
#include "../../../GCodeInterpreter/StdAfx.h"

extern CGCodeInterpreter *gci;
extern CKMotionDLL *km;
extern CCoordMotion *cm;
extern struct mg_server * server;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int ev_handler(struct mg_connection *conn, enum mg_event ev);
void initHandler();
void info_handler(int signum);

#ifdef __cplusplus
}

#endif // __cplusplus

#endif // FROZEN_HEADER_INCLUDED
