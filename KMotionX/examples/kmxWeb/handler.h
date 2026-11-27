#ifndef HANDLER_HEADER_INCLUDED
#define HANDLER_HEADER_INCLUDED
#include <stdint.h>
#include "mongoose.h"
#include "frozen.h"

extern struct mg_server * server;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int ev_handler(struct mg_connection *conn, enum mg_event ev);
void initHandler();
void info_handler(int signum);
void teardown();

void ErrMsgHandler(const wchar_t *msg);
int ConsoleHandler(const wchar_t *msg);
int MessageBoxHandler(const wchar_t *, const wchar_t *, uint32_t );
int MUserCallback(int);
int UserCallback(const wchar_t *);
void StatusCallback(int line_no, const wchar_t *msg);
void CompleteCallback(int status, int line_no, int sequence_number,const wchar_t *err);

#ifdef __cplusplus
}

#endif // __cplusplus

#endif // FROZEN_HEADER_INCLUDED
