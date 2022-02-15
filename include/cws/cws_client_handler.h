#ifndef cws_client_handler_h
#define cws_client_handler_h

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "cws/cws_client.h"
#include "cws/cws_common.h"
#include "cws/cws_request.h"
#include "util/mman.h"

/**
 * @brief Start handling an individual client in it's own thread
 */
void cws_handle_client(cws_client_t *client);

#endif