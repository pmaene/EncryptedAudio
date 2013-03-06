#ifndef __ENC_PROTOCOL_H__
#define __ENC_PROTOCOL_H__

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#include "bigdigits.h"

typedef uint8_t word_t;

#define ENC_KEY_BITS         1248
#define ENC_KEY_CHARS        156
#define ENC_KEY_DIGITS       39


#define ENC_KEY_PACKET_SIZE  277
#define ENC_DATA_PACKET_SIZE 1500

#define NO_ALLOCS

void senderHello(word_t *sendPacket);
void receiverHello(word_t *sendPacket, word_t *receivedPacket);
void senderAcknowledge(word_t *sendPacket, word_t *receivedPacket);

void sendData(word_t *sendPacket);

#endif
