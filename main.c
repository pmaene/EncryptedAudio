#include "protocol.h"

#include "receiver.h"
#include "sender.h"

int main() {
    short i = 0;

    printf("- Key Exchange\n\n");
    printf("-- sender_senderHello\n");

    field_t senderKeyExchangePacket[ENC_KEY_PACKET_SIZE];
    sender_senderHello(senderKeyExchangePacket);

    printf("--- senderKeyExchangePacket\n");
    for (i = 0; i < ENC_KEY_PACKET_SIZE; i++)
        printf("%x", senderKeyExchangePacket[i]);

    printf("\n\n");

    exit(EXIT_SUCCESS);
}
