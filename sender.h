#ifndef __ENC_SENDER_H__
#define __ENC_SENDER_H__

#include "protocol.h"

void sender_contruct();
void sender_destruct();

int sender_senderHello(field_t *sendPacket);

#endif
