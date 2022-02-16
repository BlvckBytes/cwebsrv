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

/**
 * @brief Turn the numeric HTTP method into it's string representation
 * 
 * @param method HTTP method to stringify
 * @return const char* Stringified result, NULL for invalid methods
 */
const char *cws_http_method_stringify(cws_http_method_t method);

/**
 * @brief Parse a stringified HTTP method into it's numeric representation
 * 
 * @param string Stringified HTTP method
 * @param output Numeric output
 * 
 * @return true Could parse successfully
 * @return false Invalid method provided
 */
bool cws_http_method_parse(char *string, cws_http_method_t *output);

#endif