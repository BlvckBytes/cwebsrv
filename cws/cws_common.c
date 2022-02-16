#include "cws/cws_common.h"

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

bool rp_exit(bool exit, char **error_msg, const char *error_fmt, ...)
{
  if (!exit | !error_msg) return false;

  va_list ap;
  va_start(ap, error_fmt);

  // Format error string using specified format
  scptr char *buf = mman_alloc(sizeof(char), 128, NULL);
  bool res = vstrfmt(&buf, NULL, error_fmt, ap);

  // On success, store output pointer
  if (res) *error_msg = mman_ref(buf);

  va_end(ap);
  return res;
}
