#include "buffer.h"

void _printBuffer();

field_t buffer[ENC_BUFFER_CHARS];
bool modified;

void buffer_construct() {
    memset(buffer, 0, ENC_BUFFER_CHARS*sizeof(field_t));
    modified = false;
}

void buffer_write(field_t *data, size_t length) {
    if (!modified) {
        memcpy(buffer, data, length);
        modified = true;

        #ifndef __ENC_NO_BUFFER_PRINTS__
            _printBuffer();
        #endif
    }
}

void buffer_read(field_t *data, size_t length) {
    memcpy(data, buffer, length);
    modified = false;
}

bool buffer_isModified() {
    return modified;
}

#ifndef __ENC_NO_BUFFER_PRINTS__
    void _printBuffer() {
        unsigned short i;

        printf("\n# Buffer\n");
        printf("--------\n\n");

        for (i = 0; i < ENC_BUFFER_CHARS; i++)
            printf("%x", buffer[i]);

        printf("\n\n");
    }
#endif
