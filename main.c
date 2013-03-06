#include "protocol.h"

#include "receiver.h"
#include "sender.h"

int main() {
    word_t senderKeyExchangePacket[ENC_KEY_PACKET_SIZE];
    sender_senderHello(senderKeyExchangePacket);

    exit(EXIT_SUCCESS);
}
