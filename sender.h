#ifndef __ENC_SENDER_H__
#define __ENC_SENDER_H__

#include <string.h>

#include "buffer.h"
#include "channel.h"
#include "protocol.h"

void sender_construct();

void sender_senderHello();
int sender_senderAcknowledge();
void sender_deriveKey();
int sender_sendData();

#endif
