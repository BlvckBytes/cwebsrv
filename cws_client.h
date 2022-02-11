#ifndef cws_client_h
#define cws_client_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * @brief Encapsulates a client socket with all it's dependencies
 */
typedef struct
{
  struct sockaddr_in *address;
  socklen_t *address_size;
  int descriptor;
  pthread_t *thread;
} cws_client_t;

/**
 * @brief Allocate a new non-initialized client struct
 */
cws_client_t *cws_alloc_client();

/**
 * @brief Free a no longer needed client struct
 */
void cws_free_client(cws_client_t *client);

#endif