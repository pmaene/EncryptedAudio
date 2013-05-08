#include "crypto.h"

/* From RFC 3447, Public-Key Cryptography Standards (PKCS) #1: RSA
 * Cryptography Specifications Version 2.1.
 *
 *     id-sha256    OBJECT IDENTIFIER ::=
 *       {joint-iso-itu-t(2) country(16) us(840) organization(1)
 *         gov(101) csor(3) nistalgorithm(4) hashalgs(2) 1}
 */
const uint8_t sha256_prefix[19] =
{
  /* 19 octets prefix, 32 octets hash, total 51 */
  0x30,      49, /* SEQUENCE */
    0x30,    13, /* SEQUENCE */
      0x06,   9, /* OBJECT IDENTIFIER */
        0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
      0x05,   0, /* NULL */
    0x04,    32  /* OCTET STRING */
      /* Here comes the raw hash value */
};

const size_t sha256_prefix_size = sizeof(sha256_prefix);

static void _hash(uint8_t *hash, uint8_t *data, size_t hashLength, size_t dataLength);
#ifdef __ENC_USE_SHA1__
    static void _hash_sha1(uint8_t *hash, uint8_t *data, size_t hashLength, size_t dataLength);
#endif
static void _hash_sha2(uint8_t *hash, uint8_t *data, size_t hashLength, size_t dataLength);
#ifdef __ENC_USE_SHA3__
    static void _hash_sha3(uint8_t *hash, uint8_t *data, size_t hashLength, size_t dataLength);
#endif

void _pkcs_prepareHash(uint8_t *preparedHash, const uint8_t *prefix, uint8_t *hash, size_t preparedHashLength, size_t prefixLength, size_t hashLength, size_t modulusLength);

// Diffie-Hellman
const unsigned char        Enc_Generator[ENC_PRIVATE_KEY_CHARS] =
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

const unsigned char            Enc_Prime[ENC_PRIVATE_KEY_CHARS] =
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
const unsigned char   Enc_SenderModulus[ENC_SIGN_MODULUS_CHARS] =
    "\x00\xa2\x79\xf4\x8f\xe4\x08\x9d\x96\x13\x09\x66\x31\x6f\x56"
    "\x48\x78\x2c\xaa\xba\x8d\x01\x05\x6f\x41\xb2\xba\x63\x6a\x7a"
    "\x8c\x1f\x54\x33\x31\xc3\x0f\xa0\xe7\x30\x35\x36\x09\x51\x6f"
    "\x6d\xdd\xaf\xe5\x35\xc9\xb9\x6e\x16\x73\xa6\x30\x15\xff\xad"
    "\x86\xaf\xc3\xf4\x6c\x8d\x4a\x76\xe5\xb6\x4e\xe4\x8f\x60\x12"
    "\x46\xc0\xcd\x42\x65\x04\x58\x08\xf1\x3a\xa3\xe2\xc9\x1a\xc3"
    "\x3c\x71\x81\xe9\x20\xff\x7a\xc0\xff\x8e\x24\x44\x34\x6e\x5a"
    "\x8f\xfc\x13\x94\xf6\x09\x19\x57\x94\x0a\xec\xcf\xe7\x19\x11"
    "\x96\x1d\x00\xff\x44\x63\xb7\x40\x3c\xa9\xe4\xd2\x51\x42\x0a"
    "\xd4\xc7\x09\xc5\xea\x72\xdb\x6a\xf8\x3b\x0c\x06\x5f\x4f\x67"
    "\xeb\x2b\x53\x07\xa4\x86\xf7";

const unsigned char    Enc_SenderPrimeOne[ENC_SIGN_PRIME_CHARS] =
    "\x00\xd3\x2f\xd1\x07\xdd\x06\x98\x3a\x2d\x60\x78\xad\x94\xbd"
    "\x30\xbe\x92\x48\x00\x0b\x50\xc3\xe3\x97\xec\xb1\xc5\x3d\x12"
    "\xa3\xc8\x9d\x32\x10\x05\x64\x0c\x6a\x2c\xef\x00\xba\x05\x00"
    "\xdc\x8a\x11\x78\x4a\x80\x04\x77\x61\x49\xee\x36\x39\xeb\xe4"
    "\xf2\x4a\xba\x57\x38\x5d\x59\x9b\xbb\x6f\xd4\xdb\xf4\xf4\x2f"
    "\x7a\x1a\xde\xeb";

