#ifndef cws_request_h
#define cws_request_h

#include "cws/cws_http_method.h"
#include "cws/cws_uri.h"
#include "cws/cws_common.h"
#include "datastruct/htable.h"
#include "util/longp.h"
#include "util/mman.h"
#include "util/partial_strdup.h"
#include <stdarg.h>

/*
============================================================================
                               Configuration                                
============================================================================
*/
// Maximum number of headers the hash table can grow to
#define CWS_MAX_NUM_HEADERS 64UL

/*
============================================================================
                             Head parsing chain                             
============================================================================
*/

/**
 * @brief Represents a HTTP request's head
 */
typedef struct cws_request_head
{
  // Request method performed on the URI
  cws_http_method_t method;

  // Requested resource
  cws_uri_t *uri;

  // key: headerfname string
  // value: header value string
  htable_t *headers;

  // Part of the body, which has been within the first segment
  char *body_part;

  // Http version
  long http_ver_major;
  long http_ver_minor;
} cws_request_head_t;

/**
 * @brief Represents a stage of request head parsing
 * @returns true On successful execution
 * @returns false On errors, error-description is set in err
 */
typedef bool (*cws_head_parser_t)(
  char *req,                    // Full request string
  size_t *offs,                 // Offset pointer for change in place
  cws_request_head_t *result,   // Parse result to which partial results are applied
  char **err                    // Error buffer ptr
);

/**
 * @brief Parse a web request by it's raw request string
 * 
 * @param request Raw request string
 * @param error_msg Error message output buffer
 * @return cws_request_t* Parsed result, NULL when an error occurred
 */
cws_request_head_t *cws_request_head_parse(char *request, char **error_msg);

/*
============================================================================
                                 Printing                                   
============================================================================
*/

/**
 * @brief Print the contents of a request on stdout
 */
void cws_request_head_print(cws_request_head_t *request);

#endif