#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "json.h"
#include "frozen.h"
#include "math.h"

int toks(const struct json_token *tk, void *sptr, const int size) {
  int len = size;
  char ** ptr = (char **) sptr;
  if (tk->type == JSON_TYPE_NULL) {
    return 0;
  }
  if (*ptr == NULL && tk->type != JSON_TYPE_NULL) {
    if (len <= 0) len = tk->len;
    int mal = (sizeof(char) * (len)) + sizeof(char);
    *ptr = (char*) malloc(mal);
  }

  int min = fmin(len, tk->len);
  memcpy(*ptr, tk->ptr, min);
  memset(*ptr + min, 0, 1);

  return 0;
}

int toki(const struct json_token *tk, int * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str);
  *value = atoi(str);
  free(str);
  return *value;
}

double tokd(const struct json_token *tk, double * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str);
  sscanf(str, "%lf", value);
  free(str);
  return *value;
}
bool tokb(const struct json_token *tk, bool * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str);
  if (strcmp(str, "true") == 0) {
    *value = true;
  } else {
    *value = false;
  }
  free(str);
  return *value;
}



int json_str(struct json_token *tks, const char *path, void *sptr, const int size) {
  json_token *token;
  token = find_json_token(tks, path);
  if (token != NULL) {

    //TODO this should not be needed. fix in toks() somehow
    char ** ptr = (char **) sptr;
    if(*ptr != NULL){
      free(*ptr);*ptr = NULL;
    }

    return toks(token, sptr, size);
  }
  return 1;
}

int json_int(struct json_token *tks, const char *path, int * value) {
  json_token *token;
  token = find_json_token(tks, path);
  if (token != NULL) {
    return toki(token, value);
  }
  return *value;
}

double json_double(struct json_token *tks, const char *path, double * value) {
  json_token *token;
  token = find_json_token(tks, path);
  if (token != NULL) {
    return tokd(token, value);
  }
  return *value;
}

bool json_bool(struct json_token *tks, const char *path, bool * value) {
  json_token *token;
  token = find_json_token(tks, path);
  if (token != NULL) {
    return tokb(token, value);
  }
  return *value;
}

