#ifndef JSON_HEADER_INCLUDED
#define JSON_HEADER_INCLUDED
#include "frozen.h"

int toks(const struct json_token *tk, void *value, const int size = 0);
int toki(const struct json_token *tk, int * value);
double tokd(const struct json_token *tk, double * value);
bool tokb(const struct json_token *tk, bool * value);


int json_str(struct json_token *toks, const char *path, void *sptr, const int size = 0);
int json_int(struct json_token *toks, const char *path, int * value);
double json_double(struct json_token *toks, const char *path, double * value);
bool json_bool(struct json_token *toks, const char *path, bool * value);


#endif
