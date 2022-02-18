#ifndef cws_socket_h
#define cws_socket_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

#include "cws/cws_client.h"
#include "cws/cws_common.h"
#include "util/mman.h"

/**
 * @brief A handler for incoming clients
 */
typedef void (*cws_client_handler_t)(cws_client_t *);

/**
 * @brief A server socket with it's address and file descriptor
 */
typedef struct cws_socket
{
  struct sockaddr_in addr;        // Address the socket is bound to
  int descriptor;                 // File descriptor of the socket
  pthread_t thread;               // Thread of the accept loop
  bool thread_active;             // Whether or not the loop thread is active
  cws_client_handler_t handler;   // Client handler function
} cws_socket_t;

/**
 * @brief Create a new server socket in order to handle client requests
 * 
 * @param addr Address to listen on
 * @param port Port to listen on
 * 
 * @return cws_socket_t* Instance of socket struct or NULL on errors
 */
cws_socket_t *cws_socket_create(in_addr_t addr, int port);

/**
 * @brief Start listening for client requests
 * 
 * @param socket Previously created socket handle
 * @param backlog Size of connection request queue
 * @param handler Client request handler function
 * 
 * @return true Successful went into listening mode
 * @return false Could not start listening
 */
bool cws_socket_listen(cws_socket_t *socket, int backlog, cws_client_handler_t handler);

#endif