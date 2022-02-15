#ifndef cws_request_h
#define cws_request_h

#include "cws/cws_http_method.h"
#include "cws/cws_uri.h"
#include "cws/cws_common.h"
#include "datastruct/htable.h"
#include "util/str2int.h"
#include "util/mman.h"
#include "util/partial_strdup.h"
#include <stdarg.h>

/*
============================================================================
                               Configuration                                
============================================================================
*/

// Default number of headers the hash table gets allocated to
#define CWS_DEF_NUM_HEADERS 4UL

// Maximum number of headers the hash table can grow to
#define CWS_MAX_NUM_HEADERS 128UL

/*
============================================================================
                                  Request                                   
============================================================================
*/

/**
 * @brief Represents a HTTP request by a client
 */
typedef struct cws_request
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

/**
 * @brief Parse a web request by it's raw request string
 * 
 * @param request Raw request string
 * @param error_msg Error message output buffer
 * @return cws_request_t* Parsed result, NULL when an error occurred
 */
cws_request_t *cws_request_parse(char *request, const char **error_msg);

/**
 * @brief Print the contents of a request on stdout
 */
void cws_request_print(cws_request_t *request);

#endif