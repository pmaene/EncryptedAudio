#include "receiver.h"

// RSA
const unsigned char Enc_ReceiverPrivateExp[ENC_PRIVATE_KEY_CHARS] =
    "\x3c\x13\xf0\x04\xd1\x24\xdd\x01\x03\xd2\xb0\x71\x42\xa2\xf6"
    "\x10\xc6\x35\x96\x9c\xd4\x4c\x46\xd9\xaf\x91\xbe\xcc\xad\xc9"
    "\xc5\x51\xdb\x4d\x5b\x81\xd9\x89\x13\xd4\xc4\xb0\x30\x30\xf7"
    "\x00\xe6\xc5\x50\x5d\x10\xa4\x9f\xf4\xec\xc0\x73\xd8\xfd\x36"
    "\x9e\x60\xe1\x42\x4f\x73\xae\xf7\x28\x0d\x15\xcd\x92\xb0\x0d"
    "\x5f\x04\xeb\xf9\xd0\xc0\xe7\x38\xff\x9e\xc3\x2c\xab\x22\xcd"
    "\x4b\xda\xcb\xc7\xbd\x24\xd4\x51\x71\xe5\x99\x01\xcb\xcb\x78"
    "\xc0\xbd\x3f\xc2\xc5\xd9\x02\x46\x68\xa9\xbc\xfd\x11\x3f\xc5"
    "\xdc\xc3\x6a\x55\xad\x3f\x5f\xfb\x08\xad\x01\x5a\xac\x02\xbc"
    "\xa1\xaa\xb7\x88\x01\x2b\xbd\x60\x97\x41\xbf\x5b\x6e\x06\x55"
    "\xf9\x03\xb5\xd1\xd3\xc1";

// Memory Pointers
int *senderModExp;
int *receiverSecret;

void receiver_construct() {
    senderModExp = (int *) calloc(ENC_PRIVATE_KEY_CHARS, sizeof(field_t));
    receiverSecret = (int *) calloc(ENC_DH_SECRET_DIGITS, sizeof(field_t));
}

int receiver_receiverHello(field_t *sendPacket, field_t *receivedPacket) {
    unsigned char i = 0;

    printf("--> receiver_receiverHello\n");

    if (0x00 != receivedPacket[0])
        return ENC_REJECT_PACKET_TAG;

    printf("--| senderModExp\n");
    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++) {
        senderModExp[i] = receivedPacket[i+1];
        printf("%x", senderModExp[i]);
    }
    printf("\n");

    receiverHello(sendPacket, senderModExp, receiverSecret, Enc_ReceiverPrivateExp);
}

void receiver_destruct() {
	free(senderModExp);
    free(receiverSecret);
}
