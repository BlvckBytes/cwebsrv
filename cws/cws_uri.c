#include "cws/cws_uri.h"

bool cws_uri_parse(char *raw_uri, cws_uri_t **output, const char **error_msg)
{
  // Parse the path without parameters
  size_t raw_uri_offs = 0;
  char *path = cws_strdup_until(raw_uri, &raw_uri_offs, "?", false);
  if (rp_exit(!path, error_msg, "Could not parse the path!")) return false;

  htable_t *query = htable_alloc(CWS_DEF_NUM_QUERYPARAMS, CWS_MAX_NUM_QUERYPARAMS, dynarr_free);

  // Parse all available headers
  char *curr_param;
  while (
    // Parse next header line
    (curr_param = cws_strdup_until(raw_uri, &raw_uri_offs, "&", false))
  )
  {
    size_t curr_param_offs = 0;
    scptr char *param_key = cws_strdup_until(curr_param, &curr_param_offs, "=", false);
    char *param_value = cws_strdup_until(curr_param, &curr_param_offs, "\0", false);
    if (rp_exit(!param_key || !param_value, error_msg, "Malformed query parameter in request!")) return NULL;

    // Ensure existence of the value list array
    dynarr_t *curr_value_list;
    if (htable_fetch(query, param_key, (void **) &curr_value_list) == htable_KEY_NOT_FOUND)
    {
      curr_value_list = dynarr_alloc(CWS_NUM_SAME_QUERYPARAMS, CWS_MAX_NUM_SAME_QUERYPARAMS, mman_dealloc_direct);
      htable_result_t ins_res = htable_insert(query, param_key, curr_value_list);
      if (rp_exit(ins_res == htable_FULL, error_msg, "Too many query parameters in request!")) return NULL;
    }

    // Push value into key-array
    dynarr_result_t arr_res = dynarr_push(curr_value_list, param_value, NULL);
    if (rp_exit(arr_res == dynarr_FULL, error_msg, "Too many same-named query parameters in request!")) return NULL;

    mman_dealloc_direct(curr_param);
  }

  cws_uri_t *res = (cws_uri_t *) mman_alloc(sizeof(cws_uri_t), cws_uri_free);
  res->raw_uri = strdup(raw_uri); // needs freeing
  res->path = path; // needs mman freeing
  res->query = query; // needs mman freeing

  if (output)
    *output = mman_ref(res);

  return true;
}

void cws_uri_free(void *ref)
{
  cws_uri_t *uri = (cws_uri_t *) ref;
  if (!uri) return;
  mman_dealloc_direct(uri->path);
  mman_dealloc_direct(uri->query);
  free(uri->raw_uri);
  free(uri);
  uri = NULL;
}