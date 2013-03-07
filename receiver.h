#ifndef __ENC_RECEIVER_H__
#define __ENC_RECEIVER_H__

#include "protocol.h"

void receiver_receiverHello(field_t *sendPacket, field_t *receivedPacket);

void receiver_cleanup();

#endif
