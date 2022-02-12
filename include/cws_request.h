#ifndef cws_request_h
#define cws_request_h

#include "cws_http_method.h"
#include "cws_uri.h"
#include "cws_util.h"
#include "htable.h"
#include "str2int.h"

/**
 * @brief Represents a HTTP request by a client
 */
typedef struct
{
  // Request method performed on the URI
  cws_http_method_t method;

  // Requested resource
  cws_uri_t *uri;

  // key: headerfname string
  // value: header value string
  htable_t *headers;

  // Raw body as a string
  char *body;

  // Http version
  uint8_t http_ver_major;
  uint8_t http_ver_minor;
} cws_request_t;

cws_request_t *cws_request_parse(char *request, const char **error_msg);

void cws_request_free(cws_request_t *request);

void cws_request_print(cws_request_t *request);

#endif