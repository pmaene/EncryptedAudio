#include "channel.h"

void _printChannel();

field_t channel[ENC_CHANNEL_CHARS];

void channel_construct() {
    srand(time(NULL));
    memset(channel, 0, ENC_CHANNEL_CHARS*sizeof(field_t));
}

void channel_write(field_t *restrict data, size_t length) {
    memset(channel, 0x00, length);

    if ((rand() % 100) > ENC_DROP_RATE) {
        memcpy(channel, data, length);

        #ifndef __ENC_NO_CHANNEL_PRINTS__
            _printChannel();
        #endif
    }
}

void channel_read(field_t *restrict data, size_t length) {
    memcpy(data, channel, length);
}

#ifndef __ENC_NO_CHANNEL_PRINTS__
    void _printChannel() {
        unsigned short i;

        printf("\n# Channel\n");
        printf("---------\n\n");

        for (i = 0; i < ENC_CHANNEL_CHARS; i++)
            printf("%x", channel[i]);

        printf("\n\n");
    }
#endif
