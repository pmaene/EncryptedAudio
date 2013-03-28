#ifndef __ENC_RECEIVER_H__
#define __ENC_RECEIVER_H__

#include "protocol.h"

void receiver_construct();

int receiver_receiverHello();
void receiver_deriveKey();

void receiver_destruct();

int receiver_receiveData();

#endif
