#include "random.h"

void getRandomDigit(digit_t *randomDigit, size_t nbDigits) {
	unsigned char tmp[nbDigits*sizeof(digit_t)];

	digit_t randomData;

    memset(tmp, 0, nbDigits*sizeof(digit_t));

	randomData = open("/dev/urandom", O_RDONLY);
	read(randomData, &tmp, nbDigits*sizeof(digit_t));
	close(randomData);

	mpConvFromOctets(randomDigit, nbDigits, tmp, sizeof(digit_t)*nbDigits);
}
