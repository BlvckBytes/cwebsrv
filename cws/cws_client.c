#include "cws/cws_client.h"

/**
 * @brief Clean up a cws_client struct that is about to be destroyed
 */
static void cws_client_cleanup(mman_meta_t *ref)
{
  mman_dealloc(((cws_client_t *) ref->ptr)->address);
  mman_dealloc(((cws_client_t *) ref->ptr)->thread);
}

cws_client_t *cws_client_make()
{
  static size_t addr_size = sizeof(struct sockaddr_in);

  scptr cws_client_t *client = (cws_client_t *) mman_alloc(sizeof(cws_client_t), 1, cws_client_cleanup);
  client->address_size = (socklen_t *) &addr_size;
  client->address = (struct sockaddr_in *) mman_alloc(addr_size, 1, NULL);
  client->thread = (pthread_t *) mman_alloc(sizeof(pthread_t *), 1, NULL);

  return mman_ref(client);
}