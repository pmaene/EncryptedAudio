#ifndef __ENC_PROTOCOL_H__
#define __ENC_PROTOCOL_H__

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#include "bigdigits.h"

typedef DIGIT_T digit_t;
typedef unsigned char field_t;

// Key Sizes
#define ENC_PRIVATE_KEY_CHARS       156
#define ENC_PRIVATE_KEY_DIGITS      39
#define ENC_PUBLIC_KEY_CHARS        3
#define ENC_PUBLIC_KEY_DIGITS       1

// Packet Sizes
#define ENC_KEY_PACKET_CHARS        314
#define ENC_DATA_PACKET_CHARS       1500

// Diffie-Hellman Size
#define ENC_DH_SECRET_CHARS         128
#define ENC_DH_SECRET_DIGITS        32

// Hash Sizes
#define ENC_HASH_CHARS              20
#define ENC_HASH_DIGITS             5

// Signature Sizes
#define ENC_SIGNATURE_CHARS         157
#define ENC_SIGNATURE_DIGITS        40

// Accept and Reject Reasons
#define ENC_ACCEPT_PACKET           0
#define ENC_REJECT_PACKET_TAG       1
#define ENC_REJECT_PACKET_SIGNATURE 2

#define ENC_SIGNATURE_ACCEPTED      1
#define ENC_SIGNATURE_REJECTED      0

void senderHello(field_t *sendPacket, digit_t *senderSecret);
int receiverHello(field_t *sendPacket, field_t *receivedPacket, digit_t *receiverSecret, digit_t *senderModExp, unsigned char *receiverPrivateExp);
int senderAcknowledge(field_t *sendPacket, field_t *receivedPacket, digit_t *senderSecret, digit_t *receiverModExp, unsigned char *senderPrivateExp);

void sendData(field_t *sendPacket);

#endif
