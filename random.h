#ifndef __ENC_RANDOM_H__
#define __ENC_RANDOM_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "bigdigits.h"
#include "types.h"

void getRandomDigit(digit_t *randomDigit, size_t nbDigits);

#endif
