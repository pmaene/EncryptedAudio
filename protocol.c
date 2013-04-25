#include "protocol.h"

void senderHello(field_t *sendPacket, digit_t *senderSecret) {
    unsigned char cModExpResult[ENC_PRIVATE_KEY_CHARS];

    digit_t generator[ENC_PRIVATE_KEY_DIGITS];
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];

    // Generate x
    getRandomDigit(senderSecret, ENC_DH_SECRET_DIGITS);

	// Calculate alpha^x mod p = generator^senderSecret mod prime
    mpConvFromOctets(prime, ENC_PRIVATE_KEY_DIGITS, Enc_Prime, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(generator, ENC_PRIVATE_KEY_DIGITS, Enc_Generator, ENC_PRIVATE_KEY_CHARS);
    mpModExp(modExpResult, generator, senderSecret, prime, ENC_PRIVATE_KEY_DIGITS);
    mpConvToOctets(modExpResult, ENC_PRIVATE_KEY_DIGITS, cModExpResult, ENC_PRIVATE_KEY_CHARS);

    sendPacket[0] = 0x00;
    memcpy(sendPacket+1, cModExpResult, ENC_PRIVATE_KEY_CHARS);
}

int receiverHello(field_t *sendPacket, field_t *receivedPacket, digit_t *receiverSecret, digit_t *senderModExp, unsigned char *receiverPrivateExp) {
    if (0x00 != receivedPacket[0])
        return ENC_REJECT_PACKET_TAG;

    unsigned char cModExpResult[ENC_PRIVATE_KEY_CHARS];
    unsigned char cSenderModExp[ENC_PRIVATE_KEY_CHARS];
    unsigned char cSignature[ENC_ENCRYPTED_SIGNATURE_CHARS];  

    digit_t exponent[ENC_PRIVATE_KEY_DIGITS];
    digit_t generator[ENC_PRIVATE_KEY_DIGITS];
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];
    digit_t p[ENC_SIGN_PRIME_DIGITS];
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];
    digit_t q[ENC_SIGN_PRIME_DIGITS];
    digit_t signature[ENC_SIGNATURE_DIGITS];

    field_t encryptedSignature[ENC_ENCRYPTED_SIGNATURE_CHARS];

    uint8_t signatureMessage[2*ENC_PRIVATE_KEY_CHARS];
    uint8_t receiverCTRNonce[ENC_CTR_NONCE_CHARS];
    uint8_t receiverAESKey[ENC_AES_KEY_CHARS];

    // Generate y
    getRandomDigit(receiverSecret, ENC_DH_SECRET_DIGITS);

	// Calculate alpha^y mod p = alpha^receiverSecret mod prime
    mpConvFromOctets(prime, ENC_PRIVATE_KEY_DIGITS, Enc_Prime, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(generator, ENC_PRIVATE_KEY_DIGITS, Enc_Generator, ENC_PRIVATE_KEY_CHARS);
    mpModExp(modExpResult, generator, receiverSecret, prime, ENC_PRIVATE_KEY_DIGITS);
    mpConvToOctets(modExpResult, ENC_PRIVATE_KEY_DIGITS, cModExpResult, ENC_PRIVATE_KEY_CHARS);

	// Concatenate alpha^y | alpha^x
    memcpy(signatureMessage, cModExpResult, ENC_PRIVATE_KEY_CHARS);
    memcpy(signatureMessage+ENC_PRIVATE_KEY_CHARS, receivedPacket+1, ENC_PRIVATE_KEY_CHARS);
    memcpy(cSenderModExp, receivedPacket+1, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(senderModExp, ENC_PRIVATE_KEY_DIGITS, cSenderModExp, ENC_PRIVATE_KEY_CHARS);

    // Derive the receiver keys from senderModExp
    receiver_deriveKey(receiverAESKey, receiverCTRNonce, senderModExp);

    // Create signature
    memset(signature, 0, sizeof(signature));
    mpConvFromOctets(exponent, ENC_SIGNATURE_DIGITS, receiverPrivateExp, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(p, ENC_SIGN_PRIME_DIGITS, Enc_ReceiverPrimeOne, ENC_SIGN_PRIME_CHARS);
    mpConvFromOctets(q, ENC_SIGN_PRIME_DIGITS, Enc_ReceiverPrimeTwo, ENC_SIGN_PRIME_CHARS);
    _sign_crt(signature, signatureMessage, exponent, p, q);

    #ifdef __ENC_PRINT_ENCRYPTION
        printf("---| signature\n");
        mpPrintNL(signature, ENC_SIGNATURE_DIGITS);
    #endif

    // Encrypt signature
    mpConvToOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);
    _encryptData(encryptedSignature, receiverAESKey, receiverCTRNonce, 0, cSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);
    #ifdef __ENC_PRINT_ENCRYPTION
        printf("---| encyptedSignature\n");
        memcpy(signature, encryptedSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);
        mpPrintNL(signature, ENC_SIGNATURE_DIGITS);
    #endif

    sendPacket[0] = 0x01;
    memcpy(sendPacket+1, cModExpResult, ENC_PRIVATE_KEY_CHARS);
    memcpy(sendPacket+ENC_PRIVATE_KEY_CHARS+1, encryptedSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);

    return ENC_ACCEPT_PACKET;
}

int senderAcknowledge(field_t *sendPacket, field_t *receivedPacket, digit_t *senderSecret, digit_t *receiverModExp, unsigned char *senderPrivateExp) {
    size_t i;
    unsigned char cModExpResult[ENC_PRIVATE_KEY_CHARS];
    unsigned char cReceiverModExp[ENC_PRIVATE_KEY_CHARS];
    unsigned char cSignature[ENC_ENCRYPTED_SIGNATURE_CHARS];

    digit_t exponent[ENC_PRIVATE_KEY_DIGITS];
    digit_t generator[ENC_PRIVATE_KEY_DIGITS];
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];
    digit_t modulus[ENC_SIGNATURE_DIGITS];
    digit_t p[ENC_SIGN_PRIME_DIGITS];
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];
    digit_t publicExp[ENC_SIGNATURE_DIGITS];
    digit_t q[ENC_SIGN_PRIME_DIGITS];
    digit_t signature[ENC_SIGNATURE_DIGITS];

    field_t encryptedSignature[ENC_ENCRYPTED_SIGNATURE_CHARS];

    uint8_t signatureMessage[2*ENC_PRIVATE_KEY_CHARS];
    uint8_t senderCTRNonce[ENC_CTR_NONCE_CHARS];
    uint8_t senderAESKey[ENC_AES_KEY_CHARS];

    if (0x01 != receivedPacket[0])
        return ENC_REJECT_PACKET_TAG;


    // Calculate alpha^x mod p = alpha^senderSecret mod prime
    mpConvFromOctets(prime, ENC_PRIVATE_KEY_DIGITS, Enc_Prime, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(generator, ENC_PRIVATE_KEY_DIGITS, Enc_Generator, ENC_PRIVATE_KEY_CHARS);
    mpModExp(modExpResult, generator, senderSecret, prime, ENC_PRIVATE_KEY_DIGITS),
    mpConvToOctets(modExpResult, ENC_PRIVATE_KEY_DIGITS, cModExpResult, ENC_PRIVATE_KEY_CHARS);

    // Concatenate alpha^y | alpha^x
    memcpy(signatureMessage, receivedPacket+1, ENC_PRIVATE_KEY_CHARS);
    memcpy(cReceiverModExp, receivedPacket+1, ENC_PRIVATE_KEY_CHARS);
    memcpy(signatureMessage+ENC_PRIVATE_KEY_CHARS, cModExpResult, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(receiverModExp, ENC_PRIVATE_KEY_DIGITS, cReceiverModExp, ENC_PRIVATE_KEY_CHARS);

    //deriveKey from receiverModExp
    sender_deriveKey(senderAESKey, senderCTRNonce, receiverModExp);

    // Decrypt signature
    memcpy(encryptedSignature, receivedPacket+ENC_PRIVATE_KEY_CHARS+1, ENC_ENCRYPTED_SIGNATURE_CHARS);
    _decryptData(cSignature, senderAESKey, senderCTRNonce, 0, (unsigned char *) encryptedSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);

    #ifdef __ENC_PRINT_ENCRYPTION
        printf("---| encryptedSignature\n");
        mpConvToOctets(signature, ENC_SIGNATURE_DIGITS, encryptedSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);
        mpPrintNL(signature, ENC_SIGNATURE_DIGITS);
    #endif

    // Verify signature
    #ifdef __ENC_PRINT_ENCRYPTION
        printf("---| signature\n");
        mpConvFromOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);
        mpPrintNL(signature, ENC_SIGNATURE_DIGITS);
    #endif
    mpConvFromOctets(publicExp, ENC_SIGNATURE_DIGITS, Enc_PublicExp, ENC_PUBLIC_KEY_CHARS);
    mpConvFromOctets(modulus, ENC_SIGNATURE_DIGITS, Enc_ReceiverModulus, ENC_SIGNATURE_CHARS);
    if (!_verify(signature, signatureMessage, publicExp, modulus))
        return ENC_REJECT_PACKET_SIGNATURE;

    // Concatenate alpha^x | alpha^y
    memcpy(signatureMessage, cModExpResult, ENC_PRIVATE_KEY_CHARS);
    memcpy(signatureMessage+ENC_PRIVATE_KEY_CHARS, cReceiverModExp, ENC_PRIVATE_KEY_CHARS);

    // Create Signature
    mpConvFromOctets(exponent, ENC_SIGNATURE_DIGITS, senderPrivateExp, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(p, ENC_SIGN_PRIME_DIGITS, Enc_SenderPrimeOne, ENC_SIGN_PRIME_CHARS);
    mpConvFromOctets(q, ENC_SIGN_PRIME_DIGITS, Enc_SenderPrimeTwo, ENC_SIGN_PRIME_CHARS);
    _sign_crt(signature, signatureMessage, exponent, p, q);

    // Encrypt signature
    memset(cSignature, 0, sizeof(cSignature));
    mpConvToOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);
    _encryptData(encryptedSignature, senderAESKey, senderCTRNonce, 0, cSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);

    printf("\nSignatureMessage:\n");
    for (i=0; i<ENC_PRIVATE_KEY_CHARS*2; i++)
        printf("%x",signatureMessage[i]);
    printf("\n");


    printf("\nsignature before encryption:\n");
    for (i = 0; i < ENC_ENCRYPTED_SIGNATURE_CHARS; i++)
        printf("%x", cSignature[i]);
    printf("\n");

    sendPacket[0] = 0x01;
    mpConvFromOctets(signature, ENC_SIGNATURE_DIGITS, encryptedSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);
    memcpy(sendPacket+1, encryptedSignature, ENC_ENCRYPTED_SIGNATURE_CHARS);

    return ENC_ACCEPT_PACKET;
}
int checkSenderAcknowledge(field_t *ackPacket) {
    
    
    return ENC_ACCEPT_PACKET;
}

int increaseCounter(uint32_t *counter) {
	uint32_t nextValue = *counter + 1;

	if (*counter > nextValue) {
		*counter = nextValue;
		return ENC_COUNTER_WRAPAROUND;
	} else {
		*counter = nextValue;
		return ENC_ACCEPT_PACKET;
	}
}
