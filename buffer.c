#include "buffer.h"

void _printBuffer();

field_t *buffer;
bool *modified;

void buffer_construct() {
    buffer = calloc(ENC_BUFFER_CHARS, sizeof(field_t));
    modified = calloc(1, sizeof(bool));
}

void buffer_write(field_t *data, size_t length) {
    unsigned short i;

    if (!modified) {
        for (i = 0; i < length; i++)
            buffer[i] = data[i];
    }

    *modified = true;
}

void buffer_read(field_t *data, size_t length) {
    unsigned int i;

    for (i = 0; i < length; i++)
        data[i] = buffer[i];

    *modified = false;
}

bool buffer_isModified() {
    return modified;
}

void buffer_destruct() {
    free(buffer);
    free(modified);
}

void _printBuffer() {
    unsigned short i;

    for (i = 0; i < ENC_BUFFER_CHARS; i++)
        printf("%x", buffer[i]);

    printf("\n");
}
