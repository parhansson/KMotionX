#ifndef HANDLER_HEADER_INCLUDED
#define HANDLER_HEADER_INCLUDED

#include "mongoose.h"
#include "frozen.h"
#include "../../../GCodeInterpreter/StdAfx.h"

extern struct mg_server * server;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int ev_handler(struct mg_connection *conn, enum mg_event ev);
void initHandler();
void info_handler(int signum);

void ErrMsgHandler(const char *msg);
int ConsoleHandler(const char *msg);
int MessageBoxHandler(const char *, const char *, int );
int MUserCallback(int);
int UserCallback(const char *);
void StatusCallback(int line_no, const char *msg);
void CompleteCallback(int status, int line_no, int sequence_number,const char *err);

int push_to_clients(int opCode, const char *data , size_t data_len);

typedef int PUSH_TO_CLIENTS(int opCode, const char *data, size_t data_len);

#ifdef __cplusplus
}

#endif // __cplusplus

#endif // FROZEN_HEADER_INCLUDED