const unsigned char    Enc_SenderPrimeTwo[ENC_SIGN_PRIME_CHARS] =
    "\x00\xc4\xf4\x12\x1e\xa3\xaa\x33\x72\xd5\xed\x87\x8a\x84\x6e"
    "\x0e\xe6\xfa\x0c\x5d\x1c\xd4\x5c\x24\x3d\x51\xfe\xc0\xe6\x06"
    "\x15\x8d\x22\xa2\xc5\x45\xd3\x9e\x15\x97\x33\x4f\x9f\x34\x26"
    "\xa9\x0d\xd6\x2d\x9f\x68\xb5\x80\x0a\x7e\xef\xca\x6f\xa2\x2a"
    "\x4b\x26\x3e\x74\x96\xb4\xaf\x43\xba\x77\x50\x56\x2a\x88\x9d"
    "\x02\xf9\x2d\x25";

const unsigned char Enc_ReceiverModulus[ENC_SIGN_MODULUS_CHARS] =
    "\x00\xb2\x20\x91\x91\x03\xe9\x2f\x99\x59\xa3\x65\x80\x2c\xb5"
    "\x25\x03\x03\xaf\xe9\x3c\x42\x13\x07\xc2\x5e\x15\x1b\x21\x09"
    "\x9e\x31\xcb\xef\xaf\x19\xa7\xda\x52\xfc\x8c\x01\x2b\x53\x4e"
    "\xee\x53\x2d\x31\xa6\xe5\x3c\x26\xbc\x61\x9e\x38\x86\x74\xc1"
    "\x44\xf9\xb1\x9c\x87\x18\x9f\x58\x3d\x0c\x00\x58\x83\xaa\x5c"
    "\x77\xe1\xd2\x5d\x04\x8c\x00\xb6\x24\x91\x66\x15\x6a\xd9\xc1"
    "\xaa\x3e\x02\x99\xb5\x11\xad\xf6\xb4\x73\x77\x91\xda\x4a\xb2"
    "\xb6\xa5\x3c\xda\xcd\x2f\xaa\xc9\x47\x51\x20\xf2\x87\xe3\xbe"
    "\x3b\xf5\x8f\x15\x63\xb4\xc1\xb0\xb0\x22\x51\xf0\x4b\xd4\x1c"
    "\x50\x91\xa8\x5b\x8f\x09\x55\x5d\x46\x65\x41\xa7\x4e\xab\x5f"
    "\x4c\xf1\x86\x05\x55\xdf\x83";

const unsigned char  Enc_ReceiverPrimeOne[ENC_SIGN_PRIME_CHARS] =
    "\x00\xc1\x44\x3b\x5b\x7b\xb9\x35\x3b\xd3\x5b\x9a\xe4\x04\xfe"
    "\x79\x61\xd0\xcc\x1d\x4e\xf5\xc0\xab\xfb\x09\x40\x09\x6e\xbf"
    "\x14\x50\x3b\x34\xb3\x63\xb6\xd6\x66\xb6\x00\x80\xd7\x6b\xfd"
    "\xa8\x6a\x65\xa5\x7f\x24\xaa\x98\x56\x2e\x40\x2b\x26\x71\x26"
    "\x4c\xf9\xa0\x6e\x64\xda\x6d\xc0\x15\x1c\x1c\x10\x17\x94\xc6"
    "\x70\x29\x9b\xed";

const unsigned char  Enc_ReceiverPrimeTwo[ENC_SIGN_PRIME_CHARS] =
    "\x00\xeb\xf2\x50\x3d\xba\xd5\x22\xd3\xb7\x03\xab\xa2\x32\x40"
    "\x5a\xda\x3c\xf0\x19\xcf\x69\xfd\x8e\xf8\x7e\x89\xb1\xcb\xf6"
    "\xae\x7d\xa1\x95\xf4\xe9\x50\x2a\xcb\x1d\x9b\x01\xd3\xc2\xd0"
    "\x11\xca\x6e\x76\x01\x9d\x87\x03\x25\x19\x40\xf4\x7e\x1c\xb7"
    "\x80\xfa\x7c\x68\x9b\xcf\xb3\x49\x58\xc9\x0e\xf6\xf4\xe2\x26"
    "\x29\xb9\x5b\x2f";

