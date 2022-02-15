#include "cws/cws_uri.h"

/**
 * @brief Clean up a cws_uri struct that is about to be destroyed
 */
INLINED static void cws_uri_cleanup(mman_meta_t *ref)
{
  mman_dealloc(((cws_uri_t *) ref->ptr)->path);
  mman_dealloc(((cws_uri_t *) ref->ptr)->query);
  mman_dealloc(((cws_uri_t *) ref->ptr)->raw_uri);
}

bool cws_uri_parse(char *raw_uri, cws_uri_t **output, char **error_msg)
{
  // Clone the raw URI for internal storage
  scptr char *uri_copy = strclone(raw_uri, CWS_URI_MAXLEN);
  if (rp_exit(!raw_uri, error_msg, "The URI was too long (max=%lu)!\n", CWS_URI_MAXLEN)) return NULL;

  // Parse the path without parameters
  size_t raw_uri_offs = 0;
  char *path = partial_strdup(raw_uri, &raw_uri_offs, "?", false);
  if (rp_exit(!path, error_msg, "Could not parse the path!")) return false;

  htable_t *query_table = htable_make(CWS_MIN_QUERYPARAMS, CWS_MAX_QUERYPARAMS, NULL);

  // Parse all available headers
  char *curr_param;
  while (
    // Parse next header line
    (curr_param = partial_strdup(raw_uri, &raw_uri_offs, "&", false))
  )
  {
    // Split on "="
    size_t curr_param_offs = 0;
    scptr char *param_key = partial_strdup(curr_param, &curr_param_offs, "=", false);
    char *param_value = partial_strdup(curr_param, &curr_param_offs, "\0", false);
    if (rp_exit(!param_key || !param_value, error_msg, "Malformed query parameter!")) return NULL;

    // Ensure existence of the value list array
    dynarr_t *curr_value_list;
    if (htable_fetch(query_table, param_key, (void **) &curr_value_list) == HTABLE_KEY_NOT_FOUND)
    {
      curr_value_list = dynarr_make(CWS_MIN_SAME_QUERYPARAMS, CWS_MAX_SAME_QUERYPARAMS, mman_dealloc);
      htable_result_t ins_res = htable_insert(query_table, param_key, curr_value_list);
      if (rp_exit(ins_res == HTABLE_FULL, error_msg, "Too many query parameters (max=%lu)!", CWS_MAX_QUERYPARAMS)) return NULL;
    }

    // Push value into key-array
    dynarr_result_t arr_res = dynarr_push(curr_value_list, param_value, NULL);
    if (rp_exit(arr_res == dynarr_FULL, error_msg, "Too many same-named query parameters (max=%lu)!", CWS_MAX_SAME_QUERYPARAMS)) return NULL;

    mman_dealloc(curr_param);
  }

  cws_uri_t *res = (cws_uri_t *) mman_alloc(sizeof(cws_uri_t), 1, cws_uri_cleanup);
  res->raw_uri = mman_ref(uri_copy); // needs mman freeing
  res->path = path; // needs mman freeing
  res->query = query_table; // needs mman freeing

  if (output) *output = mman_ref(res);
  return true;
}