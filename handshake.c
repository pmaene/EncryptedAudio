#include "handshake.h";

void _handshake() {
    #ifndef __ENC_NO_PRINTS__
        printf("\n# Key Exchange\n");
        printf("--------------\n\n");
    #endif

    switch (handshakeState) {
        case SENDER_HELLO:
            sender_senderHello();
            handshakeState = RECEIVER_HELLO;
            break;
        case RECEIVER_HELLO:
            if (ENC_ACCEPT_PACKET == receiver_receiverHello())
                handshakeState = SENDER_ACKNOWLEDGE;
            break;
        case SENDER_ACKNOWLEDGE:
            if (ENC_ACCEPT_PACKET == sender_senderAcknowledge())
                handshakeState = RECEIVER_CHECK_ACKNOWLEDGE;
            break;
        case RECEIVER_CHECK_ACKNOWLEDGE:
            if (ENC_ACCEPT_PACKET == receiver_checkSenderAcknowledge())
                handshakeState = HANDSHAKE_FINISHED;
            break;
        case HANDSHAKE_FINISHED:
            break;
    }
}