#ifndef cws_util_h
#define cws_util_h

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>

#include "mman.h"
#include "htable.h"

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
 * @brief Create a duplicate from a string at an offset until either a
 * specified separator is reached or the string ended
 * 
 * @param str Input string
 * @param offs Offset reference, gets modified in place (for chaining)
 * @param sep Separator string
 * @param skip Skip mode - if true, NULL is returned (for efficiency)
 * @return char* Allocated substring
 */
char *cws_strdup_until(char *str, size_t *offs, const char* sep, bool skip);

#endif