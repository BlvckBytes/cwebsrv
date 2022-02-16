#include "cws/cws_common.h"

void cws_print_addr_in(struct sockaddr_in addr)
{
  // Print octet by octet using bitmasking
  for (uint8_t i = 0; i < 4; i++)
    printf(
      "%" PRIu8 "%s",
      (uint8_t) ((addr.sin_addr.s_addr >> (8 * i)) & 0xFF),
      i != 3 ? "." : ""
    );

  // Print port after reversing endianness
  printf(":%" PRIu16, htons(addr.sin_port));
}

bool rp_exit(bool exit, char **error_msg, const char *error_fmt, ...)
{
  if (!exit | !error_msg) return false;

  va_list ap;
  va_start(ap, error_fmt);

  // Format error string using specified format
  scptr char *buf = mman_alloc(sizeof(char), 128, NULL);
  bool res = vstrfmt(&buf, NULL, error_fmt, ap);

  // On success, store output pointer
  if (res) *error_msg = mman_ref(buf);

  va_end(ap);
  return res;
}

void cws_discard_request(cws_client_t *client)
{
  char waste_buf[128];
  int read;
  bool waited = false;

  // Read non-blocking with timeouts until nothing remains
  // TODO: Research on this topic, whether or not this style is really the only solution
  while (
    // Successfully read data
    (read = recv(client->descriptor, waste_buf, sizeof(waste_buf), MSG_DONTWAIT)) > 0

    // Error on recv and error means that no data is available
    // INFO: This additional step excludes polling on a dead socket (hopefully?)
    || (read < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
  )
  {
    // No data after read, quit reading now
    if (waited && read <= 0)
      break;

    // No data, not in waiting mode, wait
    if (read <= 0)
    {
      usleep(CWS_REQ_SEG_TIMEOUT_US);
      waited = true;
    }
  }
}

bool errif_resp(
  cws_client_t *client,
  bool error_cond,
  cws_response_code_t code,
  const char *message
)
{
  // Error did not occur
  if (!error_cond) return false;

  // Set up response body buffer
  scptr char *body = mman_alloc(sizeof(char), 128, NULL);
  size_t body_offs = 0;

  // Build simple and quick JSON response
  strfmt(&body, &body_offs, "{" CRLF);
  strfmt(&body, &body_offs, "\"error\": true," CRLF);
  strfmt(&body, &body_offs, "\"message\": \"%s\"," CRLF, message);
  strfmt(&body, &body_offs, "}");

  // Send response to the client after discarding their message
  cws_discard_request(client);
  cws_response_send(client, code, NULL, body);
  return true;
}