const unsigned char         Enc_PublicExp[ENC_PUBLIC_KEY_CHARS] =
    "\x01\x00\x01";

// Keys
void _calculateSymmetricKey(digit_t *key, digit_t *modExpResult, digit_t *secret) {
    digit_t prime[ENC_PRIVATE_KEY_DIGITS];

    mpConvFromOctets(prime, ENC_PRIVATE_KEY_DIGITS, Enc_Prime, ENC_PRIVATE_KEY_CHARS);
    mpModExp(key, modExpResult, secret, prime, ENC_PRIVATE_KEY_DIGITS);
}

void _deriveKeys(uint8_t *aesKey, uint8_t *hashKey, uint8_t *CTRNonce, digit_t *symmetricKey) {
    #ifndef __ENC_NO_PRINTS__
        size_t i;
    #endif

    uint8_t hashMessage[ENC_PRIVATE_KEY_CHARS+1];
    uint8_t hashResult[ENC_HASH_DIGEST_CHARS];

    mpConvToOctets(symmetricKey, ENC_PRIVATE_KEY_DIGITS, hashMessage, ENC_PRIVATE_KEY_CHARS);

    #ifndef __ENC_NO_PRINTS__
        printf("---> _deriveKeys \n");
    #endif

    hashMessage[ENC_PRIVATE_KEY_CHARS] = 1;
    _hash(hashResult, hashMessage, ENC_HASH_DIGEST_CHARS, ENC_PRIVATE_KEY_CHARS+1);
    memcpy(aesKey, hashResult, ENC_AES_KEY_CHARS);

    memset(hashMessage, 0, (ENC_PRIVATE_KEY_CHARS+1));
    memcpy(hashMessage, hashResult, ENC_HASH_DIGEST_CHARS);
    hashMessage[ENC_HASH_DIGEST_CHARS+1] = 2;
    _hash(hashResult, hashMessage, ENC_HASH_DIGEST_CHARS, ENC_PRIVATE_KEY_CHARS+1);
    memcpy(hashKey, hashResult, ENC_HMAC_KEY_CHARS);

    memset(hashMessage, 0, (ENC_PRIVATE_KEY_CHARS+1));
    memcpy(hashMessage, hashResult, ENC_HASH_DIGEST_CHARS);
    hashMessage[ENC_HASH_DIGEST_CHARS+1] = 3;
    _hash(hashResult, hashMessage, ENC_HASH_DIGEST_CHARS, ENC_PRIVATE_KEY_CHARS+1);
    memcpy(CTRNonce, hashResult, ENC_CTR_NONCE_CHARS);

    #ifndef __ENC_NO_PRINTS__
        printf("---| aesKey\n");
        for (i = 0; i < ENC_AES_KEY_CHARS; i++)
            printf("%x", aesKey[i]);
        printf("\n");

        printf("---| hashKey\n");
        for (i = 0; i < ENC_HMAC_KEY_CHARS; i++)
            printf("%x", hashKey[i]);
        printf("\n");

        printf("---| CTRNonce\n");
        for (i = 0; i < ENC_CTR_NONCE_CHARS; i++)
            printf("%x", CTRNonce[i]);
        printf("\n");
    #endif
}

// Hashes
static void _hash(uint8_t *hash, uint8_t *data, size_t hashLength, size_t dataLength) {
    #if defined(__ENC_USE_SHA1__)
        _hash_sha1(hash, data, hashLength, dataLength);
    #elif defined(__ENC_USE_SHA2__)
        _hash_sha2(hash, data, hashLength, dataLength);
    #elif defined(__ENC_USE_SHA3__)
        _hash_sha3(hash, data, hashLength, dataLength);
    #endif
}

