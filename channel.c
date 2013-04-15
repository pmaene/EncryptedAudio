#include "channel.h"

void _printChannel();

field_t *channel;

void channel_construct() {
    srand(time(NULL));
    channel = calloc(ENC_CHANNEL_CHARS, sizeof(field_t));
}

void channel_write(field_t *data, size_t length) {
    unsigned short i;

    for (i = 0; i < ENC_CHANNEL_CHARS; i++)
        channel[i] = 0;

    if ((rand() % 100) > ENC_DROP_RATE) {
        for (i = 0; i < length; i++)
            channel[i] = data[i];
    }
}

void channel_read(field_t *data, size_t length) {
    unsigned short i;

    for (i = 0; i < length; i++)
        data[i] = channel[i];
}

void channel_destruct() {
    free(channel);
}

void _printChannel() {
    #ifndef __ENC_NO_PRINTS__
        unsigned short i;

        for (i = 0; i < ENC_CHANNEL_CHARS; i++)
            printf("%x", channel[i]);

        printf("\n");
    #endif
}
