void senderHello(field_t *sendPacket, digit_t *senderModExp, digit_t *senderSecret) {
    unsigned char cModExpResult[ENC_PRIVATE_KEY_CHARS];

    // Generate x
    getRandomDigit(senderSecret);

    // Calculate alpha^x mod p = generator^senderSecret mod prime
    mpModExp(modExpResult, Enc_GeneratorDigits, senderSecret, Enc_PrimeDigits, ENC_PRIVATE_KEY_DIGITS );
    mpConvToOctets(modExpResult, ENC_PRIVATE_KEY_DIGITS, cModExpResult, ENC_PRIVATE_KEY_CHARS);
    sendPacket[0] = 0x00;
    memcpy(sendPacket+1, cModExpResult, ENC_PRIVATE_KEY_CHARS);
    memcpy(senderModExp, modExpResult, ENC_PRIVATE_KEY_DIGITS*sizeof(digit_t));
}
