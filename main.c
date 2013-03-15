#include <stdlib.h>

#include "protocol.h"

#include "sender.h"
#include "receiver.h"

void _handshake();

int main() {
    // Construct
    sender_construct();
    receiver_construct();

    // Handshake
    _handshake();

	// Derivation of the AES- and hashKey
	sender_deriveKey();
    printf("\n");
	receiver_deriveKey();
    printf("\n");

    sender_hmacTest();

    // Destruct
    sender_destruct();
    receiver_destruct();

    exit(EXIT_SUCCESS);
}

void _handshake() {
    unsigned short i;
    unsigned short j;

    printf("# Key Exchange\n\n");

    // SenderHello
    field_t senderKeyExchangePacket[ENC_KEY_PACKET_CHARS];
    for (j = 0; j < ENC_KEY_PACKET_CHARS; j++)
        senderKeyExchangePacket[j] = 0;
    sender_senderHello(senderKeyExchangePacket);

    printf("-| senderKeyExchangePacket\n");
    for (i = 0; i < ENC_KEY_PACKET_CHARS; i++)
        printf("%x", senderKeyExchangePacket[i]);

    printf("\n\n");

    // ReceiverHello
    field_t receiverKeyExchangePacket[ENC_KEY_PACKET_CHARS];
    for (j = 0; j < ENC_KEY_PACKET_CHARS; j++)
        receiverKeyExchangePacket[j] = 0;
    receiver_receiverHello(receiverKeyExchangePacket, senderKeyExchangePacket);

    printf("-| receiverKeyExchangePacket\n");
    for (i = 0; i < ENC_KEY_PACKET_CHARS; i++)
        printf("%x", receiverKeyExchangePacket[i]);

    printf("\n\n");

    // SenderAcknowledge
    field_t senderAcknowledgePacket[ENC_KEY_PACKET_CHARS];
    for (j = 0; j < ENC_KEY_PACKET_CHARS; j++)
        senderAcknowledgePacket[j] = 0;
    sender_senderAcknowledge(senderAcknowledgePacket, receiverKeyExchangePacket);

    printf("-| senderAcknowledgePacket\n");
    for (i = 0; i < ENC_KEY_PACKET_CHARS; i++)
        printf("%x", senderAcknowledgePacket[i]);

    printf("\n\n");
}

