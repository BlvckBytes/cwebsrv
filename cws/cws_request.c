#include "cws/cws_request.h"

/**
 * @brief Clean up a cws_request struct that is about to be destroyed
 */
INLINED static void cws_request_cleanup(mman_meta_t *ref)
{
  mman_dealloc(((cws_request_head_t *) ref->ptr)->headers);
  mman_dealloc(((cws_request_head_t *) ref->ptr)->uri);
}

/*
============================================================================
                               Parsing stages                               
============================================================================
*/

static bool ps_http_method(char *req, size_t *offs, cws_request_head_t *res, char **err)
{
  // Cut method string
  scptr char *method_str = partial_strdup(req, offs, " ", false);
  if (rp_exit(!method_str, err, "HTTP method missing!")) return false;

  // Parse method string
  cws_http_method_t method;
  if (rp_exit(!cws_http_method_parse(method_str, &method), err, "Unsupported HTTP method!")) return false;

  res->method = method;
  return true;
}

static bool ps_uri(char *req, size_t *offs, cws_request_head_t *res, char **err)
{
  // Cut URI string
  scptr char *raw_uri = partial_strdup(req, offs, " ", false);
  if (rp_exit(!raw_uri, err, "URI missing!")) return false;

  // Parse URI string
  scptr cws_uri_t *uri;
  if (rp_exit(!cws_uri_parse(raw_uri, &uri, err), err, "Could not parse the URI!")) return false;

  res->uri = mman_ref(uri);
  return true;
}

static bool ps_http_version(char *req, size_t *offs, cws_request_head_t *res, char **err)
{
  // Cut HTTP version
  scptr char *raw_vers = partial_strdup(req, offs, "\n", false);
  if (rp_exit(!raw_vers, err, "HTTP version missing!")) return false;

  // Skip "HTTP/"
  size_t vers_offs = 0;
  partial_strdup(raw_vers, &vers_offs, "/", true);

  // Cut major version
  scptr char *vers_major_str = partial_strdup(raw_vers, &vers_offs, ".", false);
  if (rp_exit(!vers_major_str, err, "Major HTTP version missing!")) return false;

  // Cut minor version
  scptr char *vers_minor_str = partial_strdup(raw_vers, &vers_offs, "\n", false);
  if (rp_exit(!vers_minor_str, err, "Minor HTTP version missing!")) return false;

  // Parse major/minor version
  long vers_major = 0, vers_minor = 0;
  if (rp_exit((
    longp(&vers_major, vers_major_str, 10) != LONGP_SUCCESS ||
    longp(&vers_minor, vers_minor_str, 10) != LONGP_SUCCESS),
    err, "HTTP version major/minor non-numerical!"
  )) return false;

  res->http_ver_major = vers_major;
  res->http_ver_minor = vers_minor;
  return true;
}

static bool ps_headers(char *req, size_t *offs, cws_request_head_t *res, char **err)
{
  // Create headers hash table
  scptr htable_t *headers = htable_make(16, CWS_MAX_NUM_HEADERS, mman_dealloc);

  // Parse all available headers
  char *curr_header;
  while (
    // Parse next header line
    (curr_header = partial_strdup(req, offs, "\n", false)) &&

    // Stop when encountering an empty line
    !(curr_header[0] == 0 || strcmp(curr_header, "\n") == 0 || strcmp(curr_header, "\r\n") == 0)
  )
  {
    // Split into key and value, based on first occurrence of ":"
    size_t curr_header_offs = 0;
    scptr char *header_key = partial_strdup(curr_header, &curr_header_offs, ": ", false);
    scptr char *header_value = partial_strdup(curr_header, &curr_header_offs, "\n", false);
    if (rp_exit(!header_key || !header_value, err, "Malformed header!")) return false;

    // Insert into the hash table
    htable_result_t ins_res = htable_insert(headers, header_key, mman_ref(header_value));
    if (rp_exit(ins_res == HTABLE_KEY_ALREADY_EXISTS, err, "Duplicate header in request!")) return false;
    if (rp_exit(ins_res == HTABLE_FULL, err, "Too many headers (max=%u)!", CWS_MAX_NUM_HEADERS)) return false;

    mman_dealloc(curr_header);
  }

  res->headers = mman_ref(headers);
  return true;
}

static bool ps_body_part(char *req, size_t *offs, cws_request_head_t *res, char **err)
{
  // Just get the rest as the body
  scptr char *body = partial_strdup(req, offs, "\0", false);
  res->body_part = mman_ref(body);
  return true;
}

/*
============================================================================
                               Parsing chain                                
============================================================================
*/

cws_request_head_t *cws_request_head_parse(char *request, char **error_msg)
{
  // Allocate an empty request
  scptr cws_request_head_t *req = (cws_request_head_t *) mman_alloc(sizeof(cws_request_head_t), 1, cws_request_cleanup);

  // Register stages in the right order here
  cws_head_parser_t parsing_stages[] = {
    ps_http_method,
    ps_uri,
    ps_http_version,
    ps_headers,
    ps_body_part
  };

  // Execute all stages
  size_t req_offs = 0;
  size_t num_stages = sizeof(parsing_stages) / sizeof(cws_head_parser_t);
  for (size_t i = 0; i < num_stages; i++)
    if (!parsing_stages[i](request, &req_offs, req, error_msg)) return NULL;

  return mman_ref(req);
}

/*
============================================================================
                                 Printing                                   
============================================================================
*/

void cws_request_head_print(cws_request_head_t *request)
{
  if (!request) return;

  printf("----------< HTTP Request Head >----------\n");
  printf("Method: %s\n", cws_http_method_stringify(request->method));
  printf("Version: HTTP/%ld.%ld\n", request->http_ver_major, request->http_ver_minor);

  cws_uri_t *uri = request->uri;
  if (uri)
  {
    printf("URI Raw: %s\n", request->uri->raw_uri);
    printf("URI Path: %s\n", request->uri->path);
    printf("URI Parameters:\n");
    if (uri->query)
    {
      scptr char *res = htable_dump_hr(uri->query, (stringifier_t) dynarr_dump_hr_strs);
      printf("%s", res);
    }
    else printf("URI parameters not parsed!\n");
  }
  else printf("URI not parsed!\n");

  printf("Headers:\n");
  if (request->headers)
  {
    scptr char *res = htable_dump_hr(request->headers, NULL);
    printf("%s", res);
  }
  else printf("Headers not parsed!\n");
  printf("----------< HTTP Request Head >----------\n");
}