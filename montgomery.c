#include "montgomery.h"

void crtModExp(digit_t *result, digit_t *x, digit_t *e, digit_t *p, digit_t *q, size_t nbMessageDigits, size_t nbExponentDigits, size_t nbPrimeDigits) {
    digit_t one[nbPrimeDigits];

    digit_t eP[nbPrimeDigits];
    digit_t eQ[nbPrimeDigits];
    digit_t pMinusOne[nbPrimeDigits];
    digit_t qMinusOne[nbPrimeDigits];

    digit_t garnerConstant[nbPrimeDigits];

    digit_t modExpResultOne[nbPrimeDigits];
    digit_t modExpResultTwo[nbPrimeDigits];

    digit_t modExpResultsDifference[nbPrimeDigits];
    digit_t u[nbPrimeDigits];
    digit_t up[2*nbPrimeDigits];

    digit_t modExpTmpExponent[nbMessageDigits];
    digit_t modExpTmpModulo[nbMessageDigits];

    printf("\n\n----->crtModExp\n");

    printf("-----| x\n");
    mpPrintDecimal("", x, nbMessageDigits, "");
    printf("\n");

    mpSetZero(one, nbPrimeDigits);
    one[0] = 1;

    mpSubtract(pMinusOne, p, one, nbPrimeDigits);
    mpModulo(eP, e, nbExponentDigits, pMinusOne, nbPrimeDigits);
    mpSubtract(qMinusOne, q, one, nbPrimeDigits);
    mpModulo(eQ, e, nbExponentDigits, qMinusOne, nbPrimeDigits);

    mpModInv(garnerConstant, p, q, nbPrimeDigits);

    mpSetZero(modExpTmpExponent, nbMessageDigits);
    mpSetEqual(modExpTmpExponent, eP, nbPrimeDigits);
    mpSetZero(modExpTmpModulo, nbMessageDigits);
    mpSetEqual(modExpTmpModulo, p, nbPrimeDigits);
    mpModExp(modExpResultOne, x, modExpTmpExponent, modExpTmpModulo, nbMessageDigits);

    mpSetZero(modExpTmpExponent, nbMessageDigits);
    mpSetEqual(modExpTmpExponent, eQ, nbPrimeDigits);
    mpSetZero(modExpTmpModulo, nbMessageDigits);
    mpSetEqual(modExpTmpModulo, q, nbPrimeDigits);
    mpModExp(modExpResultTwo, x, eQ, q, nbPrimeDigits);

    mpSubtract(modExpResultsDifference, modExpResultTwo, modExpResultOne, nbPrimeDigits);
    if (mpCompare(modExpResultTwo, modExpResultOne, nbPrimeDigits) < 0)
        mpAdd(modExpResultsDifference, modExpResultsDifference, q, nbPrimeDigits);
    printf("-----| modExpResultsDifference\n");
    mpPrintDecimal("", modExpResultsDifference, nbPrimeDigits, "");
    printf("\n");

    mpModMult(u, modExpResultsDifference, garnerConstant, q, nbPrimeDigits);
    printf("-----| u\n");
    mpPrintDecimal("", u, nbPrimeDigits, "");
    printf("\n");
    mpMultiply(up, u, p, nbPrimeDigits);
    printf("-----| up\n");
    mpPrintDecimal("", up, 2*nbPrimeDigits, "");
    printf("\n");

    mpAdd(result, modExpResultOne, up, 2*nbPrimeDigits);
    printf("-----| result\n");
    mpPrintDecimal("", result, 2*nbPrimeDigits, "");
    printf("\n");

    printf("\n\n");
}

void montModExp(digit_t *result, digit_t *x, digit_t *e, digit_t *m, size_t nbDigits) {
    digit_t base[1];
    digit_t baseShifted[nbDigits+1];

    base[0] = 2;

    printf("----> Montgomery\n");

    mpShiftLeft(baseShifted, base, nbDigits*sizeof(digit_t)*8-1, nbDigits+1);
    mpPrintNL(baseShifted, nbDigits+1);
}

void montModMultiply(unsigned char *m, unsigned char *a, unsigned char *b, unsigned char *n, unsigned char nPrime, size_t nbChars) {
    unsigned short i;
    unsigned short j;
    unsigned short t;
    unsigned short tmp;

    unsigned char c;
    unsigned char u[nbChars+1];

    unsigned char n0 = n[0];
    unsigned char b0 = b[0];

    for (i = 0; i < nbChars; i++) {
        for (j = 0; j < i; j++) {
            tmp = (unsigned char)(tmp) + a[j]*b[i-j];

            t = t + (tmp >> 8);

            tmp = (unsigned char)(tmp) + m[j]*n[i-j];
            t = t + (tmp >> 8);
        }

        tmp = (unsigned char)(tmp) + a[i]*b0;
        t = t + (tmp >> 8);

        c = (unsigned char)(tmp)*nPrime;
        m[i] = c;

        tmp = (unsigned char)(tmp) + c*n0;
        tmp = t + (tmp >> 8);
        t = tmp >> 8;
    }

    for (i = nbChars; i < 2*nbChars; i++) {
        for (j = i-nbChars+1; j < nbChars; j++) {
            tmp = (unsigned char)(tmp)  + a[j]*b[i-j];
            t = t + (tmp >> 8);

            tmp = (unsigned char)(tmp) + m[j]*n[i-j];
            t = t + (tmp >> 8);
        }

        m[i-nbChars] = (unsigned char)(tmp);
        tmp = (unsigned char)(t);
        t = t >> 8;
    }

    m[nbChars] = tmp;
    tmp = m[0] - n[0];

    for (i=0; i < nbChars; i++) {
        tmp = m[i] - n[i] - (tmp >> 8);
        u[i] = tmp;
    }

    if ((tmp >> 8) == 0) {
        for (i = 0; i < nbChars; i++) {
            m[i] = u[i];
        }
    }
}
