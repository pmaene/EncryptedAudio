#ifndef __ENC_PROTOCOL_H__
#define __ENC_PROTOCOL_H__

void senderHello(uint8_t *sendPacket);
void receiverHello(uint8_t *sendPacket, uint8_t *receivedPacket);
void senderAcknowledge(uint8_t *sendPacket, uint8_t *receivedPacket);

void sendData(uint8_t *sendPacket);

#endif
