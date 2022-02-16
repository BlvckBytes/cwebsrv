#include "cws/cws_client_handler.h"

static void cws_print_prefix(cws_client_t *client)
{
  printf("[");
  cws_print_addr_in(*(client->address));
  printf("]: ");
}

static void cws_serve_client(void *arg)
{
  scptr cws_client_t *client = (cws_client_t *) arg;

  // TODO: This fixed buffer is no good idea...
  char message[2048];
  size_t read_size;

  cws_print_prefix(client);
  printf("Now serving requests in another thread!\n");

  while ((read_size = recv(client->descriptor, message, sizeof(message), 0)) > 0)
  {
    // Parse and print request
    scptr char *error_msg;
    scptr cws_request_t *req = cws_request_parse(message, &error_msg);

    // Print error message, if exists
    if (error_msg)
    {
      cws_print_prefix(client);
      printf("Error: %s\n", error_msg);
    }

    // Print the parsed request otherwise
    else
    {
      cws_print_prefix(client);
      printf("\n");
      cws_request_print(req);
    }

    // Reset buffer
    memset(message, 0, sizeof(message));
  }

  cws_print_prefix(client);

  if (read_size < 0)
    printf("Could not read message (%d)!\n", errno);
  else
    printf("Disconnected!\n");

  // Close client socket
  close(client->descriptor);

  // WARNING: This is dev-phase only output!
  mman_print_info();
}

void cws_handle_client(cws_client_t *client)
{
  pthread_create(client->thread, NULL, (void *) cws_serve_client, mman_ref(client));
}