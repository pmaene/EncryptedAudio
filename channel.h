#ifndef __ENC_CHANNEL_H__
#define __ENC_CHANNEL_H__

#include <stdio.h>
#include <stdlib.h>

#include "bigdigits.h"
#include "types.h"

// Channel Size
#define ENC_CHANNEL_CHARS 1024

void channel_construct();
void channel_write(field_t *data, size_t length);
void channel_read(field_t *data, size_t length);

#endif
