#ifndef cws_uri_h
#define cws_uri_h

#include "htable.h"

typedef struct
{
  // Raw URI as found in the request
  char *raw_uri;

  // Absolute, requested path, starting with a /
  char *path;

  // key: query parameter name (string)
  // value: query parameter values (zero terminated string array)
  htable_t *query;
} cws_uri_t;

bool cws_uri_parse(char *raw_uri, cws_uri_t **output, const char **error_message);

void cws_uri_free(cws_uri_t *uri);

#endif