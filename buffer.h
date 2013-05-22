#ifndef __ENC_BUFFER_H__
#define __ENC_BUFFER_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bigdigits.h"
#include "types.h"

// Channel Parameters
#define ENC_BUFFER_CHARS 8

void buffer_construct();
void buffer_write(field_t *restrict data, size_t length);
void buffer_read(field_t *restrict data, size_t length);

bool buffer_isModified();

#endif
