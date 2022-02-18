#include "cws/cws_socket.h"

cws_socket_t *cws_socket_create(in_addr_t addr, int port)
{
  // Build address information based on parameters
  struct sockaddr_in server_addr;
  server_addr.sin_addr.s_addr = addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  // Create basic internet socket
  int srv_desc = socket(
    AF_INET,       // DOMAIN
    SOCK_STREAM,   // TYPE
    0              // PROTOCOL (0 = use default)
  );

  // Could not create socket
  if (srv_desc < 0) return NULL;

  // Socket address should be reusable
  bool reuse_addr = true;
  setsockopt(srv_desc, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int));

  // Bind to local endpoint
  int bind_ret = bind(srv_desc, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if (bind_ret < 0) return NULL;

  // Create structure containing both elements
  scptr cws_socket_t *sock = (cws_socket_t *) mman_alloc(sizeof(cws_socket_t), 1, NULL);
  sock->addr = server_addr;
  sock->descriptor = srv_desc;

  // Could not allocate socket container
  if (!sock)
  {
    errno = ENOMEM;
    return NULL;
  }

  return mman_ref(sock);
}

/**
 * @brief Socket accept loop subroutine to be used as a thread function
 * 
 * @param arg Socket structure pointer
 */
static void *cws_socket_loop(void *arg)
{
  cws_socket_t *sock = (cws_socket_t *) arg;
  sock->thread_active = true;

  // Client request serve loop
  while (sock->thread_active)
  {
    scptr cws_client_t *client = cws_client_make();
    client->descriptor = accept(sock->descriptor, (struct sockaddr *) client->address, client->address_size);

    // Issue while establishing the connection
    if (client->descriptor < 0)
    {
      printf("Could not accept incoming request from ");
      cws_print_addr_in(*client->address);
      printf(" (%d)!\n", errno);
      continue;
    }

    // Pass serving onto the handler
    sock->handler(client);
  }

  return NULL;
}

bool cws_socket_listen(cws_socket_t *socket, int backlog, cws_client_handler_t handler)
{
  // Start listening
  socket->handler = handler;
  if (listen(socket->descriptor, backlog) < 0) return false;

  // Start accepting in another thread
  int err = pthread_create(&socket->thread, NULL, cws_socket_loop, socket);

  // Could not create thread
  if (err)
  {
    errno = err;
    return false;
  }

  // Success
  return true;
}