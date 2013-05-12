#ifndef __ENC_RECEIVER_H__
#define __ENC_RECEIVER_H__

#include <string.h>
#include <stdbool.h>

#include "buffer.h"
#include "channel.h"
#include "protocol.h"


void receiver_construct();

int receiver_receiverHello();
void receiver_deriveKey(uint8_t *restrict aesKey, uint8_t *restrict CTRNonce, digit_t *restrict modExp);
int receiver_receiveData();
int receiver_checkSenderAcknowledge();

#endif
