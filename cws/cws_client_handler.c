#include "cws/cws_client_handler.h"

static void cws_print_prefix(cws_client_t *client)
{
  printf("[");
  cws_print_addr_in(*(client->address));
  printf("]: ");
}

/**
 * @brief Calculate the remaining length of a request based on it's head and
 * the Content-Length header's value
 * 
 * @param client Client for error-reporting
 * @param head Parsed head of request
 * @return long Remaining number of bytes
 */
INLINED static long cws_remaining_len(cws_client_t *client, cws_request_head_t *head, char **err)
{
  char *content_length;
  if (htable_fetch(head->headers, "Content-Length", (void **) &content_length) != HTABLE_SUCCESS)
  {
    *err = "Error: Could not find content-length header!\n";
    return 0;
  }

  long content_length_i = 0;
  if (longp(&content_length_i, content_length, 10) != LONGP_SUCCESS)
  {
    *err = "Error: Could not parse content-length as an integer!\n";
    return 0;
  }

  long body_part_len = head->body_part ? strlen(head->body_part) : 0;
  return content_length_i - body_part_len;
}

static void cws_serve_client(void *arg)
{
  // Begin serve by logging
  scptr cws_client_t *client = (cws_client_t *) arg;
  cws_print_prefix(client);
  printf("Now serving request in another thread!\n");

  // Read buffer management
  scptr char *message_seg = mman_alloc(sizeof(char), CWS_HANDLER_SEGLEN, NULL);
  scptr char *message = mman_alloc(sizeof(char), CWS_HANDLER_SEGLEN, NULL);
  size_t message_offs = 0, read_size = 0;

  // Read all segments
  scptr cws_request_head_t *head;
  long seg_data_remaining = 1;
  bool first_seg = true;
  while (
    seg_data_remaining > 0 // There is still data to be read
    && (read_size = recv(client->descriptor, message_seg, CWS_HANDLER_SEGLEN, 0)) > 0) // And the connection is still available
  {
    if (first_seg)
    {
      // Parse head
      scptr char *err;
      head = cws_request_head_parse(message_seg, &err);
      if (errif_resp(client, err, STATUS_BAD_REQUEST, err)) break;

      // Calculate remaining length
      seg_data_remaining = cws_remaining_len(client, head, &err);
      if (errif_resp(client, err, STATUS_BAD_REQUEST, err)) break;
      
      // Copy head into message buffer
      if (errif_resp(
        client,
        !strfmt(&message, &message_offs, "%s", head->body_part),
        STATUS_INTERNAL_SERVER_ERROR, "Could not allocate space for the head!"
      )) break;

      // First segment completed
      first_seg = false;
      continue;
    }

    // Concat segment into message
    if (errif_resp(
      client,
      !strfmt(&message, &message_offs, "%s", message_seg),
      STATUS_INTERNAL_SERVER_ERROR, "Could not allocate space for a segment!"
    )) break;

    // Decrement remaining segment data by what just has been read
    // Clear segment buffer and advance to next segment
    seg_data_remaining -= read_size;
    memset(message_seg, 0, CWS_HANDLER_SEGLEN);
  }

  // Terminate final message
  message[++message_offs] = 0;

  cws_print_prefix(client);
  printf("Done parsing request message (%lu bytes)!\n", strlen(message));
  cws_request_head_print(head);

  // Respond with this simple test response
  scptr htable_t *headers = htable_make(3, 16, mman_dealloc);

  // This header should be added
  htable_insert(headers, "X-Custom", strfmt_direct("Hello, world! :)"));

  // This header should be skipped, as it's a required auto-gen header
  htable_insert(headers, "Content-Type", strfmt_direct("128"));

  cws_response_send(client, STATUS_OK, headers, "Thank you for your request! :)");
  cws_print_prefix(client) ;
  printf("Responded!\n");

  // Close the connection
  close(client->descriptor);
  cws_print_prefix(client) ;
  printf("Connection closed!\n");

  // Print memory status
  mman_print_info();
}

void cws_handle_client(cws_client_t *client)
{
  pthread_create(client->thread, NULL, (void *) cws_serve_client, mman_ref(client));
}