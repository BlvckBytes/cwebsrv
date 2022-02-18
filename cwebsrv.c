#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

#include "cws/cws_socket.h"
#include "cws/cws_client_handler.h"
#include "util/mman.h"

int main(void)
{
  // Try to create a server socket
  scptr cws_socket_t *sock = cws_socket_create(INADDR_ANY, 8192);
  if (!sock)
  {
    fprintf(stderr, "Could not create server socket (%d)!\n", errno);
    return 1;
  }

  // Listen for requests
  if (!cws_socket_listen(sock, 16, cws_handle_client))
  {
    fprintf(stderr, "Could not go into listen mode (%d)!\n", errno);
    return 1;
  }

  // Inform about endpoint
  printf("Listening for requests on ");
  cws_print_addr_in(sock->addr);
  printf("!\n");

  // Join listener thread
  int err = pthread_join(sock->thread, NULL);
  if (err)
  {
    fprintf(stderr, "Could not join listener thread (%d)!\n", errno);
    return 1;
  }

  // Listener thread has been exited
  printf("Done! Exiting...\n");
  return 0;
}