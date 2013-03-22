#include "channel.h"

void _printChannel();

field_t *channel;

void channel_construct() {
    channel = calloc(ENC_CHANNEL_CHARS, sizeof(field_t));
}

void channel_write(field_t *data, size_t length) {
    unsigned short i;

    srand(time(NULL));
    if ((rand() % 100) > ENC_DROP_RATE) {
        for (i = 0; i < length; i++)
            channel[i] = data[i];
    }

    printf("* channel\n");
    _printChannel();
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
    unsigned short i;

    for (i = 0; i < ENC_CHANNEL_CHARS; i++)
        printf("%x", channel[i]);

    printf("\n");
}
