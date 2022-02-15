#ifndef cws_uri_h
#define cws_uri_h

#include "cws/cws_common.h"
#include "datastruct/htable.h"
#include "util/mman.h"
#include "util/partial_strdup.h"
#include "util/strclone.h"
#include "datastruct/dynarr.h"

/*
============================================================================
                               Configuration                                
============================================================================
*/

// Default number of headers the hash table gets allocated to
#define CWS_MIN_QUERYPARAMS 4UL

// Maximum number of headers the hash table can grow to
#define CWS_MAX_QUERYPARAMS 128UL

// Default number of same named query parameter values (array)
#define CWS_MIN_SAME_QUERYPARAMS 128UL

// Maximum number of same named query parameter values (array)
#define CWS_MAX_SAME_QUERYPARAMS 128UL

// Maximum length in characters of the raw URI
#define CWS_URI_MAXLEN 1024UL

/*
============================================================================
                                    URI                                     
============================================================================
*/

typedef struct cws_uri
{
  // Raw URI as found in the request
  char *raw_uri;

  // Absolute, requested path, starting with a /
  char *path;

  // key: query parameter name (string)
  // value: query parameter values (zero terminated string array)
  htable_t *query;
} cws_uri_t;

/**
 * @brief Parse a URI and all it's elements by it's raw string
 * 
 * @param raw_uri Raw URI string
 * @param output URI output buffer
 * @param error_msg Error message output buffer
 * 
 * @return true URI parsed successfully
 * @return false Could not parse URI
 */
bool cws_uri_parse(char *raw_uri, cws_uri_t **output, char **error_msg);

#endif