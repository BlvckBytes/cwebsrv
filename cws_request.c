#include "cws_request.h"

// TODO: Improve MMAN by getting rid of all the strdup's

cws_request_t *cws_request_parse(char *request, const char **error_msg)
{
  // Request string offset during parsing
  size_t str_offs = 0;

  // Cut method string
  mman char *method_str = cws_strdup_until(request, &str_offs, " ", false);

  if (!method_str)
  {
    if (error_msg)
      *error_msg = "HTTP method missing!";
    return NULL;
  }

  // Parse method string
  cws_http_method_t method;

  if (!cws_http_method_parse(method_str, &method))
  {
    if (error_msg)
      *error_msg = "Unsupported HTTP method!";
    return NULL;
  }

  // Cut raw uri
  mman char *raw_uri = cws_strdup_until(request, &str_offs, " ", false);

  if (!raw_uri)
  {
    if (error_msg)
      *error_msg = "URI missing!";
    return NULL;
  }

  // Parse URI from raw string
  cws_uri_t *uri;
  if (!cws_uri_parse(raw_uri, &uri, error_msg)) return NULL;

  // Cut raw HTTP version, skip "HTTP/" and cut major & minor
  size_t vers_offs = 0;
  mman char *raw_vers = cws_strdup_until(request, &str_offs, "\n", false);
  cws_strdup_until(raw_vers, &vers_offs, "/", true);
  mman char *vers_major_str = cws_strdup_until(raw_vers, &vers_offs, ".", false);
  mman char *vers_minor_str = cws_strdup_until(raw_vers, &vers_offs, "\n", false);

  if (!raw_vers || !vers_major_str || !vers_minor_str)
  {
    if (error_msg)
      *error_msg = "HTTP version missing or malformed!";
    return NULL;
  }

  int vers_major = 0, vers_minor = 0;
  if (
    str2int(&vers_major, vers_major_str, 10) != STR2INT_SUCCESS ||
    str2int(&vers_minor, vers_minor_str, 10) != STR2INT_SUCCESS
  )
  {
    if (error_msg)
      *error_msg = "HTTP version major/minor non-numerical!";
    return NULL;
  }

  htable_t *headers = htable_alloc(4, 128);
  char *curr_header;
  while (
    // Parse next header line
    (curr_header = cws_strdup_until(request, &str_offs, "\n", false)) &&

    // Stop when encountering an empty line
    !(strcmp(curr_header, "\n") == 0 || strcmp(curr_header, "\r\n") == 0)
  )
  {
    // Split into key and value, based on first occurrence of :
    size_t curr_header_offs = 0;
    mman char *header_key = cws_strdup_until(curr_header, &curr_header_offs, ": ", false);
    mman char *header_value = cws_strdup_until(curr_header, &curr_header_offs, "\n", false);

    if (!header_key || !header_value)
    {
      if (error_msg)
        *error_msg = "Malformed header in the request!";
      return NULL;
    }

    // Insert with duplicated value, since the strdup result will be destroyed after this iteration
    htable_result_t ins_res = htable_insert(headers, header_key, strdup(header_value));

    if (ins_res == htable_KEY_ALREADY_EXISTS)
    {
      if (error_msg)
        *error_msg = "Duplicate header in the request!";
      return NULL;
    }

    if (ins_res == htable_FULL)
    {
      if (error_msg)
        *error_msg = "Too many headers in the request!";
      return NULL;
    }
  }

  cws_request_t *req = (cws_request_t *) malloc(sizeof(cws_request_t));

  req->headers = headers;
  req->method = method;
  req->uri = uri;

  req->body = NULL;

  req->http_ver_major = vers_major;
  req->http_ver_minor = vers_minor;

  return req;
}

void cws_request_free(cws_request_t *request)
{
  if (!request) return;

  if (request->headers)
    htable_free(request->headers);

  if (request->body)
    free(request->body);

  if (request->uri)
    cws_uri_free(request->uri);

  free(request);
  request = NULL;
}

void cws_request_print(cws_request_t *request)
{
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

    if (request->uri->query)
      cws_print_htable_keys(request->uri->query, true);
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
  printf("%s\n", request->body ? request->body : "Body not parsed!");

  printf("------------< HTTP Request >------------\n");
}