#ifdef __ENC_USE_SHA1__
    static void _hash_sha1(uint8_t *hash, uint8_t *data, size_t hashLength, size_t dataLength) {
        struct sha1_ctx ctx;

        sha1_init(&ctx);
        sha1_update(&ctx, dataLength, data);
        sha1_digest(&ctx, hashLength, hash);
    }
#endif

static void _hash_sha2(uint8_t *hash, uint8_t *data, size_t hashLength, size_t dataLength) {
    struct sha256_ctx ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, dataLength, data);
    sha256_digest(&ctx, hashLength, hash);
}

#ifdef __ENC_USE_SHA3__
    static void _hash_sha3(uint8_t *hash, uint8_t *data, size_t hashLength, size_t dataLength) {
        struct sha3_256_ctx ctx;

        sha3_256_init(&ctx);
        sha3_256_update(&ctx, dataLength, data);
        sha3_256_digest(&ctx, hashLength, hash);
    }
#endif

void _hmac(uint8_t *hmac, uint8_t *data, uint8_t *key) {
    size_t i;

    uint8_t innerPad[ENC_HASH_DATA_CHARS];
    uint8_t outerPad[ENC_HASH_DATA_CHARS];
    uint8_t zeroPaddedKey[ENC_HASH_DATA_CHARS];
    uint8_t paddedKey[ENC_HASH_DATA_CHARS];
    uint8_t innerHashMessage[ENC_HASH_DATA_CHARS+ENC_DATA_SIZE_CHARS+5];
    uint8_t outerHashMessage[ENC_HASH_DATA_CHARS+ENC_HASH_DIGEST_CHARS];
    uint8_t hashResult[ENC_HASH_DIGEST_CHARS];

    memset(zeroPaddedKey, 0, ENC_HASH_DATA_CHARS);

    // Padding Strings
    memset(innerPad, 0x36, ENC_HASH_DATA_CHARS);
    memset(outerPad, 0x5c, ENC_HASH_DATA_CHARS);

    // Inner Padding
    memcpy(zeroPaddedKey, key, ENC_HMAC_KEY_CHARS);
    for (i = 0; i < ENC_HASH_DATA_CHARS; i++)
        paddedKey[i] = zeroPaddedKey[i] ^ innerPad[i];

    // Append Data
    memcpy(innerHashMessage, paddedKey, ENC_HASH_DATA_CHARS);
    memcpy(innerHashMessage+ENC_HASH_DATA_CHARS, data, ENC_DATA_SIZE_CHARS+5);

    _hash(hashResult, innerHashMessage, ENC_HASH_DIGEST_CHARS, ENC_HASH_DATA_CHARS+ENC_DATA_SIZE_CHARS+5);

    // Outer Padding
    for (i = 0; i < ENC_HASH_DATA_CHARS; i++)
        paddedKey[i] = zeroPaddedKey[i] ^ outerPad[i];

    // Append Hash
    memcpy(outerHashMessage, paddedKey, ENC_HASH_DATA_CHARS);
    memcpy(outerHashMessage+ENC_HASH_DATA_CHARS, hashResult, ENC_HASH_DIGEST_CHARS);

    _hash(hashResult, outerHashMessage, ENC_HASH_DIGEST_CHARS, ENC_HASH_DATA_CHARS+ENC_HASH_DIGEST_CHARS);

    memcpy(hmac, hashResult, ENC_HMAC_CHARS);
}

// Signatures
void _sign(digit_t *signature, uint8_t *message, digit_t *privateExponent, digit_t *modulus) {
    digit_t preparedHash[ENC_SIGNATURE_DIGITS];

    uint8_t cHash[ENC_HASH_DIGEST_CHARS];
    uint8_t cPreparedHash[ENC_SIGNATURE_CHARS];

    // PKCS(SHA2( alpha^y | alpha^x ))
    _hash_sha2(cHash, message, ENC_HASH_DIGEST_CHARS, 2*ENC_PRIVATE_KEY_CHARS);
    _pkcs_prepareHash(cPreparedHash, sha256_prefix, cHash, ENC_SIGNATURE_CHARS, sha256_prefix_size, ENC_HASH_DIGEST_CHARS, ENC_SIGNATURE_CHARS);

    mpConvFromOctets(preparedHash, ENC_SIGNATURE_DIGITS, (unsigned char *) cPreparedHash, ENC_SIGNATURE_CHARS);

    mpModExp(signature, preparedHash, privateExponent, modulus, ENC_SIGNATURE_DIGITS);
}

