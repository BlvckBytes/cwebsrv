#ifndef cws_common_h
#define cws_common_h

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "util/mman.h"
#include "util/strfmt.h"
#include "cws/cws_response_code.h"
#include "cws/cws_response.h"

// Timeout between segments for discarding requests
#define CWS_REQ_SEG_TIMEOUT 20

/**
 * @brief Prints the address in format <ip>:<port> while using octet-notation for the <ip>
 */
void cws_print_addr_in(struct sockaddr_in addr);

/**
 * @brief Request parser exit routine, returns true if NULL should be
 * returned by the parser
 * 
 * @param exit Exiting condition
 * @param error_msg Error message output reference
 * @param error_fmt Error string format on exiting
 * @param ... Format parameters
 */
bool rp_exit(bool exit, char **error_msg, const char *error_fmt, ...);

/**
 * @brief Discard a client's request by reading until EOS
 */
void cws_discard_request(cws_client_t *client);

/**
 * @brief Error out with an error response to the client if an error condition applies
 * 
 * @param client Error recipient
 * @param error_cond Error condition
 * @param code Response code for this error
 * @param message Message for this error
 * 
 * @return true Error occurred, message sent
 * @return false Error didn't occur
 */
bool errif_resp(cws_client_t *client, bool error_cond, cws_response_code_t code, const char *message);

#endif