#ifndef __ENC_CRT_H__
#define __ENC_CRT_H__

#include <stdio.h>

#include "bigdigits.h"
#include "crypto.h"
#include "types.h"

void crtModExp(digit_t *restrict result, digit_t *restrict x, digit_t *restrict e, digit_t *restrict p, digit_t *restrict q);

#endif