void _sign_crt(digit_t *signature, uint8_t *message, digit_t *privateExponent, digit_t *p, digit_t *q) {
    digit_t preparedHash[ENC_SIGNATURE_DIGITS];

    uint8_t cHash[ENC_HASH_DIGEST_CHARS];
    uint8_t cPreparedHash[ENC_SIGNATURE_CHARS];

    // PKCS(SHA2( alpha^y | alpha^x ))
    _hash_sha2(cHash, message, ENC_HASH_DIGEST_CHARS, 2*ENC_PRIVATE_KEY_CHARS);
    _pkcs_prepareHash(cPreparedHash, sha256_prefix, cHash, ENC_SIGNATURE_CHARS, sha256_prefix_size, ENC_HASH_DIGEST_CHARS, ENC_SIGNATURE_CHARS);

    mpConvFromOctets(preparedHash, ENC_SIGNATURE_DIGITS, (unsigned char *) cPreparedHash, ENC_SIGNATURE_CHARS);

    crtModExp(signature, preparedHash, privateExponent, p, q);
}

int _verify(digit_t *signature, uint8_t *message, digit_t *publicExponent, digit_t *modulus) {
    digit_t preparedHash[ENC_SIGN_MODULUS_DIGITS];
    digit_t modExpResult[ENC_SIGN_MODULUS_DIGITS];

    uint8_t cHash[ENC_HASH_DIGEST_CHARS];
    uint8_t cPreparedHash[ENC_SIGNATURE_CHARS];

    mpSetZero(preparedHash, ENC_SIGN_MODULUS_DIGITS);

    // PKCS(SHA2( alpha^y | alpha^x ))
    _hash_sha2(cHash, message, ENC_HASH_DIGEST_CHARS, 2*ENC_PRIVATE_KEY_CHARS);
    _pkcs_prepareHash(cPreparedHash, sha256_prefix, cHash, ENC_SIGNATURE_CHARS, sha256_prefix_size, ENC_HASH_DIGEST_CHARS, ENC_SIGNATURE_CHARS);
    mpConvFromOctets(preparedHash, ENC_SIGNATURE_DIGITS, (unsigned char *) cPreparedHash, ENC_SIGNATURE_CHARS);

    mpModExp(modExpResult, signature, publicExponent, modulus, ENC_SIGN_MODULUS_DIGITS);

    if (mpEqual(modExpResult, preparedHash, ENC_SIGNATURE_DIGITS)) {
        #ifndef __ENC_NO_PRINTS__
            printf("---> Verification Successful\n");
        #endif
        return ENC_SIGNATURE_ACCEPTED;
    }

    #ifndef __ENC_NO_PRINTS__
        printf("---> Verification Failed\n");
    #endif
    return ENC_SIGNATURE_REJECTED;
}

void _pkcs_prepareHash(uint8_t *preparedHash, const uint8_t *prefix, uint8_t *hash, size_t preparedHashLength, size_t prefixLength, size_t hashLength, size_t modulusLength) {
    size_t psLength;

    memset(preparedHash, 0x00, preparedHashLength);
    memset(preparedHash+1, 0x01, 1);

    psLength = modulusLength - (prefixLength + hashLength) - 3;
    memset(preparedHash+2, 0xff, psLength);

    memcpy(preparedHash+psLength+3, prefix, prefixLength);
    memcpy(preparedHash+prefixLength+psLength+3, hash, hashLength);
}

