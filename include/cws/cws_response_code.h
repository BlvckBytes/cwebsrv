#ifndef cws_response_code_h
#define cws_response_code_h

#include <stddef.h>

typedef enum cws_response_code
{
  /*
  ============================================================================
                                Informational                                 
  ============================================================================
  */
  STATUS_CONTINUE = 100,
  STATUS_SWITCHING_PROTOCOLS = 101,
  STATUS_PROCESSING = 102,
  STATUS_EARLY_HINTS = 103,

  /*
  ============================================================================
                                 Successfull                                  
  ============================================================================
  */
  STATUS_OK = 200,
  STATUS_CREATED = 201,
  STATUS_ACCEPTED = 202,
  STATUS_NON_AUTHORITATIVE_INFORMATION = 203,
  STATUS_NO_CONTENT = 204,
  STATUS_RESET_CONTENT = 205,
  STATUS_PARTIAL_CONTENT = 206,
  STATUS_MULTI_STATUS = 207,
  STATUS_ALREADY_REPORTED = 208,
  STATUS_IM_USED = 226,

  /*
  ============================================================================
                                 Redirection                                  
  ============================================================================
  */
  STATUS_MULTIPLE_CHOICES = 300,
  STATUS_MOVED_PERMANENTLY = 301,
  STATUS_FOUND = 302,
  STATUS_SEE_OTHER = 303,
  STATUS_NOT_MODIFIED = 304,
  STATUS_USER_PROXY = 305,
  STATUS_SWITCH_PROXY = 306,
  STATUS_TEMPORARY_REDIRECT = 307,
  STATUS_PERMANENT_REDIRECT = 308,

  /*
  ============================================================================
                                 Client Error                                 
  ============================================================================
  */
  STATUS_BAD_REQUEST = 400,
  STATUS_UNAUTHORIZED = 401,
  STATUS_PAYMENT_REQUIRED = 402,
  STATUS_FORBIDDEN = 403,
  STATUS_NOT_FOUND = 404,
  STATUS_METHOD_NOT_ALLOWED = 405,
  STATUS_NOT_ACCEPTABLE = 406,
  STATUS_PROXY_AUTHENTICATION_REQUIRED = 407,
  STATUS_REQUEST_TIMEOUT = 408,
  STATUS_CONFLICT = 409,
  STATUS_GONE = 410,
  STATUS_LENGTH_REQUIRED = 411,
  STATUS_PRECONDITION_FAILED = 412,
  STATUS_PAYLOAD_TOO_LARGE = 413,
  STATUS_URI_TOO_LONG = 414,
  STATUS_UNSUPPORTED_MEDIA_TYPE = 415,
  STATUS_RANGE_NOT_SATISFYABLE = 416,
  STATUS_EXPECTATION_FAILED = 417,
  STATUS_IM_A_TEAPOT = 418,
  STATUS_MISDIRECTED_REQUEST = 421,
  STATUS_UNPROCESSABLE_ENTITY = 422,
  STATUS_LOCKED = 423,
  STATUS_FAILED_DEPENDENCY = 424,
  STATUS_TOO_EARLY = 425,
  STATUS_UPGRADE_REQUIRED = 426,
  STATUS_PRECONDITION_REQUIRED = 428,
  STATUS_TOO_MANY_REQUESTS = 429,
  STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
  STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,

  /*
  ============================================================================
                                 Server Error                                 
  ============================================================================
  */
  STATUS_INTERNAL_SERVER_ERROR = 500,
  STATUS_NOT_IMPLEMENTED = 501,
  STATUS_BAD_GATEWAY = 502,
  STATUS_SERVICE_UNAVAILABLE = 503,
  STATUS_GATEWAY_TIMEOUT = 504,
  STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,
  STATUS_VARIANT_ALSO_NEGOTIATES = 506,
  STATUS_INSUFFICIENT_STORAGE = 507,
  STATUS_LOOP_DETECTED = 508,
  STATUS_NOT_EXTENDED = 510,
  STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511
} cws_response_code_t;

/**
 * @brief Turn the numeric response code into it's string representation
 * 
 * @param code Response code to stringify
 * @return const char* Stringified result, NULL for invalid codes
 */
const char *cws_response_code_stringify(cws_response_code_t code);

#endif