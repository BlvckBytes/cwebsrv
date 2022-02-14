#ifndef cws_request_h
#define cws_request_h

#include "cws/cws_http_method.h"
#include "cws/cws_uri.h"
#include "cws/cws_util.h"
#include "datastruct/htable.h"
#include "util/str2int.h"
#include "util/mman.h"
#include <stdarg.h>

// Default number of headers the hash table gets allocated to
#define CWS_DEF_NUM_HEADERS 4UL

// Maximum number of headers the hash table can grow to
#define CWS_MAX_NUM_HEADERS 128UL

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

void cws_request_free(void *request);

void cws_request_print(cws_request_t *request);

#endif