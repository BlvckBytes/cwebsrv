#ifndef str2int_h
#define str2int_h

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno_t;

str2int_errno_t str2int(int *out, char *s, int base);

#endif