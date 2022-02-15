#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

#include "cws/cws_client.h"
#include "cws/cws_client_handler.h"
#include "util/mman.h"

int main(void)
{
  int srv_desc = socket(
    AF_INET, // DOMAIN
    SOCK_STREAM, // TYPE
    0 // Protocol (0 = use default)
  );

  if (srv_desc < 0)
  {
    printf("Could not create server socket (%d)!\n", errno);
    return 1;
  }

  // Listen on any internet address on 8192
  struct sockaddr_in server_addr;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8192);

  // Socket should be reusable
  bool reuse_addr = true;
  setsockopt(srv_desc, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int));

  int bind_ret = bind(srv_desc, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if (bind_ret < 0)
  {
    printf("Could not bind the server socket (%d)!\n", errno);
    return 1;
  }

  int listen_ret = listen(srv_desc, 5);
  if (listen_ret < 0)
  {
    printf("Could not begin listening for clients (%d)!\n", errno);
    return 1;
  }

  printf("Listening for requests on ");
  cws_print_addr_in(server_addr);
  printf("!\n");

  while (1)
  {
    scptr cws_client_t *client = cws_client_make();
    client->descriptor = accept(srv_desc, (struct sockaddr *) client->address, client->address_size);

    if (client->descriptor < 0)
    {
      printf("Could not accept incoming request from ");
      cws_print_addr_in(*client->address);
      printf(" (%d)!\n", errno);
      continue;
    }

    cws_handle_client(client);
  }

  return 0;
}