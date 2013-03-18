#include "random.h"


int main() {
	digit_t randomDigit[64];
	getRandomDigit(randomDigit, 64);
	mpPrintNL(randomDigit, 64/4);
	printf("\n");
	exit(1);
}

void getRandomDigit(digit_t *randomDigit, int ndigits) {
	int randomData;
	int i;
	unsigned char temp[ndigits*4];
	for(i=0;i<ndigits*4;i++) {
		temp[i]=0;
	}
	randomData = open("/dev/urandom", O_RDONLY);
	read(randomData, &temp, ndigits*4);
	close(randomData);

	for(i=0;i<ndigits-3;i++) {
		printf("%x",temp[i]);
		printf("%x",temp[i+1]);
		printf("%x",temp[i+2]);
		printf("%x ",temp[i+3]);
		i = i + 3;
	}
	printf("\n\n");
	mpConvFromOctets(randomDigit, ndigits/4, temp, ndigits);
}
