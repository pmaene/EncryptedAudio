#ifndef __ENC_CRT_H__
#define __ENC_CRT_H__

#include <stdio.h>

#include "bigdigits.h"
#include "types.h"

void crtModExp(digit_t *result, digit_t *x, digit_t *e, digit_t *p, digit_t *q, size_t nbMessageDigits, size_t nbExponentDigits, size_t nbPrimeDigits);

#endif
