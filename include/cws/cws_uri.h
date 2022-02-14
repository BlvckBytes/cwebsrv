#ifndef cws_uri_h
#define cws_uri_h

#include "cws/cws_util.h"
#include "datastruct/htable.h"
#include "util/mman.h"
#include "datastruct/dynarr.h"

// Default number of headers the hash table gets allocated to
#define CWS_DEF_NUM_QUERYPARAMS 4UL

// Maximum number of headers the hash table can grow to
#define CWS_MAX_NUM_QUERYPARAMS 128UL

// Default number of same named query parameter values (array)
#define CWS_NUM_SAME_QUERYPARAMS 128UL

// Maximum number of same named query parameter values (array)
#define CWS_MAX_NUM_SAME_QUERYPARAMS 128UL

typedef struct
{
  // Raw URI as found in the request
  char *raw_uri;

  // Absolute, requested path, starting with a /
  char *path;

  // key: query parameter name (string)
  // value: query parameter values (zero terminated string array)
  htable_t *query;
} cws_uri_t;

bool cws_uri_parse(char *raw_uri, cws_uri_t **output, const char **error_msg);

void cws_uri_free(void *ref);

#endif