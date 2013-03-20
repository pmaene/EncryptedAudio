#include "channel.h"
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

// Memory Pointers
digit_t *senderSecret;
digit_t *receiverModExp;

uint8_t *senderAESKey;
uint8_t *senderHashKey;
uint8_t *senderCTRNonce;

int		*senderCounter;

void sender_construct() {
    senderSecret = calloc(ENC_PRIVATE_KEY_DIGITS, sizeof(digit_t));
    receiverModExp = calloc(ENC_PRIVATE_KEY_DIGITS, sizeof(digit_t));
	senderAESKey = calloc(ENC_HASH_CHARS/2, sizeof(uint8_t));
	senderHashKey = calloc(ENC_HASH_CHARS/2, sizeof(uint8_t));
	senderCTRNonce = calloc(ENC_CTR_NONCE_CHARS, sizeof(uint8_t));
	senderCounter = calloc(1, sizeof(int));
}

void sender_senderHello() {
    field_t sendPacket[ENC_KEY_PACKET_CHARS];

    printf("--> sender_senderHello\n");
    senderHello(sendPacket, senderSecret);

    channel_write(sendPacket, ENC_KEY_PACKET_CHARS);
}

int sender_senderAcknowledge() {
    int returnStatus;

    field_t receivedPacket[ENC_KEY_PACKET_CHARS];
    field_t sendPacket[ENC_KEY_PACKET_CHARS];

    channel_read(receivedPacket, ENC_KEY_PACKET_CHARS);

    printf("--> sender_senderAcknowledge\n");
    returnStatus = senderAcknowledge(sendPacket, receivedPacket, senderSecret, receiverModExp, (unsigned char *) Enc_SenderPrivateExp);

    printf("--| receiverModExp\n");
    mpPrintNL(receiverModExp, ENC_PRIVATE_KEY_DIGITS);

    channel_write(sendPacket, ENC_KEY_PACKET_CHARS);

    return returnStatus;
}

void sender_deriveKey() {
	unsigned char i;
	digit_t symmetricKey[ENC_PRIVATE_KEY_DIGITS];

    printf("--> sender_deriveKey\n");

	_calculateSymmetricKey(symmetricKey, receiverModExp, senderSecret);
	_deriveKeys(senderAESKey, senderHashKey, senderCTRNonce, symmetricKey);

	printf("--| senderAESKey\n");
    for (i = 0; i < ENC_HASH_CHARS/2; i++)
        printf("%x", senderAESKey[i]);

    printf("\n");

	printf("--| senderHashKey\n");
    for (i = 0; i < ENC_HASH_CHARS/2; i++)
        printf("%x", senderHashKey[i]);

    printf("\n");

	printf("--| senderCTRNonce\n");
	    for (i = 0; i < ENC_CTR_NONCE_CHARS; i++)
       		printf("%x", senderCTRNonce[i]);
    printf("\n");
}

void sender_checkEncryption() {
    // Check the encryption algorithm
    unsigned char encryptedPacket[80+16+32];
    unsigned char decryptedPacket[128];
    digit_t dataToEncrypt[256];
    unsigned char dataToEncryptChar[32] =
    "\x01\x02\x03\x04\x5c\xb9\xa9\xda\x98\xf8\x0e\xf0\xad\xa4\xed"
    "\xa9\xe9\x29\xe7\x93\x37\x40\xf1\x73\xb4\xc4\x46\x05\x04\x03"
    "\x02\x01";
    long packetCounter = 0;
    int i;

    sender_deriveKey();

    mpConvFromOctets(dataToEncrypt, 32, dataToEncryptChar, 256);
    printf("--| dataToEncrypt\n");
        for (i = 0; i < 128; i++)
       		printf("%x", dataToEncrypt[i]);

    _encryptData(encryptedPacket, dataToEncrypt, senderCTRNonce, packetCounter, 32);
    printf("\n");
    printf("--| encryptedPacket\n");
	    for (i = 0; i < 128; i++)
       		printf("%x", encryptedPacket[i]);
    printf("\n");
    _decryptData(decryptedPacket,  encryptedPacket, senderCTRNonce, packetCounter, 32);
    printf("\n");
    printf("--| decryptedPacket\n");
	    for (i = 0; i < 128; i++)
       		printf("%x", decryptedPacket[i]);
    printf("\n");

}

void sender_destruct() {
    free(senderSecret);
    free(receiverModExp);
	free(senderAESKey);
	free(senderHashKey);
	free(senderCTRNonce);
	free(senderCounter);
}
