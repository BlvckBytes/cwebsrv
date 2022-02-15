#include "cws/cws_http_method.h"

static const char *cws_http_method_str[] = {
  [OPTIONS] = "OPTIONS",
  [GET] = "GET",
  [HEAD] = "HEAD",
  [POST] = "POST",
  [PUT] = "PUT",
  [DELETE] = "DELETE",
  [TRACE] = "TRACE",
  [CONNECT] = "CONNECT"
};

static size_t cws_http_method_str_len = sizeof(cws_http_method_str) / sizeof(char *);

const char *cws_http_method_stringify(cws_http_method_t method)
{
  if (method < 0 || method >= cws_http_method_str_len)
    return NULL;

  return cws_http_method_str[method];
}

bool cws_http_method_parse(char *string, cws_http_method_t *output)
{
  for (size_t i = 0; i < cws_http_method_str_len; i++)
  {
    const char *curr = cws_http_method_str[i];
    if (strncmp(curr, string, strlen(curr)) != 0) continue;
    *output = (cws_http_method_t) i;
    return true;
  }

  return false;
}