// Encryption
void _encryptData(unsigned char *encryptedData, uint8_t *aesKey, uint8_t *nonce, uint32_t packetCounter, unsigned char *dataToEncrypt, size_t dataSize) {
    aes_key key;

    size_t blockCounter;
    size_t i;

    unsigned char encryptedBlock[aes_BLOCK_SIZE];
    unsigned char blockToEncrypt[aes_BLOCK_SIZE];

    for (blockCounter = 0; blockCounter < dataSize/aes_BLOCK_SIZE; blockCounter++) {
        memcpy(blockToEncrypt, nonce, ENC_CTR_NONCE_CHARS);
        memcpy(blockToEncrypt+ENC_CTR_NONCE_CHARS, &packetCounter, sizeof(uint32_t));
        memcpy(blockToEncrypt+ENC_CTR_NONCE_CHARS+sizeof(uint32_t), &blockCounter, sizeof(size_t));

        aes_set_encrypt_key(&key, aesKey, ENC_AES_KEY_BITS);

        aes_encrypt(&key, blockToEncrypt, encryptedBlock);

        for (i = 0; i < aes_BLOCK_SIZE; i++)
            encryptedData[i+blockCounter*aes_BLOCK_SIZE] = encryptedBlock[i] ^ dataToEncrypt[i+blockCounter*aes_BLOCK_SIZE];
    }
}

void _decryptData(unsigned char *decryptedData, uint8_t *aesKey, uint8_t *nonce, uint32_t packetCounter, unsigned char *dataToDecrypt, size_t dataSize) {
    aes_key key;

    size_t blockCounter;
    size_t i;

    unsigned char encryptedBlock[aes_BLOCK_SIZE];
    unsigned char blockToEncrypt[aes_BLOCK_SIZE];

    for (blockCounter = 0; blockCounter < dataSize/aes_BLOCK_SIZE; blockCounter++) {
        memcpy(blockToEncrypt, nonce, ENC_CTR_NONCE_CHARS);
        memcpy(blockToEncrypt+ENC_CTR_NONCE_CHARS, &packetCounter, sizeof(uint32_t));
        memcpy(blockToEncrypt+ENC_CTR_NONCE_CHARS+sizeof(uint32_t), &blockCounter, sizeof(size_t));

        aes_set_encrypt_key(&key, aesKey, ENC_AES_KEY_BITS);
        aes_encrypt(&key, blockToEncrypt, encryptedBlock);

        for (i = 0; i < aes_BLOCK_SIZE; i++)
            decryptedData[i+blockCounter*aes_BLOCK_SIZE] = encryptedBlock[i] ^ dataToDecrypt[i+blockCounter*aes_BLOCK_SIZE];
    }
}

void _conv_from_octets() {
    mpConvFromOctets(Enc_PrimeDigits, ENC_PRIVATE_KEY_DIGITS, Enc_Prime, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(Enc_GeneratorDigits, ENC_PRIVATE_KEY_DIGITS, Enc_Generator, ENC_PRIVATE_KEY_CHARS);
    mpConvFromOctets(Enc_SenderModulusDigits, ENC_SIGN_MODULUS_DIGITS, Enc_SenderModulus, ENC_SIGN_MODULUS_CHARS);
    mpConvFromOctets(Enc_SenderPrimeOneDigits, ENC_SIGN_PRIME_DIGITS, Enc_SenderPrimeOne, ENC_SIGN_PRIME_CHARS);
    mpConvFromOctets(Enc_SenderPrimeTwoDigits, ENC_SIGN_PRIME_DIGITS, Enc_SenderPrimeTwo, ENC_SIGN_PRIME_CHARS);
    mpConvFromOctets(Enc_ReceiverModulusDigits, ENC_SIGN_MODULUS_DIGITS, Enc_ReceiverModulus, ENC_SIGN_MODULUS_CHARS);
    mpConvFromOctets(Enc_ReceiverPrimeOneDigits, ENC_SIGN_PRIME_DIGITS, Enc_ReceiverPrimeOne, ENC_SIGN_PRIME_CHARS);
    mpConvFromOctets(Enc_ReceiverPrimeTwoDigits, ENC_SIGN_PRIME_DIGITS, Enc_ReceiverPrimeTwo, ENC_SIGN_PRIME_CHARS);
    mpConvFromOctets(Enc_PublicExpDigits, ENC_SIGN_MODULUS_DIGITS, Enc_PublicExp, ENC_PUBLIC_KEY_CHARS);
}
