#include "cws_uri.h"

bool cws_uri_parse(char *raw_uri, cws_uri_t **output, const char **error_message)
{
  cws_uri_t *res = (cws_uri_t *) malloc(sizeof(cws_uri_t));
  res->raw_uri = strdup(raw_uri);
  res->path = NULL;
  res->query = NULL;
  *output = res;
  return true;
}

void cws_uri_free(cws_uri_t *uri)
{
  if (!uri) return;
  if (uri->path) free(uri->path);
  if (uri->raw_uri) free(uri->raw_uri);
  free(uri);
}