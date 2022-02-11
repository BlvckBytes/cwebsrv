#include "cws_client_handler.h"

static void cws_print_prefix(cws_client_t *client)
{
  printf("[");
  cws_print_addr_in(*(client->address));
  printf("]: ");
}

static void cws_serve_client(void *arg)
{
  cws_client_t *client = (cws_client_t *) arg;
  char message[2048];
  size_t read_size;

  cws_print_prefix(client);
  printf("Now serving requests in another thread!\n");

  while ((read_size = recv(client->descriptor, message, sizeof(message), 0)) > 0)
  {
    cws_print_prefix(client);
    printf("(INCOMING) %s\n", message);

    // Reset buffer
    memset(message, 0, sizeof(message));
  }

  cws_print_prefix(client);

  if (read_size < 0)
    printf("Could not read message (%d)!\n", errno);
  else
    printf("Disconnected!\n");

  cws_print_prefix(client);
  close(client->descriptor);
  cws_free_client(client);
  printf("Resources freed!\n");
}

void cws_handle_client(cws_client_t *client)
{
  pthread_create(client->thread, NULL, (void *) cws_serve_client, client);
}