#ifndef __ENC_CRYPTO_H__
#define __ENC_CRYPTO_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "aes.h"
#include "bigdigits.h"
#include "montgomery.h"
#include "pkcs.h"
#include "types.h"
#include "sha2.h"
#include "sha3.h"

// Key Sizes
#define ENC_PRIVATE_KEY_CHARS   156
#define ENC_PRIVATE_KEY_DIGITS  39
#define ENC_PUBLIC_KEY_CHARS    3
#define ENC_PUBLIC_KEY_DIGITS   1

// Diffie-Hellman Size
#define ENC_DH_SECRET_CHARS     20
#define ENC_DH_SECRET_DIGITS    5

// Hash Sizes
#define ENC_HASH_CHARS          32
#define ENC_HASH_DIGITS         8

// Signature Sizes
#define ENC_SIGNATURE_CHARS     157
#define ENC_SIGNATURE_DIGITS    40
#define ENC_SIGN_PRIME_CHARS    79
#define ENC_SIGN_PRIME_DIGITS   20

// Signature Reasons
#define ENC_SIGNATURE_ACCEPTED  1
#define ENC_SIGNATURE_REJECTED  0

// Nonce Length CTR
#define ENC_CTR_NONCE_CHARS     80
#define ENC_CTR_NONCE_DIGITS    20

// CTR Length Mode
#define ENC_CTR_CHARS           16
#define ENC_CTR_DIGITS          4

// Diffie-Hellman
const unsigned char Enc_Generator[ENC_PRIVATE_KEY_CHARS];
const unsigned char Enc_Prime[ENC_PRIVATE_KEY_CHARS];

// RSA
const unsigned char Enc_SenderModulus[ENC_SIGNATURE_CHARS];
const unsigned char Enc_ReceiverModulus[ENC_SIGNATURE_CHARS];
const unsigned char Enc_PublicExp[ENC_PUBLIC_KEY_CHARS];

// Keys
void _calculateSymmetricKey(digit_t *key, digit_t *modExpResult, digit_t *secret);
void _deriveKeys(uint8_t *aesKey, uint8_t *hashKey, uint8_t *CTRKey, digit_t *symmetricKey);

// Hashes
void _hash(uint8_t *hash, uint8_t *data, unsigned hashLength, unsigned dataLength);
void _hmac(uint8_t *hmac, uint8_t *data, uint8_t *key, unsigned hashLength, unsigned dataLength, unsigned keyLength);

// Signatures
void _sign(digit_t *signature, uint8_t *message, digit_t *privateExponent, digit_t *modulus);
int _verify(digit_t *signature, uint8_t *message, digit_t *publicExponent, digit_t *modulus);

void _encryptData(unsigned char *encryptedData, digit_t *dataToEncrypt, uint8_t *nonce, long packetCounter, int packetSize);
void _decryptData(unsigned char *decryptedData, unsigned char *encryptedData, uint8_t *nonce, long packetCounter, int packetSize);

#endif
