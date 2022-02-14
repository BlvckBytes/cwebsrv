#ifndef cws_http_method_h
#define cws_http_method_h

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

typedef enum
{
  OPTIONS,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  CONNECT
} cws_http_method_t;

const char *cws_http_method_stringify(cws_http_method_t method);

bool cws_http_method_parse(char *string, cws_http_method_t *output);

#endif