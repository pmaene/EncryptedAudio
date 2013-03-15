#include "protocol.h"

void senderHello(field_t *sendPacket, digit_t *senderSecret) {
    unsigned char i;

    unsigned char cModExpResult[ENC_PRIVATE_KEY_CHARS];

    digit_t generator[ENC_PRIVATE_KEY_DIGITS];
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];

    // Generate x
    for (i = 0; i < ENC_DH_SECRET_DIGITS; i++)
        senderSecret[i] = spSimpleRand(0, MAX_DIGIT);

    printf("---| senderSecret\n");
    mpPrintNL(senderSecret, ENC_PRIVATE_KEY_DIGITS);

	// Calculate alpha^x mod p = generator^senderSecret mod prime
    mpConvFromOctets(prime, ENC_PRIVATE_KEY_DIGITS, Enc_Prime, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(generator, ENC_PRIVATE_KEY_DIGITS, Enc_Generator, ENC_PRIVATE_KEY_CHARS);
    mpModExp(modExpResult, generator, senderSecret, prime, ENC_PRIVATE_KEY_DIGITS);
    mpConvToOctets(modExpResult, ENC_PRIVATE_KEY_DIGITS, cModExpResult, ENC_PRIVATE_KEY_CHARS);

    printf("---| modExpResult\n");
    mpPrintNL(modExpResult, ENC_PRIVATE_KEY_DIGITS);

    sendPacket[0] = 0x00;
    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++)
        sendPacket[i+1] = cModExpResult[i];
}

/**
 * @TODO Check packet indices
 */
int receiverHello(field_t *sendPacket, field_t *receivedPacket, digit_t *receiverSecret, digit_t *senderModExp, unsigned char *receiverPrivateExp) {
    if (0x00 != receivedPacket[0])
        return ENC_REJECT_PACKET_TAG;

    unsigned char i;

    unsigned char cModExpResult[ENC_PRIVATE_KEY_CHARS];
    unsigned char cSenderModExp[ENC_PRIVATE_KEY_CHARS];
    unsigned char cSignature[ENC_SIGNATURE_CHARS];

    digit_t exponent[ENC_PRIVATE_KEY_DIGITS];
    digit_t generator[ENC_PRIVATE_KEY_DIGITS];
    digit_t hash[ENC_SIGNATURE_CHARS];
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];
    digit_t modulus[ENC_SIGNATURE_DIGITS];
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];
    digit_t signature[ENC_SIGNATURE_DIGITS];

    uint8_t hashMessage[2*ENC_PRIVATE_KEY_CHARS];
    uint8_t hashResult[ENC_HASH_CHARS];

    // Generate y
    for (i = 0; i < ENC_DH_SECRET_DIGITS; i++)
        receiverSecret[i] = spSimpleRand(0, MAX_DIGIT);

    printf("---| receiverSecret\n");
    mpPrintNL(receiverSecret, ENC_PRIVATE_KEY_DIGITS);

	// Calculate alpha^y mod p = alpha^receiverSecret mod prime
    mpConvFromOctets(prime, ENC_PRIVATE_KEY_DIGITS, Enc_Prime, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(generator, ENC_PRIVATE_KEY_DIGITS, Enc_Generator, ENC_PRIVATE_KEY_CHARS);
    mpModExp(modExpResult, generator, receiverSecret, prime, ENC_PRIVATE_KEY_DIGITS);
    mpConvToOctets(modExpResult, ENC_PRIVATE_KEY_DIGITS, cModExpResult, ENC_PRIVATE_KEY_CHARS);

    printf("---| modExpResult\n");
    mpPrintNL(modExpResult, ENC_PRIVATE_KEY_DIGITS);

	// Concatenate alpha^y | alpha^x
    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++)
        hashMessage[i] = cModExpResult[i];

    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++) {
        hashMessage[ENC_PRIVATE_KEY_CHARS+i] = receivedPacket[i+1];
        cSenderModExp[i] = receivedPacket[i+1];
    }

    mpConvFromOctets(senderModExp, ENC_PRIVATE_KEY_DIGITS, cSenderModExp, ENC_PRIVATE_KEY_CHARS);

	// SHA3( alpha^y | alpha^x )
    _hash(hashResult, hashMessage, ENC_HASH_CHARS, 2*ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(hash, ENC_SIGNATURE_DIGITS, (unsigned char *) hashResult, ENC_HASH_CHARS);

    printf("---| hash\n");
    mpPrintNL(hash, ENC_SIGNATURE_DIGITS);

    // Calculate message^privateExponent mod modulus = hash^exponent mod modulus
    mpConvFromOctets(exponent, ENC_SIGNATURE_DIGITS, receiverPrivateExp, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(modulus, ENC_SIGNATURE_DIGITS, Enc_ReceiverModulus, ENC_SIGNATURE_CHARS);
    _sign(signature, hash, exponent, modulus);
    mpConvToOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_SIGNATURE_CHARS);

    printf("---| signature\n");
    mpPrintNL(signature, ENC_SIGNATURE_DIGITS);

    sendPacket[0] = 0x01;
    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++)
        sendPacket[i+1] = cModExpResult[i];
    for (i = 0; i < ENC_SIGNATURE_CHARS; i++)
        sendPacket[(ENC_PRIVATE_KEY_CHARS+1) + i] = cSignature[i];

    return ENC_ACCEPT_PACKET;
}

