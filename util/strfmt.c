#include "util/strfmt.h"

bool strfmt(char **out, const char *fmt, va_list ap)
{
  // Initally, try to fit the message into 128 characters
  scptr char *res = mman_alloc(sizeof(char), 128, NULL);
  mman_meta_t *res_m = mman_fetch_meta(res);

  // Check if space was sufficient
  int req_sz = vsnprintf(res, res_m->num_blocks, fmt, ap);
  if (req_sz >= res_m->num_blocks)
  {
    // Try to allocate more space
    if ((res = mman_realloc(&res, sizeof(char), req_sz)) == NULL)
      // Not enough space left
      return false;

    vsnprintf(res, res_m->num_blocks, fmt, ap);
  }

  *out = mman_ref(res);
  return true;
}