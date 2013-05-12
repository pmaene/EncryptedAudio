#include "receiver.h"

int receiver_checkHmac(field_t *dataPacket);

// RSA
const unsigned char Enc_ReceiverPrivateExp[ENC_PRIVATE_KEY_CHARS] =
    "\x3c\x13\xf0\x04\xd1\x24\xdd\x01\x03\xd2\xb0\x71\x42\xa2\xf6"
    "\x10\xc6\x35\x96\x9c\xd4\x4c\x46\xd9\xaf\x91\xbe\xcc\xad\xc9"
    "\xc5\x51\xdb\x4d\x5b\x81\xd9\x89\x13\xd4\xc4\xb0\x30\x30\xf7"
    "\x00\xe6\xc5\x50\x5d\x10\xa4\x9f\xf4\xec\xc0\x73\xd8\xfd\x36"
    "\x9e\x60\xe1\x42\x4f\x73\xae\xf7\x28\x0d\x15\xcd\x92\xb0\x0d"
    "\x5f\x04\xeb\xf9\xd0\xc0\xe7\x38\xff\x9e\xc3\x2c\xab\x22\xcd"
    "\x4b\xda\xcb\xc7\xbd\x24\xd4\x51\x71\xe5\x99\x01\xcb\xcb\x78"
    "\xc0\xbd\x3f\xc2\xc5\xd9\x02\x46\x68\xa9\xbc\xfd\x11\x3f\xc5"
    "\xdc\xc3\x6a\x55\xad\x3f\x5f\xfb\x08\xad\x01\x5a\xac\x02\xbc"
    "\xa1\xaa\xb7\x88\x01\x2b\xbd\x60\x97\x41\xbf\x5b\x6e\x06\x55"
    "\xf9\x03\xb5\xd1\xd3\xc1";

bool senderTrusted = false;

digit_t receiverSecret[ENC_PRIVATE_KEY_DIGITS];
digit_t receiver_receiverModExp[ENC_PRIVATE_KEY_DIGITS];
digit_t receiver_senderModExp[ENC_PRIVATE_KEY_DIGITS];

uint8_t receiverAESKey[ENC_AES_KEY_CHARS];
uint8_t receiverHashKey[ENC_HMAC_KEY_CHARS];
uint8_t receiverCTRNonce[ENC_CTR_NONCE_CHARS];

uint32_t receiverPacketCounter[1];

void receiver_construct() {
    memset(receiver_receiverModExp, 0, ENC_PRIVATE_KEY_DIGITS*sizeof(digit_t));
    memset(receiverSecret, 0, ENC_PRIVATE_KEY_DIGITS*sizeof(digit_t));
    memset(receiver_senderModExp, 0, ENC_PRIVATE_KEY_DIGITS*sizeof(digit_t));

    memset(receiverAESKey, 0, ENC_AES_KEY_CHARS*sizeof(uint8_t));
    memset(receiverHashKey, 0, ENC_HMAC_KEY_CHARS*sizeof(uint8_t));
    memset(receiverCTRNonce, 0, ENC_CTR_NONCE_CHARS*sizeof(uint8_t));

    memset(receiverPacketCounter, 0, sizeof(uint32_t));
}

int receiver_receiverHello() {
    int returnStatus;

    field_t receivedPacket[ENC_KEY_PACKET_CHARS];
    field_t sendPacket[ENC_KEY_PACKET_CHARS];
    channel_read(receivedPacket, ENC_KEY_PACKET_CHARS);

    #ifndef __ENC_NO_PRINTS__
        printf("--> receiver_receiverHello\n");
    #endif

    returnStatus = receiverHello(sendPacket, receiver_receiverModExp, receivedPacket, receiverSecret, receiver_senderModExp, (unsigned char *) Enc_ReceiverPrivateExp);
    channel_write(sendPacket, ENC_KEY_PACKET_CHARS);

    return returnStatus;
}

void receiver_deriveKey(uint8_t *restrict aesKey, uint8_t *restrict CTRNonce, digit_t *restrict modExp) {
	digit_t symmetricKey[ENC_PRIVATE_KEY_DIGITS];

    #ifndef __ENC_NO_PRINTS__
        printf("--> receiver_deriveKey\n");
    #endif

    memcpy(receiver_senderModExp, modExp, ENC_PRIVATE_KEY_DIGITS);
	_calculateSymmetricKey(symmetricKey, receiver_senderModExp, receiverSecret);
	_deriveKeys(receiverAESKey, receiverHashKey, receiverCTRNonce, symmetricKey);
    memcpy(aesKey, receiverAESKey, ENC_AES_KEY_CHARS);
    memcpy(CTRNonce, receiverCTRNonce, ENC_CTR_NONCE_CHARS);
}

