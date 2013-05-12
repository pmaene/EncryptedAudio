#ifndef __ENC_PROTOCOL_H__
#define __ENC_PROTOCOL_H__

#include <stdio.h>
#include <string.h>

#include "bigdigits.h"
#include "crypto.h"
#include "random.h"
#include "types.h"
#include "sender.h"
#include "receiver.h"

// Data Sizes
#define ENC_DATA_SIZE_CHARS         128
#define ENC_DATA_SIZE_DIGITS        32

// Packet Sizes
#define ENC_KEY_PACKET_CHARS        317
#define ENC_DATA_PACKET_CHARS       ENC_DATA_SIZE_CHARS + ENC_HMAC_CHARS + 5
#define ENC_DATA_PACKET_DIGITS      ENC_DATA_PACKET_CHARS/4

// Diffie-Hellman Size
#define ENC_DH_SECRET_CHARS         20
#define ENC_DH_SECRET_DIGITS        5

// Accept and Reject Reasons
#define ENC_ACCEPT_PACKET           0
#define ENC_REJECT_PACKET_TAG       1
#define ENC_REJECT_PACKET_SIGNATURE 2
#define ENC_COUNTER_WRAPAROUND	    3
#define ENC_LOST_PACKET             4
#define ENC_HMAC_ACCEPTED           5
#define ENC_HMAC_REJECTED           6
#define ENC_INVALID_ACK             7

void senderHello(field_t *restrict sendPacket, digit_t *restrict senderModExp, digit_t *restrict senderSecret);
int receiverHello(field_t *restrict sendPacket, digit_t *restrict receiverModExp, field_t *restrict receivedPacket, digit_t *restrict receiverSecret, digit_t *restrict senderModExp, unsigned char *restrict receiverPrivateExp);
int senderAcknowledge(field_t *restrict SsendPacket, field_t *restrict receivedPacket, digit_t *restrict senderSecret, digit_t *restrict receiverModExp, digit_t *restrict senderModExp, unsigned char *restrict senderPrivateExp);

void sendData(field_t *sendPacket);

int increaseCounter(uint32_t *counter);

#endif
