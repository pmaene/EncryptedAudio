#include "random.h"

void getRandomDigit(digit_t *randomDigit, size_t nbDigits) {
	unsigned char tmp[sizeof(digit_t)*nbDigits];
	unsigned short i;
	digit_t randomData;

	for(i = 0; i < sizeof(digit_t)*nbDigits; i++)
		tmp[i] = 0;

	randomData = open("/dev/urandom", O_RDONLY);
	read(randomData, &tmp, nbDigits*sizeof(digit_t));
	close(randomData);

	mpConvFromOctets(randomDigit, nbDigits, tmp, sizeof(digit_t)*nbDigits);
}