int receiver_receiveData() {
    unsigned char encryptedData[ENC_DATA_SIZE_CHARS];

    field_t dataPacket[ENC_DATA_PACKET_CHARS];
    field_t data[ENC_DATA_SIZE_CHARS];

    #ifndef __ENC_NO_ENCRYPTION_PRINTS__
        digit_t dataDigits[ENC_DATA_SIZE_DIGITS];
    #endif

    uint32_t receivedPacketCounter;

    #ifndef __ENC_NO_PRINTS__
        printf("\n# Receiver\n");
        printf("--------\n");
    #endif

    channel_read(dataPacket, ENC_DATA_PACKET_CHARS);

    memcpy(&receivedPacketCounter, dataPacket+1, sizeof(uint32_t));

    if (receiver_checkHmac(dataPacket) == ENC_HMAC_REJECTED) {
        return ENC_HMAC_REJECTED;
    } else if (dataPacket[0] != 0x03) {
        return ENC_REJECT_PACKET_TAG;
    } else if (*receiverPacketCounter > receivedPacketCounter) {
        return ENC_LOST_PACKET;
    } else {
        while (*receiverPacketCounter != receivedPacketCounter) {
            if (increaseCounter(receiverPacketCounter) == ENC_COUNTER_WRAPAROUND)
                return ENC_COUNTER_WRAPAROUND;
        }

        #ifndef __ENC_NO_PRINTS__
            printf("--| receiverPacketCounter: %d\n", *receiverPacketCounter);
        #endif

        memcpy(encryptedData, dataPacket+5, ENC_DATA_SIZE_CHARS);

        #ifndef __ENC_NO_ENCRYPTION_PRINTS__
            printf("--| encryptedData\n");
            mpConvFromOctets(dataDigits, ENC_DATA_SIZE_DIGITS, encryptedData, ENC_DATA_SIZE_CHARS);
            mpPrintNL(dataDigits, ENC_DATA_SIZE_DIGITS);
        #endif

        _decryptData(data, receiverAESKey, receiverCTRNonce, *receiverPacketCounter, encryptedData, ENC_DATA_SIZE_CHARS);

        #ifndef __ENC_NO_ENCRYPTION_PRINTS__
            printf("--| data\n");
            mpConvFromOctets(dataDigits, ENC_DATA_SIZE_DIGITS, data, ENC_DATA_SIZE_CHARS);
            mpPrintNL(dataDigits, ENC_DATA_SIZE_DIGITS);
            printf("\n");
        #endif

        while (buffer_isModified()) {}
        buffer_write(data, ENC_DATA_SIZE_CHARS);

        return ENC_ACCEPT_PACKET;
    }
}

int receiver_checkSenderAcknowledge() {
    unsigned char ackSignature[ENC_ENCRYPTED_SIGNATURE_CHARS];
    unsigned char decryptedSignature[ENC_ENCRYPTED_SIGNATURE_CHARS];
    unsigned char cReceiverModExp[ENC_PRIVATE_KEY_CHARS];
    unsigned char cSenderModExp[ENC_PRIVATE_KEY_CHARS];

    uint8_t signatureMessage[2*ENC_PRIVATE_KEY_CHARS];

    field_t senderAck[1+ENC_ENCRYPTED_SIGNATURE_CHARS];
    digit_t signature[ENC_SIGN_MODULUS_DIGITS];

    if (senderTrusted == false) {
        channel_read(senderAck, ENC_ENCRYPTED_SIGNATURE_CHARS+1);

        if (senderAck[0] != 1)
            return ENC_REJECT_PACKET_TAG;

        memcpy(ackSignature, senderAck+1, ENC_ENCRYPTED_SIGNATURE_CHARS);

        // Decrypt Signature
        _decryptData(decryptedSignature, receiverAESKey, receiverCTRNonce, 0, ackSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);
        mpConvFromOctets(signature, ENC_ENCRYPTED_SIGNATURE_DIGITS, decryptedSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);

        // Calculate alpha^x | alpha^y
        mpConvToOctets(receiver_receiverModExp, ENC_PRIVATE_KEY_DIGITS, cReceiverModExp, ENC_PRIVATE_KEY_CHARS);
        mpConvToOctets(receiver_senderModExp, ENC_PRIVATE_KEY_DIGITS, cSenderModExp, ENC_PRIVATE_KEY_CHARS);

        memcpy(signatureMessage, cSenderModExp, ENC_PRIVATE_KEY_CHARS);
        memcpy(signatureMessage+ENC_PRIVATE_KEY_CHARS, cReceiverModExp, ENC_PRIVATE_KEY_CHARS);
        
        // Check Signature
        if (!_verify(signature, signatureMessage, Enc_PublicExpDigits, Enc_SenderModulusDigits))
            return ENC_INVALID_ACK;

        senderTrusted = true;
        return ENC_ACCEPT_PACKET;
    }

    return ENC_INVALID_ACK;
}

int receiver_checkHmac(field_t *restrict dataPacket) {
    size_t i;
    uint8_t hmac[ENC_HMAC_CHARS];

    _hmac(hmac, dataPacket, receiverHashKey);

    for (i = 0; i < ENC_HMAC_CHARS; i++) {
        if (hmac[i] != dataPacket[5+ENC_DATA_SIZE_CHARS+i])
            return ENC_HMAC_REJECTED;
    }

    return ENC_HMAC_ACCEPTED;
}
