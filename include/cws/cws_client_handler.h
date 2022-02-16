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

// Size of one HTTP message segment
// WARNING: This needs to fit a full head in order be able to
// parse Content-Length in the first packet!
#define CWS_HANDLER_SEGLEN 1024

/**
 * @brief Start handling an individual client in it's own thread
 */
void cws_handle_client(cws_client_t *client);

#endif