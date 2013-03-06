#include "protocol.h"

#include "receiver.h"
#include "sender.h"

int main() {
    word_t senderKeyExchangePacket[ENC_KEY_PACKET_SIZE];
    sender_senderHello(senderKeyExchangePacket);
    printf("%x\n", senderKeyExchangePacket[0]);

    exit(EXIT_SUCCESS);
}
