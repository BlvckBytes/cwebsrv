#include "cws_uri.h"

bool cws_uri_parse(char *raw_uri, cws_uri_t **output, const char **error_message)
{
  cws_uri_t *res = (cws_uri_t *) mman_alloc(sizeof(cws_uri_t), cws_uri_free);
  res->raw_uri = strdup(raw_uri); // needs freeing
  res->path = NULL;
  res->query = NULL;

  if (output)
    *output = mman_ref(res);

  return true;
}

void cws_uri_free(void *ref)
{
  cws_uri_t *uri = (cws_uri_t *) ref;
  if (!uri) return;
  if (uri->path) free(uri->path);
  if (uri->raw_uri) free(uri->raw_uri);
  free(uri);
  uri = NULL;
}