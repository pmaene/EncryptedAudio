#ifndef __ENC_SENDER_H__
#define __ENC_SENDER_H__

#include "protocol.h"

void sender_construct();

void sender_senderHello();
int sender_senderAcknowledge();
void sender_deriveKey();

void sender_destruct();

void sender_checkEncryption();

int sender_sendData();

#endif
