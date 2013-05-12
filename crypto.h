#ifndef __ENC_CRYPTO_H__
#define __ENC_CRYPTO_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes.h"
#include "bigdigits.h"
#include "crt.h"
#include "protocol.h"
#include "types.h"
#include "sha1.h"
#include "sha2.h"
#include "sha3.h"

// Keys
#define ENC_PRIVATE_KEY_CHARS          156
#define ENC_PRIVATE_KEY_DIGITS         39
#define ENC_PUBLIC_KEY_CHARS           3
#define ENC_PUBLIC_KEY_DIGITS          1

// Hashes
#define __ENC_USE_SHA2__

#if defined(__ENC_USE_SHA1__)
    #define ENC_HASH_DIGEST_CHARS      20
    #define ENC_HASH_DIGEST_DIGITS     5
    #define ENC_HASH_DATA_CHARS        64
    #define ENC_HASH_DATA_DIGITS       8
#elif defined(__ENC_USE_SHA2__)
    #define ENC_HASH_DIGEST_CHARS      32
    #define ENC_HASH_DIGEST_DIGITS     8
    #define ENC_HASH_DATA_CHARS        64
    #define ENC_HASH_DATA_DIGITS       8
#elif defined(__ENC_USE_SHA3__)
    #define ENC_HASH_DIGEST_CHARS      32
    #define ENC_HASH_DIGEST_DIGITS     8
    #define ENC_HASH_DATA_CHARS        136
    #define ENC_HASH_DATA_DIGITS       17
#endif

#define ENC_HMAC_KEY_CHARS             10
#define ENC_HMAC_KEY_DIGITS            3
#define ENC_HMAC_CHARS                 20
#define ENC_HMAC_DIGITS                5

// Signatures
#define ENC_SIGNATURE_CHARS            156
#define ENC_SIGNATURE_DIGITS           39
#define ENC_ENCRYPTED_SIGNATURE_CHARS  160
#define ENC_ENCRYPTED_SIGNATURE_DIGITS 40
#define ENC_SIGN_MODULUS_CHARS         157
#define ENC_SIGN_MODULUS_DIGITS        40
#define ENC_SIGN_PRIME_CHARS           79
#define ENC_SIGN_PRIME_DIGITS          20

// Signature Reasons
#define ENC_SIGNATURE_ACCEPTED         1
#define ENC_SIGNATURE_REJECTED         0

// AES-CTR
#define ENC_AES_KEY_BITS               128
#define ENC_AES_KEY_CHARS              16
#define ENC_AES_KEY_DIGITS             4

#define ENC_CTR_CHARS                  16
#define ENC_CTR_DIGITS                 4
#define ENC_CTR_NONCE_CHARS            8
#define ENC_CTR_NONCE_DIGITS           2

// Diffie-Hellman
extern digit_t Enc_GeneratorDigits[ENC_PRIVATE_KEY_DIGITS];
extern digit_t Enc_PrimeDigits[ENC_PRIVATE_KEY_DIGITS];

// RSA
extern digit_t Enc_SenderModulusDigits[ENC_PRIVATE_KEY_DIGITS];
extern digit_t Enc_SenderPrimeOneDigits[ENC_PRIVATE_KEY_DIGITS];
extern digit_t Enc_SenderPrimeTwoDigits[ENC_PRIVATE_KEY_DIGITS];
extern digit_t Enc_ReceiverModulusDigits[ENC_PRIVATE_KEY_DIGITS];
extern digit_t Enc_ReceiverPrimeOneDigits[ENC_PRIVATE_KEY_DIGITS];
extern digit_t Enc_ReceiverPrimeTwoDigits[ENC_PRIVATE_KEY_DIGITS];
extern digit_t Enc_PublicExpDigits[ENC_PRIVATE_KEY_DIGITS];

// Keys
void _calculateSymmetricKey(digit_t *restrict key, digit_t *restrict modExpResult, digit_t *restrict secret);
void _deriveKeys(uint8_t *restrict aesKey, uint8_t *restrict hashKey, uint8_t *restrict CTRKey, digit_t *restrict symmetricKey);

// Hashes
void _hmac(uint8_t *restrict hmac, uint8_t *restrict data, uint8_t *restrict key);

// Signatures
void _sign(digit_t *restrict signature, uint8_t *restrict message, digit_t *restrict privateExponent, digit_t *restrict modulus);
void _sign_crt(digit_t *restrict signature, digit_t *restrict message, digit_t *restrict privateExponent, digit_t *restrict p, digit_t *restrict q);
int _verify(digit_t *restrict signature, uint8_t *restrict message, digit_t *restrict publicExponent, digit_t *restrict modulus);

void _encryptData(unsigned char *restrict encryptedData, uint8_t *restrict aesKey, uint8_t *restrict nonce, uint32_t packetCounter, unsigned char *restrict dataToEncrypt, size_t dataSize);
void _decryptData(unsigned char *restrict decryptedData, uint8_t *restrict aesKey, uint8_t *restrict nonce, uint32_t packetCounter, unsigned char *restrict dataToDecrypt, size_t dataSize);

void _convFromOctets();

#endif
