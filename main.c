#include "protocol.h"

#include "receiver.h"
#include "sender.h"

int main() {
    short i = 0;

    printf("# Key Exchange\n\n");

    field_t senderKeyExchangePacket[ENC_KEY_PACKET_CHARS];
    sender_senderHello(senderKeyExchangePacket);

    printf("-| senderKeyExchangePacket\n");
    for (i = 0; i < ENC_KEY_PACKET_CHARS; i++)
        printf("%x", senderKeyExchangePacket[i]);

    printf("\n\n");

    sender_cleanup();

    exit(EXIT_SUCCESS);
}
