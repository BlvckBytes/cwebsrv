#ifndef cws_client_h
#define cws_client_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>

#include "util/mman.h"

/**
 * @brief Encapsulates a client socket with all it's dependencies
 */
typedef struct cws_client
{
  struct sockaddr_in *address;
  socklen_t *address_size;
  int descriptor;
  pthread_t *thread;
} cws_client_t;

/**
 * @brief Creates a new non-initialized client struct
 */
cws_client_t *cws_client_make();

#endif