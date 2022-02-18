#include "cws/cws_response.h"

/*
============================================================================
                               Building stages                              
============================================================================
*/

bool rb_status_line (
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  htable_t *header_buf,
  char *body,
  size_t *offs,
  char **response
)
{
  // Stringify and check the status-code
  const char *code_str = cws_response_code_stringify(code);
  if (!code_str) return false;

  // Append status line
  strfmt(response, offs, "HTTP/1.1 %d %s" CRLF, code, code_str);
  return true;
}

bool rb_headers_append(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  htable_t *header_buf,
  char *body,
  size_t *offs,
  char **response
)
{
  // List all available header keys
  char **keys;
  htable_list_keys(header_buf, &keys);

  // Loop all keys
  for (char **key = keys; *key; key++)
  {
    // Get the corresponding value
    char *value;
    if (htable_fetch(header_buf, *key, (void **) &value) != HTABLE_SUCCESS) return false;

    // Append to the response
    strfmt(response, offs, "%s: %s" CRLF, *key, value);
  }

  return true;
}

bool rb_headers_required(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  htable_t *header_buf,
  char *body,
  size_t *offs,
  char **response
)
{
  // Determine the length of the body
  size_t body_len = body ? strlen(body) : 0;

  // Insert all required headers
  if (htable_insert(header_buf, "Connection", strfmt_direct("Closed")) != HTABLE_SUCCESS) return false;
  if (htable_insert(header_buf, "Content-Type", strfmt_direct("text/html")) != HTABLE_SUCCESS) return false;
  if (htable_insert(header_buf, "Server", strfmt_direct("cWebSrv/0.0.1")) != HTABLE_SUCCESS) return false;
  if (htable_insert(header_buf, "Content-Length", strfmt_direct("%lu", body_len)) != HTABLE_SUCCESS) return false;

  return true;
}

bool rb_headers_additional(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  htable_t *header_buf,
  char *body,
  size_t *offs,
  char **response
)
{
  // No additional headers desired
  if (!headers) return true;

  // Append all additional headers to the buffer, skip duplicates (predefineds are not changable)
  if (htable_append_table(header_buf, headers, HTABLE_AM_SKIP) != HTABLE_SUCCESS) return false;

  return true;
}

bool rb_empty_line(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  htable_t *header_buf,
  char *body,
  size_t *offs,
  char **response
)
{
  // Just add an empty line
  strfmt(response, offs, CRLF);
  return true;
}

bool rb_body(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  htable_t *header_buf,
  char *body,
  size_t *offs,
  char **response
)
{
  // Just add the body
  if (body) strfmt(response, offs, "%s", body);
  return true;
}

/*
============================================================================
                                Building chain                              
============================================================================
*/

bool cws_response_send(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  char *body
)
{
  // Allocate some space for the message to be built
  scptr char *message = mman_alloc(sizeof(char), CWS_RESPONSE_BUFFER, NULL);
  size_t message_offs = 0;

  // Register stages in the right order here
  cws_response_builder_t building_stages[] = {
    rb_status_line,
    rb_headers_required,
    rb_headers_additional,
    rb_headers_append,
    rb_empty_line,
    rb_body
  };

  // Execute all stages
  scptr htable_t *header_buf = htable_make(8, CWS_RESPONSE_MAX_HEADERS, mman_dealloc);
  size_t num_stages = sizeof(building_stages) / sizeof(cws_response_builder_t);
  for (size_t i = 0; i < num_stages; i++)
  {
    if (!building_stages[i](
      client,
      code,
      headers,
      header_buf,
      body,
      &message_offs,
      &message
    )) return false;
  }

  // Send the finished message to the client
  send(client->descriptor, message, strlen(message), 0);
  return true;
}