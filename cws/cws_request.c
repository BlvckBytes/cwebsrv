#include "cws/cws_request.h"

/**
 * @brief Request parser header parsing subroutine
 * 
 * @param request Request string
 * @param str_offs Current offset reference
 * @param error_msg Error message output reference
 * @return htable_t* Table reference on success, NULL on errors
 */
static htable_t *parse_headers(char *request, size_t *str_offs, const char **error_msg)
{
  // Create headers hash table
  scptr htable_t *headers = htable_alloc(CWS_DEF_NUM_HEADERS, CWS_MAX_NUM_HEADERS, free);

  // Parse all available headers
  char *curr_header;
  while (
    // Parse next header line
    (curr_header = cws_strdup_until(request, str_offs, "\n", false)) &&

    // Stop when encountering an empty line
    !(curr_header[0] == 0 || strcmp(curr_header, "\n") == 0 || strcmp(curr_header, "\r\n") == 0)
  )
  {
    // Split into key and value, based on first occurrence of :
    size_t curr_header_offs = 0;
    scptr char *header_key = cws_strdup_until(curr_header, &curr_header_offs, ": ", false);
    scptr char *header_value = cws_strdup_until(curr_header, &curr_header_offs, "\n", false);
    if (rp_exit(!header_key || !header_value, error_msg, "Malformed header in request!")) return NULL;

    htable_result_t ins_res = htable_insert(headers, header_key, strdup(header_value));
    if (rp_exit(ins_res == htable_KEY_ALREADY_EXISTS, error_msg, "Duplicate header in request!")) return NULL;
    if (rp_exit(ins_res == htable_FULL, error_msg, "Too many headers in request!")) return NULL;

    mman_dealloc_direct(curr_header);
  }

  return mman_ref(headers);
}

cws_request_t *cws_request_parse(char *request, const char **error_msg)
{
  // Request string offset trackers for parsing
  size_t str_offs = 0, vers_offs = 0;

  // Cut method string
  scptr char *method_str = cws_strdup_until(request, &str_offs, " ", false);
  if (rp_exit(!method_str, error_msg, "HTTP method missing!")) return NULL;

  // Parse method string
  cws_http_method_t method;
  if (rp_exit(!cws_http_method_parse(method_str, &method), error_msg, "Unsupported HTTP method!")) return NULL;

  // Cut URI string
  scptr char *raw_uri = cws_strdup_until(request, &str_offs, " ", false);
  if (rp_exit(!raw_uri, error_msg, "URI missing!")) return NULL;

  // Parse URI string
  cws_uri_t *uri;
  if (rp_exit(!cws_uri_parse(raw_uri, &uri, error_msg), error_msg, "Could not parse the URI!")) return NULL;

  // Cut HTTP version
  scptr char *raw_vers = cws_strdup_until(request, &str_offs, "\n", false);
  if (rp_exit(!raw_vers, error_msg, "HTTP version missing!")) return NULL;

  // Skip "HTTP/"
  cws_strdup_until(raw_vers, &vers_offs, "/", true);

  // Cut major version
  scptr char *vers_major_str = cws_strdup_until(raw_vers, &vers_offs, ".", false);
  if (rp_exit(!vers_major_str, error_msg, "Major HTTP version missing!")) return NULL;

  // Cut minor version
  scptr char *vers_minor_str = cws_strdup_until(raw_vers, &vers_offs, "\n", false);
  if (rp_exit(!vers_minor_str, error_msg, "Minor HTTP version missing!")) return NULL;

  // Parse major/minor version
  int vers_major = 0, vers_minor = 0;
  if (rp_exit((
    str2int(&vers_major, vers_major_str, 10) != STR2INT_SUCCESS ||
    str2int(&vers_minor, vers_minor_str, 10) != STR2INT_SUCCESS),
    error_msg, "HTTP version major/minor non-numerical!"
  )) return NULL;

  // Parse the headers
  htable_t *headers = parse_headers(request, &str_offs, error_msg);
  if (!headers) return NULL;

  // Just get the rest of the body
  char *body = cws_strdup_until(request, &str_offs, "\0", false);

  // Allocate request and set it's members
  scptr cws_request_t *req = (cws_request_t *) mman_alloc(sizeof(cws_request_t), cws_request_free);
  req->headers = headers; // needs mman freeing
  req->method = method; // no freeing
  req->uri = uri; // needs mman freeing
  req->body = body; // needs mman freeing
  req->http_ver_major = vers_major; // no freeing
  req->http_ver_minor = vers_minor; // no freeing

  return mman_ref(req);
}

void cws_request_free(void *ref)
{
  cws_request_t *request = (cws_request_t *) ref;
  if (!request) return;

  mman_dealloc_direct(request->headers);
  mman_dealloc_direct(request->uri);
  mman_dealloc_direct(request->body);
  mman_dealloc_direct(request);
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
        if (htable_fetch(query, *key, (void **) &values) != htable_SUCCESS)
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