#include "cws/cws_client.h"

cws_client_t *cws_alloc_client()
{
  static size_t addr_size = sizeof(struct sockaddr_in);

  scptr cws_client_t *client = (cws_client_t *) mman_alloc(sizeof(cws_client_t), cws_free_client);
  client->address_size = (socklen_t *) &addr_size;
  client->address = (struct sockaddr_in *) mman_alloc(addr_size, NULL);
  client->thread = (pthread_t *) mman_alloc(sizeof(pthread_t *), NULL);

  return mman_ref(client);
}

void cws_free_client(void *ref)
{
  cws_client_t *client = (cws_client_t *) ref;
  mman_dealloc_direct(client->address);
  mman_dealloc_direct(client->thread);
}