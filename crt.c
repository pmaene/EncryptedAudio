#include "crt.h"

void crtModExp(digit_t *result, digit_t *x, digit_t *e, digit_t *p, digit_t *q, size_t nbMessageDigits, size_t nbExponentDigits, size_t nbPrimeDigits) {
    digit_t one[nbPrimeDigits];

    digit_t pMinusOne[nbPrimeDigits];
    digit_t eP[nbPrimeDigits];
    digit_t qMinusOne[nbPrimeDigits];
    digit_t eQ[nbPrimeDigits];

    digit_t modExpTmpExponent[nbMessageDigits];
    digit_t modExpTmpModulo[nbMessageDigits];
    digit_t modExpResultOne[nbMessageDigits];
    digit_t modExpResultTwo[nbMessageDigits];

    digit_t modExpResultsDifference[nbPrimeDigits];
    digit_t garnerConstant[nbPrimeDigits];

    digit_t u[nbPrimeDigits];
    digit_t up[2*nbPrimeDigits];

    #ifndef __ENC_NO_PRINTS__
        printf("* crtModExp\n");
    #endif

    mpSetZero(one, nbPrimeDigits);
    one[0] = 1;

    mpSubtract(pMinusOne, p, one, nbPrimeDigits);
    mpModulo(eP, e, nbExponentDigits, pMinusOne, nbPrimeDigits);
    mpSubtract(qMinusOne, q, one, nbPrimeDigits);
    mpModulo(eQ, e, nbExponentDigits, qMinusOne, nbPrimeDigits);

    mpSetZero(modExpTmpExponent, nbMessageDigits);
    mpSetEqual(modExpTmpExponent, eP, nbPrimeDigits);
    mpSetZero(modExpTmpModulo, nbMessageDigits);
    mpSetEqual(modExpTmpModulo, p, nbPrimeDigits);
    mpModExp(modExpResultOne, x, modExpTmpExponent, modExpTmpModulo, nbMessageDigits);

    mpSetZero(modExpTmpExponent, nbMessageDigits);
    mpSetEqual(modExpTmpExponent, eQ, nbPrimeDigits);
    mpSetZero(modExpTmpModulo, nbMessageDigits);
    mpSetEqual(modExpTmpModulo, q, nbPrimeDigits);
    mpModExp(modExpResultTwo, x, modExpTmpExponent, modExpTmpModulo, nbMessageDigits);

    mpSubtract(modExpResultsDifference, modExpResultTwo, modExpResultOne, nbPrimeDigits);
    if (mpCompare(modExpResultTwo, modExpResultOne, nbMessageDigits) < 0)
        mpAdd(modExpResultsDifference, modExpResultsDifference, q, nbPrimeDigits);

    mpModInv(garnerConstant, p, q, nbPrimeDigits);
    mpModMult(u, modExpResultsDifference, garnerConstant, q, nbPrimeDigits);
    mpMultiply(up, u, p, nbPrimeDigits);
    mpAdd(result, modExpResultOne, up, 2*nbPrimeDigits);
}
