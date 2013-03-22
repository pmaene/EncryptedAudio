#ifndef __ENC_BUFFER_H__
#define __ENC_BUFFER_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bigdigits.h"
#include "types.h"

// Channel Parameters
#define ENC_BUFFER_CHARS 1024

void buffer_construct();
void buffer_write(field_t *data, size_t length);
void buffer_read(field_t *data, size_t length);
void buffer_destruct();

#endif
