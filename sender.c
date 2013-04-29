#include "sender.h"

// RSA
const unsigned char Enc_SenderPrivateExp[ENC_PRIVATE_KEY_CHARS] =
    "\x07\x8e\x74\x79\x5c\xb9\xa9\xda\x98\xf8\x0e\xf0\xad\xa4\xed"
    "\xa9\xe9\x29\xe7\x93\x37\x40\xf1\x73\xb4\xc4\x46\x6b\x65\x57"
    "\xae\xfd\x53\x84\x34\xf6\x3d\x1d\x83\x78\x76\x12\xe8\xde\xad"
    "\xd5\x69\xa0\xd7\x67\x92\x60\x34\x5b\x1c\x97\x8f\xee\x5c\x3b"
    "\xaf\x61\x05\xb5\x26\xbd\x17\xda\x87\x28\x14\x29\x36\xe6\xa2"
    "\xb3\xc4\xd8\x06\xaf\x49\xc0\x5e\x4b\x8a\xea\xb5\x50\x58\xa1"
    "\x93\x1a\x34\xb7\xb1\x1b\x4f\xf9\x6c\xc0\xed\xad\xdf\xa1\xa0"
    "\xe3\x9e\x87\x28\x65\x3f\xf2\x6f\x40\xa4\x0c\xe6\x13\x63\xd9"
    "\x59\x1f\x7c\x31\x31\x73\xfc\xf7\xda\x68\xc8\x23\xff\xc3\x19"
    "\x4f\x85\x17\x63\xce\xf1\x7a\xe6\xb1\xde\xb0\xb9\x65\xfd\x2a"
    "\x83\xeb\x6d\xf6\x5a\x41";

digit_t senderSecret[ENC_PRIVATE_KEY_DIGITS];
digit_t sender_receiverModExp[ENC_PRIVATE_KEY_DIGITS];

uint8_t senderAESKey[ENC_AES_KEY_CHARS];
uint8_t senderHashKey[ENC_HMAC_KEY_CHARS];
uint8_t senderCTRNonce[ENC_CTR_NONCE_CHARS];

uint32_t senderPacketCounter[1];

void sender_construct() {
    memset(senderSecret, 0, ENC_PRIVATE_KEY_DIGITS*sizeof(digit_t));
    memset(sender_receiverModExp, 0, ENC_PRIVATE_KEY_DIGITS*sizeof(digit_t));

    memset(senderAESKey, 0, ENC_AES_KEY_CHARS);
    memset(senderHashKey, 0, ENC_HMAC_KEY_CHARS);
    memset(senderCTRNonce, 0, ENC_CTR_NONCE_CHARS);

    memset(senderPacketCounter, 0, sizeof(uint32_t));
}

void sender_senderHello() {
    field_t sendPacket[ENC_KEY_PACKET_CHARS];

    #ifndef __ENC_NO_PRINTS__
        printf("--> sender_senderHello\n");
    #endif
    senderHello(sendPacket, senderSecret);

    channel_write(sendPacket, ENC_KEY_PACKET_CHARS);
}

int sender_senderAcknowledge() {
    int returnStatus;

    field_t receivedPacket[ENC_KEY_PACKET_CHARS];
    field_t sendPacket[ENC_KEY_PACKET_CHARS];

    channel_read(receivedPacket, ENC_KEY_PACKET_CHARS);

    #ifndef __ENC_NO_PRINTS__
        printf("--> sender_senderAcknowledge\n");
    #endif

    returnStatus = senderAcknowledge(sendPacket, receivedPacket, senderSecret, sender_receiverModExp, (unsigned char *) Enc_SenderPrivateExp);

    channel_write(sendPacket, ENC_KEY_PACKET_CHARS);

    return returnStatus;
}

void sender_deriveKey(uint8_t *aesKey, uint8_t *CTRNonce, digit_t *modExp) {
	digit_t symmetricKey[ENC_PRIVATE_KEY_DIGITS];

    #ifndef __ENC_NO_PRINTS__
        printf("--> sender_deriveKey\n");
    #endif

    memcpy(sender_receiverModExp, modExp, ENC_PRIVATE_KEY_DIGITS);
	_calculateSymmetricKey(symmetricKey, sender_receiverModExp, senderSecret);
	_deriveKeys(senderAESKey, senderHashKey, senderCTRNonce, symmetricKey);
    memcpy(aesKey, senderAESKey, ENC_AES_KEY_CHARS);
    memcpy(CTRNonce, senderCTRNonce, ENC_CTR_NONCE_CHARS);
}

int sender_sendData() {
    unsigned char encryptedData[ENC_DATA_SIZE_CHARS];

    #ifndef __ENC_NO_ENCRYPTION_PRINTS__
        digit_t dataDigits[ENC_DATA_SIZE_DIGITS];
    #endif
    field_t data[ENC_DATA_SIZE_CHARS];
    field_t dataPacket[ENC_DATA_PACKET_CHARS];

    #ifndef __ENC_NO_PRINTS__
        size_t i;
    #endif
    uint8_t hmac[ENC_HMAC_CHARS];

    #ifndef __ENC_NO_PRINTS__
        printf("\n\n# Sender\n");
        printf("--------\n");
    #endif

    buffer_read(data, ENC_DATA_SIZE_CHARS);

    #ifndef __ENC_NO_PRINTS__
        printf("--| senderPacketCounter: %d\n", *senderPacketCounter);
    #endif

    #ifndef __ENC_NO_ENCRYPTION_PRINTS__
        printf("--| data\n");
        mpConvFromOctets(dataDigits, ENC_DATA_SIZE_DIGITS, data, ENC_DATA_SIZE_CHARS);
        mpPrintNL(dataDigits, ENC_DATA_SIZE_DIGITS);
    #endif

    _encryptData(encryptedData, senderAESKey, senderCTRNonce, *senderPacketCounter, data, ENC_DATA_SIZE_CHARS);

    #ifndef __ENC_NO_ENCRYPTION_PRINTS__
        printf("--| encryptedData\n");
        mpConvFromOctets(dataDigits, ENC_DATA_SIZE_DIGITS, encryptedData, ENC_DATA_SIZE_CHARS);
        mpPrintNL(dataDigits, ENC_DATA_SIZE_DIGITS);
    #endif

    dataPacket[0] = 0x03;
    memcpy(dataPacket+1, senderPacketCounter, sizeof(uint32_t));
    memcpy(dataPacket+5, encryptedData, ENC_DATA_SIZE_CHARS);

    // Calculate HMAC
    _hmac(hmac, dataPacket, senderHashKey, ENC_HMAC_CHARS, 5+ENC_DATA_SIZE_CHARS, ENC_HMAC_KEY_CHARS);
    memcpy(dataPacket+5+ENC_DATA_SIZE_CHARS, hmac, ENC_HMAC_CHARS);

    #ifndef __ENC_NO_PRINTS__
        printf("--| hmac\n");
        for (i = 0; i < ENC_HMAC_CHARS; i++)
            printf("%x", hmac[i]);
        printf("\n");
    #endif

    #ifndef __ENC_NO_PRINTS__
        printf("--| senderPacketCounter: %d\n", *senderPacketCounter);
    #endif

    channel_write(dataPacket, ENC_DATA_PACKET_CHARS);

    return increaseCounter(senderPacketCounter);
}
