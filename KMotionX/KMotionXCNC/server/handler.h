#ifndef HANDLER_HEADER_INCLUDED
#define HANDLER_HEADER_INCLUDED

#include "mongoose.h"
#include "frozen.h"
#include "../../../GCodeInterpreter/StdAfx.h"

extern CGCodeInterpreter *Interpreter;
extern CKMotionDLL *km;
extern CCoordMotion *CM;
extern struct mg_server * server;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int ev_handler(struct mg_connection *conn, enum mg_event ev);
void initHandler();
void info_handler(int signum);
void push_message(const char *msg);

#ifdef __cplusplus
}

#endif // __cplusplus

#endif // FROZEN_HEADER_INCLUDED
