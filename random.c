#include "random.h"

int main() {
	mpPrintNL(getRandomDigit(64),64/4);
	printf("\n");
	exit(1);
}

digit_t getRandomDigit(ndigits) {
	int randomData;
	int i;
	int temp[ndigits];
	digit_t myRandomDigit[ndigits];
	for(i=0;i<ndigits;i++) {
		temp[i]=0;
	}
	randomData = open("/dev/urandom", O_RDONLY);
	read(randomData, &temp, ndigits);
	close(randomData);

	for(i=0;i<ndigits/4;i++) {
		//printf("---|temp%d\n",i);
		printf("%x ",temp[i]);
	}
	printf("\n\n");
	mpConvFromOctets(myRandomDigit, ndigits/4, temp, ndigits);	

	return myRandomDigit;
}
