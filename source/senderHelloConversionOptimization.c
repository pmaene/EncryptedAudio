void senderHello(field_t *restrict sendPacket, digit_t *restrict senderModExp, digit_t *restrict senderSecret) {
    digit_t modExpResult[ENC_PRIVATE_KEY_DIGITS];

    // Generate x
    getRandomDigit(senderSecret);

    // Calculate alpha^x mod p = generator^senderSecret mod prime
    mpModExp(modExpResult, Enc_GeneratorDigits, senderSecret, Enc_PrimeDigits, ENC_PRIVATE_KEY_DIGITS);
    sendPacket[0] = 0x00;
    memcpy(sendPacket+1, modExpResult, ENC_PRIVATE_KEY_CHARS);
    memcpy(senderModExp, modExpResult, ENC_PRIVATE_KEY_DIGITS*sizeof(digit_t));
}
