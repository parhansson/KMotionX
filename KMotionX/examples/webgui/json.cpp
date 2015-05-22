#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "json.h"
#include "frozen.h"
#include "math.h"
#include "dbg.h"

int toks(const struct json_token *tk, char **ptr, const int maxsize) {
  int len = maxsize;

  if (tk->type == JSON_TYPE_NULL) {
    if (*ptr != NULL) {
      memset(*ptr, 0, 1);
    }
    return 0;
  } else {

    if (len <= 0) len = tk->len;
    int mal = (sizeof(char) * (len)) + sizeof(char);
    if (*ptr == NULL) {
      //debug("allocate %d", mal);
      *ptr = (char*) malloc(mal);
      if(*ptr == NULL){
          debug("allocate %d failed", mal);
          return -1;
      }
    }

    int min = fmin(len, tk->len);
    memcpy(*ptr, tk->ptr, min);
    memset(*ptr + min, 0, 1);

    return min;
  }

}

int toki(const struct json_token *tk, int * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str, 0);
  *value = atoi(str);
  free(str);
  return *value;
}

double tokd(const struct json_token *tk, double * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str, 0);
  sscanf(str, "%lf", value);
  free(str);
  return *value;
}
bool tokb(const struct json_token *tk, bool * value) {
  if (tk->type == JSON_TYPE_NULL) {
    return *value;
  }
  char *str = nullptr;
  toks(tk, &str, 0);
  if (strcmp(str, "true") == 0) {
    *value = true;
  } else {
    *value = false;
  }
  free(str);
  return *value;
}
/**
 *reads a json string value with path copying result into sptr
 */
int json_str(struct json_token *tks, const char *path, char *sptr) {
    char * val = NULL;
    int r = json_str(tks, path, &val, 0);
    strcpy(sptr, val);
    free(val);
    return r;
}
/**
 * reads a json string value with path returning result in sptr with max size
 *
 */
int json_str(struct json_token *tks, const char *path, char **sptr, const int size) {
  json_token *token;
  token = find_json_token(tks, path);
  if (token != NULL) {
    return toks(token, sptr, size);
  }
  return -1;
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

