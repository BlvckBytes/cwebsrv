#ifndef cws_common_h
#define cws_common_h

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "util/mman.h"
#include "util/strfmt.h"
#include "datastruct/htable.h"

/**
 * @brief Print all key/value pairs within a hash table
 * 
 * @param table Table to print
 * @param val_is_arr True if the value is a string array, false if it's a string scalar
 */
void cws_print_htable_keys(htable_t *table, bool val_is_arr);

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

#endif