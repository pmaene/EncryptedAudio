#include "crt.h"

void crtModExp(digit_t *restrict result, digit_t *restrict x, digit_t *restrict e, digit_t *restrict p, digit_t *restrict q) {
    digit_t one[ENC_SIGN_PRIME_DIGITS];

    digit_t pMinusOne[ENC_SIGN_PRIME_DIGITS];
    digit_t eP[ENC_SIGN_PRIME_DIGITS];
    digit_t qMinusOne[ENC_SIGN_PRIME_DIGITS];
    digit_t eQ[ENC_SIGN_PRIME_DIGITS];

    digit_t modExpTmpExponent[ENC_SIGNATURE_DIGITS];
    digit_t modExpTmpModulo[ENC_SIGNATURE_DIGITS];
    digit_t modExpResultOne[ENC_SIGNATURE_DIGITS];
    digit_t modExpResultTwo[ENC_SIGNATURE_DIGITS];

    digit_t modExpResultsDifference[ENC_SIGN_PRIME_DIGITS];
    digit_t garnerConstant[ENC_SIGN_PRIME_DIGITS];

    digit_t u[ENC_SIGN_PRIME_DIGITS];
    digit_t up[2*ENC_SIGN_PRIME_DIGITS];

    mpSetZero(one, ENC_SIGN_PRIME_DIGITS);
    *one = 1;

    mpSubtract(pMinusOne, p, one, ENC_SIGN_PRIME_DIGITS);
    mpModulo(eP, e, ENC_PRIVATE_KEY_DIGITS, pMinusOne, ENC_SIGN_PRIME_DIGITS);
    mpSubtract(qMinusOne, q, one, ENC_SIGN_PRIME_DIGITS);
    mpModulo(eQ, e, ENC_PRIVATE_KEY_DIGITS, qMinusOne, ENC_SIGN_PRIME_DIGITS);

    mpSetZero(modExpTmpExponent, ENC_SIGNATURE_DIGITS);
    mpSetEqual(modExpTmpExponent, eP, ENC_SIGN_PRIME_DIGITS);
    mpSetZero(modExpTmpModulo, ENC_SIGNATURE_DIGITS);
    mpSetEqual(modExpTmpModulo, p, ENC_SIGN_PRIME_DIGITS);
    mpModExp(modExpResultOne, x, modExpTmpExponent, modExpTmpModulo, ENC_SIGNATURE_DIGITS);

    mpSetZero(modExpTmpExponent, ENC_SIGNATURE_DIGITS);
    mpSetEqual(modExpTmpExponent, eQ, ENC_SIGN_PRIME_DIGITS);
    mpSetZero(modExpTmpModulo, ENC_SIGNATURE_DIGITS);
    mpSetEqual(modExpTmpModulo, q, ENC_SIGN_PRIME_DIGITS);
    mpModExp(modExpResultTwo, x, modExpTmpExponent, modExpTmpModulo, ENC_SIGNATURE_DIGITS);

    mpSubtract(modExpResultsDifference, modExpResultTwo, modExpResultOne, ENC_SIGN_PRIME_DIGITS);
    if (mpCompare(modExpResultTwo, modExpResultOne, ENC_SIGNATURE_DIGITS) < 0)
        mpAdd(modExpResultsDifference, modExpResultsDifference, q, ENC_SIGN_PRIME_DIGITS);

    mpModInv(garnerConstant, p, q, ENC_SIGN_PRIME_DIGITS);
    mpModMult(u, modExpResultsDifference, garnerConstant, q, ENC_SIGN_PRIME_DIGITS);
    mpMultiply(up, u, p, ENC_SIGN_PRIME_DIGITS);
    mpAdd(result, modExpResultOne, up, 2*ENC_SIGN_PRIME_DIGITS);
}
