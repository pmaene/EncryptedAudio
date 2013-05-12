#ifndef __ENC_SENDER_H__
#define __ENC_SENDER_H__

#include <string.h>

#include "buffer.h"
#include "channel.h"
#include "protocol.h"

void sender_construct();

void sender_senderHello();
int sender_senderAcknowledge();
void sender_deriveKey(uint8_t aesKey[], uint8_t CTRNonce[], digit_t *restrict modExp);
int sender_sendData();

#endif
