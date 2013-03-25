#ifndef __ENC_PROTOCOL_H__
#define __ENC_PROTOCOL_H__

#include <stdio.h>

#include "bigdigits.h"
#include "crypto.h"
#include "random.h"
#include "types.h"

// Data Sizes
#define ENC_DATA_SIZE_CHARS     128
#define ENC_DATA_SIZE_DIGITS    32

// Packet Sizes
#define ENC_KEY_PACKET_CHARS        314
#define ENC_DATA_PACKET_CHARS       ENC_DATA_SIZE_CHARS + ENC_HMAC_CHARS + 5

// Diffie-Hellman Size
#define ENC_DH_SECRET_CHARS         20
#define ENC_DH_SECRET_DIGITS        5

// Accept and Reject Reasons
#define ENC_ACCEPT_PACKET           0
#define ENC_REJECT_PACKET_TAG       1
#define ENC_REJECT_PACKET_SIGNATURE 2

//Counter Wraparound
#define ENC_COUNTER_WRAPAROUND	    1
#define ENC_COUNTER_NO_WRAPAROUND	0

void senderHello(field_t *sendPacket, digit_t *senderSecret);
int receiverHello(field_t *sendPacket, field_t *receivedPacket, digit_t *receiverSecret, digit_t *senderModExp, unsigned char *receiverPrivateExp);
int senderAcknowledge(field_t *sendPacket, field_t *receivedPacket, digit_t *senderSecret, digit_t *receiverModExp, unsigned char *senderPrivateExp);

void sendData(field_t *sendPacket);

int increaseCounter(uint32_t *counter);

#endif
