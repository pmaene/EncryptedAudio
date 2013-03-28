#include "buffer.h"

void _printBuffer();

field_t *buffer;

void buffer_construct() {
    buffer = calloc(ENC_BUFFER_CHARS, sizeof(field_t));
}

void buffer_write(field_t *data, size_t length) {
    unsigned short i;

    for (i = 0; i < length; i++)
        buffer[i] = data[i];

    printf("* buffer\n");
    _printBuffer();
}

void buffer_read(field_t *data, size_t length) {
    unsigned int i;

    for (i = 0; i < length; i++)
        data[i] = buffer[i];
}

void buffer_destruct() {
    free(buffer);
}

void _printBuffer() {
    unsigned short i;

    for (i = 0; i < ENC_BUFFER_CHARS; i++)
        printf("%x", buffer[i]);

    printf("\n");
}
