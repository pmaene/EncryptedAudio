#ifndef __ENC_PROTOCOL_H__
#define __ENC_PROTOCOL_H__

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#include "bigdigits.h"

typedef DIGIT_T digit_t;
typedef uint8_t field_t;

// Key sizes
#define ENC_KEY_BITS          1248
#define ENC_KEY_CHARS         156
#define ENC_KEY_DIGITS        39

// Packet Sizes
#define ENC_KEY_PACKET_CHARS  177
#define ENC_DATA_PACKET_CHARS 1500

// Diffie-Hellman Secrets Size
#define ENC_DH_SECRET_DIGITS  32

#define NO_ALLOCS

void senderHello(field_t *sendPacket, int *senderSecret);
void receiverHello(field_t *sendPacket, field_t *receivedPacket);
void senderAcknowledge(field_t *sendPacket, field_t *receivedPacket);

void sendData(field_t *sendPacket);

#endif
