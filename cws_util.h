#ifndef cws_util_h
#define cws_util_h

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>

/**
 * @brief Prints the address in format <ip>:<port> while using octet-notation for the <ip>
 */
void cws_print_addr_in(struct sockaddr_in addr);

#endif