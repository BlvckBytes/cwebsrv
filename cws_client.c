#include "cws_client.h"

cws_client_t *cws_alloc_client()
{
  static size_t addr_size = sizeof(struct sockaddr_in);

  cws_client_t *client = (cws_client_t *) malloc(sizeof(cws_client_t));
  client->address_size = (socklen_t *) &addr_size;
  client->address = (struct sockaddr_in *) malloc(addr_size);
  client->thread = (pthread_t *) malloc(sizeof(pthread_t));

  return client;
}

void cws_free_client(cws_client_t *client)
{
  free(client->address);
  free(client->thread);
  free(client);
}