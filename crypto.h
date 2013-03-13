#ifndef __ENC_CRYPTO_H__
#define __ENC_CRYPTO_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bigdigits.h"
#include "types.h"
#include "sha3.h"

// Key Sizes
#define ENC_PRIVATE_KEY_CHARS   156
#define ENC_PRIVATE_KEY_DIGITS  39
#define ENC_PUBLIC_KEY_CHARS    3
#define ENC_PUBLIC_KEY_DIGITS   1

// Hash Sizes
#define ENC_HASH_CHARS          32
#define ENC_HASH_DIGITS         8

// Signature Sizes
#define ENC_SIGNATURE_CHARS     157
#define ENC_SIGNATURE_DIGITS    40

#define ENC_SIGNATURE_ACCEPTED  1
#define ENC_SIGNATURE_REJECTED  0

// Diffie-Hellman
const unsigned char Enc_Generator[ENC_PRIVATE_KEY_CHARS];
const unsigned char Enc_Prime[ENC_PRIVATE_KEY_CHARS];

// RSA
const unsigned char Enc_SenderModulus[ENC_SIGNATURE_CHARS];
const unsigned char Enc_ReceiverModulus[ENC_SIGNATURE_CHARS];
const unsigned char Enc_PublicExp[ENC_PUBLIC_KEY_CHARS];

// Keys
void _calculateSymmetricKey(digit_t *key, digit_t *modExpResult, digit_t *secret);
void _deriveKeys(uint8_t *aesKey, uint8_t *hashKey, digit_t *symmetricKey);

// Hashes
void _hash(uint8_t *hash, uint8_t *data, unsigned hashLength, unsigned dataLength);

// Signatures
void _sign(digit_t *signature, digit_t *message, digit_t *privateExponent, digit_t *modulus);
int _verify(digit_t *signature, digit_t *message, digit_t *publicExponent, digit_t *modulus);

#endif
