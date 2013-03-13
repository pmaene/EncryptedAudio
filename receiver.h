#ifndef __ENC_RECEIVER_H__
#define __ENC_RECEIVER_H__

#include "protocol.h"

void receiver_construct();
void receiver_destruct();

int receiver_receiverHello(field_t *sendPacket, field_t *receivedPacket);
void receiver_deriveKey();

#endif
