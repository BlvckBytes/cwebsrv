#include "cws/cws_response_code.h"

static const char *cws_response_code_str[] = {
  /*
  ============================================================================
                                Informational                                 
  ============================================================================
  */
  [STATUS_CONTINUE] = "Continue",
  [STATUS_SWITCHING_PROTOCOLS] = "Switching Protocols",
  [STATUS_PROCESSING] = "Processing",
  [STATUS_EARLY_HINTS] = "Early Hints",

  /*
  ============================================================================
                                 Successfull                                  
  ============================================================================
  */
  [STATUS_OK] = "OK",
  [STATUS_CREATED] = "Created",
  [STATUS_ACCEPTED] = "Accepted",
  [STATUS_NON_AUTHORITATIVE_INFORMATION] = "Non Authoritative Information",
  [STATUS_NO_CONTENT] = "No Content",
  [STATUS_RESET_CONTENT] = "Reset Content",
  [STATUS_PARTIAL_CONTENT] = "Partial Content",
  [STATUS_MULTI_STATUS] = "Multi Status",
  [STATUS_ALREADY_REPORTED] = "Already Reported",
  [STATUS_IM_USED] = "Im Used",

  /*
  ============================================================================
                                 Redirection                                  
  ============================================================================
  */
  [STATUS_MULTIPLE_CHOICES] = "Multiple Choices",
  [STATUS_MOVED_PERMANENTLY] = "Moved Permanently",
  [STATUS_FOUND] = "Found",
  [STATUS_SEE_OTHER] = "See Other",
  [STATUS_NOT_MODIFIED] = "Not Modified",
  [STATUS_USER_PROXY] = "User Proxy",
  [STATUS_SWITCH_PROXY] = "Switch Proxy",
  [STATUS_TEMPORARY_REDIRECT] = "Temporary Redirect",
  [STATUS_PERMANENT_REDIRECT] = "Permanent Redirect",

  /*
  ============================================================================
                                 Client Error                                 
  ============================================================================
  */
  [STATUS_BAD_REQUEST] = "Bad Request",
  [STATUS_UNAUTHORIZED] = "Unauthorized",
  [STATUS_PAYMENT_REQUIRED] = "Payment Required",
  [STATUS_FORBIDDEN] = "Forbidden",
  [STATUS_NOT_FOUND] = "Not Found",
  [STATUS_METHOD_NOT_ALLOWED] = "Method Not Allowed",
  [STATUS_NOT_ACCEPTABLE] = "Not Acceptable",
  [STATUS_PROXY_AUTHENTICATION_REQUIRED] = "Proxy Authentication Required",
  [STATUS_REQUEST_TIMEOUT] = "Request Timeout",
  [STATUS_CONFLICT] = "Conflict",
  [STATUS_GONE] = "Gone",
  [STATUS_LENGTH_REQUIRED] = "Length Required",
  [STATUS_PRECONDITION_FAILED] = "Precondition Failed",
  [STATUS_PAYLOAD_TOO_LARGE] = "Payload Too Large",
  [STATUS_URI_TOO_LONG] = "Uri Too Long",
  [STATUS_UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type",
  [STATUS_RANGE_NOT_SATISFYABLE] = "Range Not Satisfyable",
  [STATUS_EXPECTATION_FAILED] = "Expectation Failed",
  [STATUS_IM_A_TEAPOT] = "Im A Teapot",
  [STATUS_MISDIRECTED_REQUEST] = "Misdirected Request",
  [STATUS_UNPROCESSABLE_ENTITY] = "Unprocessable Entity",
  [STATUS_LOCKED] = "Locked",
  [STATUS_FAILED_DEPENDENCY] = "Failed Dependency",
  [STATUS_TOO_EARLY] = "Too Early",
  [STATUS_UPGRADE_REQUIRED] = "Upgrade Required",
  [STATUS_PRECONDITION_REQUIRED] = "Precondition Required",
  [STATUS_TOO_MANY_REQUESTS] = "Too Many Requests",
  [STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE] = "Request Header Fields Too Large",
  [STATUS_UNAVAILABLE_FOR_LEGAL_REASONS] = "Unavailable For Legal Reasons",

  /*
  ============================================================================
                                 Server Error                                 
  ============================================================================
  */
  [STATUS_INTERNAL_SERVER_ERROR] = "Internal Server Error",
  [STATUS_NOT_IMPLEMENTED] = "Not Implemented",
  [STATUS_BAD_GATEWAY] = "Bad Gateway",
  [STATUS_SERVICE_UNAVAILABLE] = "Service Unavailable",
  [STATUS_GATEWAY_TIMEOUT] = "Gateway Timeout",
  [STATUS_HTTP_VERSION_NOT_SUPPORTED] = "Http Version Not Supported",
  [STATUS_VARIANT_ALSO_NEGOTIATES] = "Variant Also Negotiates",
  [STATUS_INSUFFICIENT_STORAGE] = "Insufficient Storage",
  [STATUS_LOOP_DETECTED] = "Loop Detected",
  [STATUS_NOT_EXTENDED] = "Not Extended",
  [STATUS_NETWORK_AUTHENTICATION_REQUIRED] = "Network Authentication Required"
};

static size_t cws_response_code_str_len = sizeof(cws_response_code_str) / sizeof(char *);

const char *cws_response_code_stringify(cws_response_code_t code)
{
  if (code < 0 || code >= cws_response_code_str_len)
    return NULL;
  return cws_response_code_str[code];
}