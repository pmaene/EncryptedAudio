#include "random.h"

void getRandomDigit(digit_t *randomDigit) {
	size_t i;
	unsigned char tmp[ENC_DH_SECRET_DIGITS*sizeof(digit_t)];

	memset(tmp, 0, ENC_DH_SECRET_DIGITS*sizeof(digit_t));

	for (i = 0; i < ENC_DH_SECRET_DIGITS*sizeof(digit_t); i++)
		tmp[i] = rand();

	mpConvFromOctets(randomDigit, ENC_DH_SECRET_DIGITS, tmp, ENC_DH_SECRET_DIGITS*sizeof(digit_t));
}
