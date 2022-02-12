#ifndef cws_client_handler_h
#define cws_client_handler_h

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "cws_client.h"
#include "cws_util.h"

/**
 * @brief Start handling an individual client in it's own thread
 */
void cws_handle_client(cws_client_t *client);

#endif