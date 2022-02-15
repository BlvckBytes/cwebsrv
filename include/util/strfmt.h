#ifndef strfmt_h
#define strfmt_h

#include <stdbool.h>
#include <stdarg.h>
#include "util/mman.h"

/**
 * @brief Format a string and allocate it's buffer dynamically
 * 
 * @param out Output pointer buffer
 * @param fmt Format string
 * @param ap Arguments for the format
 * @return true Result has been written to the buffer
 * @return false Couldn't allocate space or the buffer/format were NULL
 */
bool strfmt(char **out, const char *fmt, va_list ap);

#endif