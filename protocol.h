#ifndef __ENC_PROTOCOL_H__
#define __ENC_PROTOCOL_H__

void senderHello(word_t *sendPacket);
void receiverHello(word_t *sendPacket, word_t *receivedPacket);
void senderAcknowledge(word_t *sendPacket, word_t *receivedPacket);

void sendData(word_t *sendPacket);

#endif
