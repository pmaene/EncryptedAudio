#include "protocol.h"
#include "sha3.h"

// Private Declarations
void _hash(uint8_t *hash, uint8_t *data);

// Diffie-Hellman
const unsigned char Enc_Generator[ENC_KEY_CHARS] =
	"\x82\xc1\x57\x1c\xf6\x8d\x59\xaa\xc1\x93\x67\xc7\xde\x23\x4b"
	"\xe8\xc3\xb9\x20\xb8\x36\x2d\x21\xf5\x3e\x3c\x6b\xc8\x4e\xaa"
	"\x5c\x54\x8d\x84\x88\x73\x3a\xc3\x27\x8b\xcf\x66\xe6\x35\xbe"
	"\xbd\x20\x73\x4b\x4b\xea\xaa\x48\x46\xf1\xbc\xf4\x8d\xfa\x0d"
	"\x91\xa3\xbb\xd2\x97\x27\x21\x84\x25\x66\x78\x03\x6d\xd5\x66"
	"\x80\xc5\xa1\x63\x6b\x25\x36\xd7\xe2\x3e\xf6\x6a\xac\x1d\x62"
	"\xb7\xdb\x3c\x6f\xbe\x05\x75\x1b\x1b\x64\x2f\x7c\x1a\xba\x7c"
	"\x07\x4f\x48\x8e\x34\x7b\xf4\xd7\xff\x25\x5f\x2d\x13\x4d\x87"
	"\x4b\x06\x54\x19\x84\xe0\x3d\x61\x14\x4e\x7a\xfb\x12\x38\x34"
	"\x62\x5e\xc0\x33\x7d\xea\xe4\x70\xa6\x79\x9c\x05\x87\xbb\xed"
	"\x9a\xcc\xf1\x0d\xf0\x8b";

const unsigned char     Enc_Prime[ENC_KEY_CHARS] =
	"\xd0\xa8\x73\x80\x04\xa3\x5f\x3d\x10\xf9\x34\x92\x24\xcc\x66"
	"\xb8\xdf\x41\x6e\x9b\x8a\x09\x79\x4d\x69\x35\xf7\x12\xa3\x32"
	"\x46\xc6\x43\x51\xef\x3b\xdb\xad\x21\xcb\xf7\x54\x52\x56\x20"
	"\xcb\x8c\xfa\xcc\x71\x48\xea\xe4\x2b\xfb\x4d\x12\x90\x12\xc6"
	"\xf7\xed\x1f\xee\x95\xa1\x3f\xcc\x95\x78\x74\xcf\x55\x91\xb7"
	"\x9f\x4e\xfc\xe6\x28\x44\x27\x5f\x86\xe1\xde\xf3\x20\xe9\x40"
	"\x07\x90\x24\x00\x7d\x17\xaa\x16\xc1\x9f\xc5\x51\x82\xaa\x5c"
	"\xe0\x42\xac\xe5\x7f\xe9\xaf\xaa\x46\x47\xb6\x76\x80\x3e\x03"
	"\xbc\xe8\xc1\x05\x90\xcc\xd0\x48\x79\xa9\x0e\xab\xcb\x18\x51"
	"\x2a\xa5\xdb\x1c\x6b\xa9\xfc\xd2\xea\x89\xae\xab\xe3\x9c\x3f"
	"\x6b\x03\xc7\x98\xac\x43";

// RSA
const unsigned char Enc_PublicExp[ENC_KEY_CHARS] =
    "\x01\x00\x01";

void senderHello(field_t *sendPacket, int *senderSecret) {
	unsigned char i;
	unsigned char message[ENC_KEY_CHARS];

	unsigned short j;

	digit_t generator[ENC_KEY_DIGITS];
	digit_t prime[ENC_KEY_DIGITS];
	digit_t modExpResult[ENC_KEY_DIGITS];

    mpConvFromOctets(prime, ENC_KEY_DIGITS, Enc_Prime, ENC_KEY_CHARS);
    mpConvFromOctets(generator, ENC_KEY_DIGITS, Enc_Generator, ENC_KEY_CHARS);

    for (i = 0; i < ENC_DH_SECRET_DIGITS; i++)
    	senderSecret[i] = spSimpleRand(0, MAX_DIGIT);

    printf("---| senderSecret\n");
    mpPrintNL(senderSecret, ENC_DH_SECRET_DIGITS);

    mpModExp(modExpResult, generator, senderSecret, prime, ENC_KEY_DIGITS),
    mpConvToOctets(modExpResult, ENC_KEY_DIGITS, message, ENC_KEY_CHARS);

    printf("---| modExpResult\n");
    mpPrintNL(modExpResult, ENC_KEY_DIGITS);

    for (j = 0; j < ENC_KEY_PACKET_CHARS; j++)
    	sendPacket[j] = 0;

    sendPacket[0] = 0x00;
    for (i = 0; i < ENC_KEY_CHARS; i++)
    	sendPacket[i+1] = message[i];
}

