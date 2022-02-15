#include "cws/cws_request.h"

// TODO: Have a separate function for each parsing step and make it set the str_offs to SIZE_MAX on error, then just 
// call them all on the same reference and have them return internally when str_offs reached that constant (error-marker)

/**
 * @brief Clean up a cws_request struct that is about to be destroyed
 */
INLINED static void cws_request_cleanup(mman_meta_t *ref)
{
  mman_dealloc(((cws_request_t *) ref->ptr)->headers);
  mman_dealloc(((cws_request_t *) ref->ptr)->uri);
  mman_dealloc(((cws_request_t *) ref->ptr)->body);
}

/**
 * @brief Request parser header parsing subroutine
 * 
 * @param request Request string
 * @param str_offs Current offset reference
 * @param error_msg Error message output reference
 * @return htable_t* Table reference on success, NULL on errors
 */
INLINED static htable_t *parse_headers(char *request, size_t *str_offs, char **error_msg)
{
  // Create headers hash table
  scptr htable_t *headers = htable_make(16, CWS_MAX_NUM_HEADERS, mman_dealloc);

  // Parse all available headers
  char *curr_header;
  while (
    // Parse next header line
    (curr_header = partial_strdup(request, str_offs, "\n", false)) &&

    // Stop when encountering an empty line
    !(curr_header[0] == 0 || strcmp(curr_header, "\n") == 0 || strcmp(curr_header, "\r\n") == 0)
  )
  {
    // Split into key and value, based on first occurrence of :
    size_t curr_header_offs = 0;
    scptr char *header_key = partial_strdup(curr_header, &curr_header_offs, ": ", false);
    scptr char *header_value = partial_strdup(curr_header, &curr_header_offs, "\n", false);
    if (rp_exit(!header_key || !header_value, error_msg, "Malformed header!")) return NULL;

    htable_result_t ins_res = htable_insert(headers, header_key, mman_ref(header_value));
    if (rp_exit(ins_res == HTABLE_KEY_ALREADY_EXISTS, error_msg, "Duplicate header in request!")) return NULL;
    if (rp_exit(ins_res == HTABLE_FULL, error_msg, "Too many headers (max=%u)!", CWS_MAX_NUM_HEADERS)) return NULL;

    mman_dealloc(curr_header);
  }

  return mman_ref(headers);
}

cws_request_t *cws_request_parse(char *request, char **error_msg)
{
  // Request string offset trackers for parsing
  size_t str_offs = 0, vers_offs = 0;

  // Cut method string
  scptr char *method_str = partial_strdup(request, &str_offs, " ", false);
  if (rp_exit(!method_str, error_msg, "HTTP method missing!")) return NULL;

  // Parse method string
  cws_http_method_t method;
  if (rp_exit(!cws_http_method_parse(method_str, &method), error_msg, "Unsupported HTTP method!")) return NULL;

  // Cut URI string
  scptr char *raw_uri = partial_strdup(request, &str_offs, " ", false);
  if (rp_exit(!raw_uri, error_msg, "URI missing!")) return NULL;

  // Parse URI string
  scptr cws_uri_t *uri;
  if (rp_exit(!cws_uri_parse(raw_uri, &uri, error_msg), error_msg, "Could not parse the URI!")) return NULL;

  // Cut HTTP version
  scptr char *raw_vers = partial_strdup(request, &str_offs, "\n", false);
  if (rp_exit(!raw_vers, error_msg, "HTTP version missing!")) return NULL;

  // Skip "HTTP/"
  partial_strdup(raw_vers, &vers_offs, "/", true);

  // Cut major version
  scptr char *vers_major_str = partial_strdup(raw_vers, &vers_offs, ".", false);
  if (rp_exit(!vers_major_str, error_msg, "Major HTTP version missing!")) return NULL;

  // Cut minor version
  scptr char *vers_minor_str = partial_strdup(raw_vers, &vers_offs, "\n", false);
  if (rp_exit(!vers_minor_str, error_msg, "Minor HTTP version missing!")) return NULL;

  // Parse major/minor version
  int vers_major = 0, vers_minor = 0;
  if (rp_exit((
    str2int(&vers_major, vers_major_str, 10) != STR2INT_SUCCESS ||
    str2int(&vers_minor, vers_minor_str, 10) != STR2INT_SUCCESS),
    error_msg, "HTTP version major/minor non-numerical!"
  )) return NULL;

  // Parse the headers
  scptr htable_t *headers = parse_headers(request, &str_offs, error_msg);
  if (!headers) return NULL;

  // Just get the rest of the body
  scptr char *body = partial_strdup(request, &str_offs, "\0", false);

  // Allocate request and set it's members
  scptr cws_request_t *req = (cws_request_t *) mman_alloc(sizeof(cws_request_t), 1, cws_request_cleanup);
  req->headers = mman_ref(headers); // needs mman freeing
  req->method = method; // no freeing
  req->uri = mman_ref(uri); // needs mman freeing
  req->body = mman_ref(body); // needs mman freeing
  req->http_ver_major = vers_major; // no freeing
  req->http_ver_minor = vers_minor; // no freeing

  return mman_ref(req);
}

void cws_request_print(cws_request_t *request)
{
  // Nothing to print
  if (!request)
  {
    printf("Invalid request!\n");
    return;
  }

  printf("------------< HTTP Request >------------\n");

  printf("Method: %s\n", cws_http_method_stringify(request->method));
  printf("Version: HTTP/%d.%d\n", request->http_ver_major, request->http_ver_minor);
  
  if (request->uri)
  {
    printf("URI Raw: %s\n", request->uri->raw_uri);
    printf("URI Path: %s\n", request->uri->path);
    printf("URI Parameters:\n");

    htable_t *query = request->uri->query;
    if (query)
    {
      char **keys;
      htable_list_keys(query, &keys);

      for (char **key = keys; *key; key++)
      {
        printf("\"%s\":\n", *key);

        dynarr_t *values;
        if (htable_fetch(query, *key, (void **) &values) != HTABLE_SUCCESS)
        {
          printf("error!\n");
          continue;
        }

        for (size_t i = 0; i < values->_array_size; i++)
          if (values->items[i]) printf("-\"%s\"\n", (char *) values->items[i]);
      }
    }
    else
      printf("URI parameters not parsed!\n");
  }
  else printf("URI not parsed!\n");


  printf("Headers:\n");
  if (request->headers)
    cws_print_htable_keys(request->headers, false);
  else
    printf("Headers not parsed!\n");

  printf("Body:\n");
  printf("%s\n", request->body ? request->body : "<no body>");

  printf("------------< HTTP Request >------------\n");
}