int senderAcknowledge(field_t *sendPacket, field_t *receivedPacket, digit_t *senderSecret, digit_t *receiverModExp, unsigned char *senderPrivateExp) {
    if (0x01 != receivedPacket[0])
        return ENC_REJECT_PACKET_TAG;

    unsigned char i;

    unsigned char cModExpResult[ENC_PRIVATE_KEY_CHARS];
    unsigned char cReceiverModExp[ENC_PRIVATE_KEY_CHARS];
    unsigned char cSignature[ENC_SIGNATURE_CHARS];

    digit_t exponent[ENC_PRIVATE_KEY_DIGITS];
    digit_t generator[ENC_PRIVATE_KEY_DIGITS];
    digit_t hash[ENC_SIGNATURE_CHARS];
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];
    digit_t modulus[ENC_SIGNATURE_DIGITS];
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];
    digit_t publicExp[ENC_SIGNATURE_DIGITS];
    digit_t signature[ENC_SIGNATURE_DIGITS];

    uint8_t hashMessage[2*ENC_PRIVATE_KEY_CHARS];
    uint8_t hashResult[ENC_HASH_CHARS];

    // Calculate alpha^x mod p = alpha^senderSecret mod prime
    mpConvFromOctets(prime, ENC_PRIVATE_KEY_DIGITS, Enc_Prime, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(generator, ENC_PRIVATE_KEY_DIGITS, Enc_Generator, ENC_PRIVATE_KEY_CHARS);
    mpModExp(modExpResult, generator, senderSecret, prime, ENC_PRIVATE_KEY_DIGITS),
    mpConvToOctets(modExpResult, ENC_PRIVATE_KEY_DIGITS, cModExpResult, ENC_PRIVATE_KEY_CHARS);

    // Concatenate alpha^y | alpha^x
    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++) {
        hashMessage[i] = receivedPacket[i+1];
        cReceiverModExp[i] = receivedPacket[i+1];
    }

    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++)
        hashMessage[ENC_PRIVATE_KEY_CHARS+i] = cModExpResult[i];

    mpConvFromOctets(receiverModExp, ENC_PRIVATE_KEY_DIGITS, cReceiverModExp, ENC_PRIVATE_KEY_CHARS);

    // SHA3( alpha^y | alpha^x )
    _hash(hashResult, hashMessage, ENC_HASH_CHARS, 2*ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(hash, ENC_SIGNATURE_DIGITS, (unsigned char *) hashResult, ENC_HASH_CHARS);

    printf("---| hash\n");
    mpPrintNL(hash, ENC_SIGNATURE_DIGITS);

    // Verify Signature
    for (i = 0; i < ENC_SIGNATURE_CHARS; i++)
        cSignature[i] = receivedPacket[(ENC_PRIVATE_KEY_CHARS+1) + i];

    mpConvFromOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_SIGNATURE_CHARS);
    printf("---| signature\n");
    mpPrintNL(signature, ENC_SIGNATURE_DIGITS);
    mpConvFromOctets(publicExp, ENC_SIGNATURE_DIGITS, Enc_PublicExp, ENC_PUBLIC_KEY_CHARS);
    mpConvFromOctets(modulus, ENC_SIGNATURE_DIGITS, Enc_ReceiverModulus, ENC_SIGNATURE_CHARS);
    if (!_verify(signature, hash, publicExp, modulus))
        return ENC_REJECT_PACKET_SIGNATURE;

    // Concatenate alpha^x | alpha^y
    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++)
        hashMessage[i] = cModExpResult[i];

    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++)
        hashMessage[(ENC_PRIVATE_KEY_CHARS-1)+i] = cReceiverModExp[i];

    // SHA3( alpha^x | alpha^y )
    _hash(hashResult, hashMessage, ENC_HASH_CHARS, 2*ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(hash, ENC_SIGNATURE_DIGITS, (unsigned char *) hashResult, ENC_HASH_CHARS);

    printf("---| hash\n");
    mpPrintNL(hash, ENC_SIGNATURE_DIGITS);

    // Calculate message^privateExponent mod modulus = hash^exponent mod modulus
    mpConvFromOctets(exponent, ENC_SIGNATURE_DIGITS, senderPrivateExp, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(modulus, ENC_SIGNATURE_DIGITS, Enc_SenderModulus, ENC_SIGNATURE_CHARS);
    _sign(signature, hash, exponent, modulus);
    mpConvToOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_SIGNATURE_CHARS);

    printf("---| signature\n");
    mpPrintNL(signature, ENC_SIGNATURE_DIGITS);

    sendPacket[0] = 0x01;
    for (i = 0; i < ENC_PRIVATE_KEY_CHARS; i++)
        sendPacket[i+1] = cReceiverModExp[i];
    for (i = 0; i < ENC_SIGNATURE_CHARS; i++)
        sendPacket[(ENC_PRIVATE_KEY_CHARS+1) + i] = cSignature[i];

    return ENC_ACCEPT_PACKET;
}

int count(int *counter) {
	int counterforward = *counter + 1;
	if(*counter > counterforward) {
		*counter = counterforward;
		return COUNTER_WRAPAROUND;
	} else {
		*counter = counterforward;
		return NO_COUNTER_WRAPAROUND;
	}
}

void sendData(field_t *sendPacket) {

}
