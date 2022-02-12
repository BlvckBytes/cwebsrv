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

void cws_print_htable_keys(htable_t *table, bool val_is_arr)
{
  // Fetch populated key slots from table
  char **header_keys = NULL;
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

  // Free key buffer
  free(header_keys);
}

char *cws_strdup_until(char *str, size_t *offs, char sep, bool skip)
{
  // No input provided
  if (!str) return NULL;

  size_t str_len = strlen(str);
  for (size_t i = *offs; i < str_len; i++)
  {
    // Last character, navigate onto NULL
    if (i == str_len - 1) i++;

    // Wait until the separator has been encountered
    // Newline can substitute for the separator
    else if (!(str[i] == sep || str[i] == '\n' || str[i] == '\r')) continue;

    // Create substring if not in skip mode
    char *res = NULL;
    if (!skip)
    {
      // Copy target range into buffer and terminate
      res = (char *) malloc(i - *offs + 1);
      strncpy(res, &str[*offs], i - *offs);
      res[i - *offs] = 0;
    }

    // Skip separator for next call
    *offs = i + 1;
    return res;
  }

  return NULL;
}
