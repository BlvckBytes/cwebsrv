#include "cws_util.h"

void cws_print_addr_in(struct sockaddr_in addr)
{
  // Print octet by octet using bitmasking
  for (uint8_t i = 0; i < 4; i++)
    printf(
      "%" PRIu8 "%s",
      (uint8_t) ((addr.sin_addr.s_addr >> (8 * i)) & 0xFF),
      i != 3 ? "." : ""
    );

  // Print port after reversing endianness
  printf(":%" PRIu16, htons(addr.sin_port));
}