#include "crt.h"

struct threadArguments {
   int id;

   digit_t *x;
   digit_t *exponent;
   digit_t *modulus;
   digit_t *result;
};

struct threadArguments argumentsArray[2];

void *_crtModExp_HalfExp(void *arguments);

void crtModExp(digit_t *result, digit_t *x, digit_t *e, digit_t *p, digit_t *q) {
    void *status;

    digit_t one[ENC_SIGN_PRIME_DIGITS];

    digit_t pMinusOne[ENC_SIGN_PRIME_DIGITS];
    digit_t eP[ENC_SIGN_PRIME_DIGITS];
    digit_t qMinusOne[ENC_SIGN_PRIME_DIGITS];
    digit_t eQ[ENC_SIGN_PRIME_DIGITS];

    digit_t modExpResultOne[ENC_SIGNATURE_DIGITS];
    digit_t modExpResultTwo[ENC_SIGNATURE_DIGITS];

    digit_t modExpResultsDifference[ENC_SIGN_PRIME_DIGITS];
    digit_t garnerConstant[ENC_SIGN_PRIME_DIGITS];

    digit_t u[ENC_SIGN_PRIME_DIGITS];
    digit_t up[2*ENC_SIGN_PRIME_DIGITS];

    pthread_t threads[2];
    pthread_attr_t threadAttributes;

    mpSetZero(one, ENC_SIGN_PRIME_DIGITS);
    *one = 1;

    mpSubtract(pMinusOne, p, one, ENC_SIGN_PRIME_DIGITS);
    mpModulo(eP, e, ENC_PRIVATE_KEY_DIGITS, pMinusOne, ENC_SIGN_PRIME_DIGITS);
    mpSubtract(qMinusOne, q, one, ENC_SIGN_PRIME_DIGITS);
    mpModulo(eQ, e, ENC_PRIVATE_KEY_DIGITS, qMinusOne, ENC_SIGN_PRIME_DIGITS);

    argumentsArray[0].id = 0;
    argumentsArray[0].x = x;
    argumentsArray[0].exponent = eP;
    argumentsArray[0].modulus = p;
    argumentsArray[0].result = modExpResultOne;

    argumentsArray[1].id = 1;
    argumentsArray[1].x = x;
    argumentsArray[1].exponent = eQ;
    argumentsArray[1].modulus = q;
    argumentsArray[1].result = modExpResultTwo;

    pthread_attr_init(&threadAttributes);
    pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], NULL, _crtModExp_HalfExp, (void *) &argumentsArray[0]);
    pthread_create(&threads[1], NULL, _crtModExp_HalfExp, (void *) &argumentsArray[1]);

    pthread_attr_destroy(&threadAttributes);
    pthread_join(threads[0], &status);
    pthread_join(threads[1], &status);

    printf("----> Threads Joined\n");

    printf("----| modExpResultOne\n");
    mpPrintNL(modExpResultOne, ENC_SIGNATURE_DIGITS);
    printf("----| modExpResultTwo\n");
    mpPrintNL(modExpResultTwo, ENC_SIGNATURE_DIGITS);

    mpSubtract(modExpResultsDifference, modExpResultTwo, modExpResultOne, ENC_SIGN_PRIME_DIGITS);
    if (mpCompare(modExpResultTwo, modExpResultOne, ENC_SIGNATURE_DIGITS) < 0)
        mpAdd(modExpResultsDifference, modExpResultsDifference, q, ENC_SIGN_PRIME_DIGITS);

    mpModInv(garnerConstant, p, q, ENC_SIGN_PRIME_DIGITS);
    mpModMult(u, modExpResultsDifference, garnerConstant, q, ENC_SIGN_PRIME_DIGITS);
    mpMultiply(up, u, p, ENC_SIGN_PRIME_DIGITS);
    mpAdd(result, modExpResultOne, up, 2*ENC_SIGN_PRIME_DIGITS);
}

void *_crtModExp_HalfExp(void *arguments) {
    digit_t modExpTmpExponent[ENC_SIGNATURE_DIGITS];
    digit_t modExpTmpModulo[ENC_SIGNATURE_DIGITS];

    struct threadArguments *threadArguments;

    threadArguments = (struct threadArguments *) arguments;

    printf("----> Starting Thread %d\n", threadArguments->id);

    mpSetZero(modExpTmpExponent, ENC_SIGNATURE_DIGITS);
    mpSetEqual(modExpTmpExponent, threadArguments->exponent, ENC_SIGN_PRIME_DIGITS);
    mpSetZero(modExpTmpModulo, ENC_SIGNATURE_DIGITS);
    mpSetEqual(modExpTmpModulo, threadArguments->modulus, ENC_SIGN_PRIME_DIGITS);
    mpModExp(threadArguments->result, threadArguments->x, modExpTmpExponent, modExpTmpModulo, ENC_SIGNATURE_DIGITS);

    pthread_exit(NULL);
}
