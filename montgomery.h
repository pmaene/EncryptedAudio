#ifndef __ENC_MONTGOMERY_H__
#define __ENC_MONTGOMERY_H__

#include <stdio.h>

#include "bigdigits.h"
#include "types.h"

void crtModExp(digit_t *result, digit_t *x, digit_t *e, digit_t *p, digit_t *q, size_t nbPrimeDigits);

void montModExp(digit_t *result, digit_t *x, digit_t *e, digit_t *m, size_t nbDigits);
void montMultiply(unsigned char *m, unsigned char *a, unsigned char *b, unsigned char *n, unsigned char nPrime, unsigned short size);

#endif
