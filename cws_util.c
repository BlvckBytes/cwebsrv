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
  mman char **header_keys = NULL;
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

static bool cws_is_at_index(char *str, const char *search, size_t offs)
{
  for (size_t i = 0; i < strlen(search); i++)
    if (str[i + offs] != search[i]) return false;
  return true;
}

char *cws_strdup_until(char *str, size_t *offs, const char* sep, bool skip)
{
  // No input provided
  if (!str) return NULL;

  size_t str_len = strlen(str), prev_offs = *offs;
  for (size_t i = *offs; i < str_len; i++)
  {
    // Last character, navigate onto NULL
    if (i == str_len - 1) i++;

    // Wait until the separator has been encountered
    else if (!cws_is_at_index(str, sep, i)) continue;

    // Skip separator for next call
    *offs = i + strlen(sep);

    // Don't create the substring
    if (skip) return NULL;

    size_t res_len = i - prev_offs;

    // Eat up carriage return that preceds the targetted newline
    if (str[i - 1] == '\r' && sep[strlen(sep) - 1] == '\n')
      res_len--;

    // Allocate a copy
    mman char *res = mman_alloc(res_len + 1, NULL);

    // Copy over string content and terminate
    for (size_t j = 0; j < res_len; j++)
      res[j] = str[j + prev_offs];
    res[res_len] = 0;

    return mman_ref(res);
  }

  return NULL;
}
