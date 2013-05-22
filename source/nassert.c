#pragma DATA_ALIGN(encryptedData);

void _encryptData(unsigned char *restrict encryptedData, uint8_t *restrict aesKey, uint8_t *restrict nonce, uint32_t packetCounter, unsigned char *restrict dataToEncrypt, size_t dataSize) {
    ...
    _nassert(((int) (array) % 8)==0);
    #pragma MUST_ITERATE(aes_BLOCK_SIZE,,8)
    #pragma UNROLL(8)
    for (i = 0; i < aes_BLOCK_SIZE; i++)
        encryptedData[i+blockCounter*aes_BLOCK_SIZE] = encryptedBlock[i] ^ dataToEncrypt[i+blockCounter*aes_BLOCK_SIZE];
}
