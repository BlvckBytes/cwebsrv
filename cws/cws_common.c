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

void cws_print_htable_keys(htable_t *table, bool val_is_arr)
{
  // Fetch populated key slots from table
  scptr char **header_keys = NULL;
  htable_list_keys(table, &header_keys);

  // No entries
  if (*header_keys == 0)
    printf("No entries!\n");

  // Print all k/v pairs
  for (char **header_key = header_keys; *header_key; header_key++)
  {
    void *header_value = NULL;
    htable_fetch(table, *header_key, &header_value);

    // Value is an array, print all entries
    if (val_is_arr)
    {
      printf("\"%s\" -> ", *header_key);
      for (void **header_val = header_value; *header_val; header_val++)
        printf("%s\"%s\"", header_val == header_value ? "" : ", ", (char *) *header_val);
      printf("\n");
    }

    // Value is scalar, print at once
    else
      printf("\"%s\" -> \"%s\"\n", *header_key, (char *) header_value);
  }
}

bool rp_exit(bool exit, char **error_msg, const char *error_fmt, ...)
{
  if (!exit | !error_msg) return false;

  va_list ap;
  va_start(ap, error_fmt);

  // Format error string using specified format
  bool res = strfmt(error_msg, error_fmt, ap);

  va_end(ap);
  return res;
}
