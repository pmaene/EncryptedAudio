#include "protocol.h"

#include "receiver.h"
#include "sender.h"

// Private Declarations
void handshake();

int main() {
    // Handshake
    handshake();

    // Cleanup
    sender_cleanup();
    receiver_cleanup();

    exit(EXIT_SUCCESS);
}

void handshake() {
    unsigned short i = 0;

    printf("# Key Exchange\n\n");

    // SenderHello
    field_t senderKeyExchangePacket[ENC_KEY_PACKET_CHARS];
    sender_senderHello(senderKeyExchangePacket);

    printf("-| senderKeyExchangePacket\n");
    for (i = 0; i < ENC_KEY_PACKET_CHARS; i++)
        printf("%x", senderKeyExchangePacket[i]);

    printf("\n\n");

    // ReceiverHello
    field_t receiverKeyExchangePacket[ENC_KEY_PACKET_CHARS];
    receiver_receiverHello(receiverKeyExchangePacket, senderKeyExchangePacket);

    printf("-| receiverKeyExchangePacket\n");
    for (i = 0; i < ENC_KEY_PACKET_CHARS; i++)
        printf("%x", receiverKeyExchangePacket[i]);

    printf("\n\n");
}