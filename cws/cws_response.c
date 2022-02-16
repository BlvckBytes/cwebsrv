#include "cws/cws_response.h"

bool rb_status_line (
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
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

bool rb_headers_required(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  char *body,
  size_t *offs,
  char **response
)
{
  // Mark connection as closed
  strfmt(response, offs, "Connection: Closed" CRLF);

  // TODO: Mime-types should not be hardcoded like this
  // Set content type
  strfmt(response, offs, "Content-Type: text/html" CRLF);

  // Set server name
  strfmt(response, offs, "Server: cWebSrv/0.0.1" CRLF);

  // Set content length based on body
  strfmt(response, offs, "Content-Length: %lu" CRLF, body ? strlen(body) : 0);
  return true;
}

bool rb_headers_additional(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  char *body,
  size_t *offs,
  char **response
)
{
  // No additional headers desired
  if (!headers) return true;

  // scptr char **header_keys;
  // htable_list_keys(headers, header_keys);
  // TODO: Actually write the headers to the response (not feeling it rn)

  return true;
}

bool rb_empty_line(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
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
  char *body,
  size_t *offs,
  char **response
)
{
  // Just add the body
  if (body) strfmt(response, offs, "%s", body);
  return true;
}

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
    rb_empty_line,
    rb_body
  };

  // Execute all stages
  size_t num_stages = sizeof(building_stages) / sizeof(cws_response_builder_t);
  for (size_t i = 0; i < num_stages; i++)
    if (!building_stages[i](client, code, headers, body, &message_offs, &message)) return false;

  // Send the finished message to the client
  send(client->descriptor, message, strlen(message), 0);
  return true;
}