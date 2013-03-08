#ifndef __ENC_SENDER_H__
#define __ENC_SENDER_H__

#include "protocol.h"

void sender_construct();
void sender_destruct();

void sender_senderHello(field_t *sendPacket);

#endif
