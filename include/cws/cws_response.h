#ifndef cws_response_h
#define cws_response_h

#include "cws/cws_client.h"
#include "cws/cws_response_code.h"
#include "datastruct/htable.h"

// Initial size of the response-buffer, it grows as needed
#define CWS_RESPONSE_BUFFER 1024

typedef bool (*cws_response_builder_t)(
  cws_client_t *client,      // Response recipient
  cws_response_code_t code,  // Response code
  htable_t *headers,         // Response headers
  char *body,                // Response body
  size_t *offs,              // Offset pointer for change in place
  char **response            // Response message buffer
);

/**
 * @brief Sends a HTTP response to the active client connection
 * 
 * @param client Recipient reference
 * @param code HTTP status code
 * @param headers Additional header map, leave NULL for none
 * @param body Body contents, leave NULL for none
 * 
 * @return true Response built and sent to client
 * @return false Could not build response or connection is down
 */
bool cws_response_send(
  cws_client_t *client,
  cws_response_code_t code,
  htable_t *headers,
  char *body
);

#endif