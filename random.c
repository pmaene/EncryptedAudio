#include "random.h"

int main() {
	mpPrintNL(getRandomDigit(),1);
	return 1;
}

digit_t getRandomDigit() {
	int randomData = open("/dev/random", O_RDONLY);
	digit_t myRandomDigit;
	read(randomData, &myRandomDigit, sizeof myRandomDigit);
	// you now have a random integer!
	close(randomData);
	return myRandomDigit;
}
