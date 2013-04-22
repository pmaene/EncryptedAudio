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
    unsigned char cSignature[ENC_SIGNATURE_CHARS];

    digit_t exponent[ENC_PRIVATE_KEY_DIGITS];
    digit_t generator[ENC_PRIVATE_KEY_DIGITS];
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];
    digit_t p[ENC_SIGN_PRIME_DIGITS];
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];
    digit_t q[ENC_SIGN_PRIME_DIGITS];
    digit_t signature[ENC_SIGNATURE_DIGITS];

    uint8_t signatureMessage[2*ENC_PRIVATE_KEY_CHARS];

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

    // Create Signature
    mpConvFromOctets(exponent, ENC_SIGNATURE_DIGITS, receiverPrivateExp, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(p, ENC_SIGN_PRIME_DIGITS, Enc_ReceiverPrimeOne, ENC_SIGN_PRIME_CHARS);
    mpConvFromOctets(q, ENC_SIGN_PRIME_DIGITS, Enc_ReceiverPrimeTwo, ENC_SIGN_PRIME_CHARS);
    _sign_crt(signature, signatureMessage, exponent, p, q);

    mpConvToOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_SIGNATURE_CHARS);

    sendPacket[0] = 0x01;
    memcpy(sendPacket+1, cModExpResult, ENC_PRIVATE_KEY_CHARS);
    memcpy(sendPacket+ENC_PRIVATE_KEY_CHARS+1, cSignature, ENC_SIGNATURE_CHARS);

    return ENC_ACCEPT_PACKET;
}

int senderAcknowledge(field_t *sendPacket, field_t *receivedPacket, digit_t *senderSecret, digit_t *receiverModExp, unsigned char *senderPrivateExp) {
    if (0x01 != receivedPacket[0])
        return ENC_REJECT_PACKET_TAG;

    unsigned char cModExpResult[ENC_PRIVATE_KEY_CHARS];
    unsigned char cReceiverModExp[ENC_PRIVATE_KEY_CHARS];
    unsigned char cSignature[ENC_SIGNATURE_CHARS];

    digit_t exponent[ENC_PRIVATE_KEY_DIGITS];
    digit_t generator[ENC_PRIVATE_KEY_DIGITS];
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];
    digit_t modulus[ENC_SIGNATURE_DIGITS];
    digit_t p[ENC_SIGN_PRIME_DIGITS];
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];
    digit_t publicExp[ENC_SIGNATURE_DIGITS];
    digit_t q[ENC_SIGN_PRIME_DIGITS];
    digit_t signature[ENC_SIGNATURE_DIGITS];

    uint8_t signatureMessage[2*ENC_PRIVATE_KEY_CHARS];

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

    // Verify Signature
    memcpy(cSignature, receivedPacket+ENC_PRIVATE_KEY_CHARS+1, ENC_SIGNATURE_CHARS);
    mpConvFromOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_SIGNATURE_CHARS);
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

    mpConvToOctets(signature, ENC_SIGNATURE_DIGITS, cSignature, ENC_SIGNATURE_CHARS);

    sendPacket[0] = 0x01;
    memcpy(sendPacket+1, cReceiverModExp, ENC_PRIVATE_KEY_CHARS);
    memcpy(sendPacket+ENC_PRIVATE_KEY_CHARS, cSignature, ENC_SIGNATURE_CHARS);

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