/**
 * @TODO Check whether receivedPacket[0] = 0
 * @TODO Check packet indices
 */
void receiverHello(field_t *sendPacket, field_t *receivedPacket, int *receiverSecret, unsigned char *receiverModulus, unsigned char *receiverPrivateExp) {
	unsigned char i;
	unsigned char message[ENC_KEY_CHARS];
	unsigned char signature[ENC_SIGNATURE_CHARS];

	unsigned short j;

	digit_t generator[ENC_KEY_DIGITS];
	digit_t prime[ENC_KEY_DIGITS];
	digit_t modExpResult[ENC_KEY_DIGITS];
	digit_t hash[ENC_HASH_DIGITS];
	digit_t _signResult[ENC_KEY_DIGITS];
	digit_t exponent[ENC_KEY_DIGITS];
	digit_t modulus[ENC_KEY_DIGITS+1];

	uint8_t _hashResult[ENC_HASH_CHARS];
	uint8_t hashMessage[2*ENC_KEY_DIGITS];

    mpConvFromOctets(prime, ENC_KEY_DIGITS, Enc_Prime, ENC_KEY_CHARS);
    mpConvFromOctets(generator, ENC_KEY_DIGITS, Enc_Generator, ENC_KEY_CHARS);

    for (i = 0; i < ENC_DH_SECRET_DIGITS; i++)
    	receiverSecret[i] = spSimpleRand(0, MAX_DIGIT);

    printf("---| receiverSecret\n");
    mpPrintNL(receiverSecret, ENC_DH_SECRET_DIGITS);

    mpModExp(modExpResult, generator, receiverSecret, prime, ENC_KEY_DIGITS),
    mpConvToOctets(modExpResult, ENC_KEY_DIGITS, message, ENC_KEY_CHARS);

    printf("---| modExpResult\n");
    mpPrintNL(modExpResult, ENC_KEY_DIGITS);

    for (i = 0; i < ENC_KEY_DIGITS; i++)
    	hashMessage[i] = modExpResult[i];
    for (i = 0; i < ENC_KEY_DIGITS; i++)
    	hashMessage[(ENC_KEY_DIGITS-1)+i] = receivedPacket[i+1];

    _hash(_hashResult, hashMessage);
    mpConvFromOctets(hash, ENC_HASH_DIGITS, _hashResult, ENC_HASH_CHARS);

    printf("---| hash\n");
    mpPrintNL(hash, ENC_HASH_DIGITS);

    mpConvFromOctets(exponent, ENC_HASH_DIGITS, receiverPrivateExp, ENC_HASH_CHARS);
    mpConvFromOctets(modulus, ENC_HASH_DIGITS, receiverModulus, ENC_HASH_CHARS);
    _sign(_signResult, hash, exponent, modulus);
    mpConvToOctets(_signResult, ENC_SIGNATURE_DIGITS, signature, ENC_SIGNATURE_CHARS);

    printf("---| signature\n");
    mpPrintNL(_signResult, ENC_SIGNATURE_DIGITS);

    for (j = 0; j < ENC_KEY_PACKET_CHARS; j++)
    	sendPacket[j] = 0;

    sendPacket[0] = 0x01;
    for (i = 0; i < ENC_KEY_CHARS; i++)
    	sendPacket[i+1] = message[i];
    for (i = 0; i < ENC_KEY_CHARS; i++)
    	sendPacket[ENC_KEY_CHARS+i] = signature[i];
}

void senderAcknowledge(field_t *sendPacket, field_t *receivedPacket, int *senderSecret, unsigned char *senderModulus, unsigned char *senderPrivateExp) {
}

void sendData(field_t *sendPacket) {
}

void _sign(digit_t *signature, digit_t *message, digit_t *exponent, digit_t *modulus) {
	mpModExp(signature, message, exponent, modulus, ENC_SIGNATURE_DIGITS);
}

int _verify() {

}

void _hash(uint8_t *hash, uint8_t *data) {
	struct sha3_256_ctx ctx;

	sha3_256_init(&ctx);
	sha3_256_update(&ctx, sizeof(data), data);
	sha3_256_digest(&ctx, sizeof(hash), hash);
}
