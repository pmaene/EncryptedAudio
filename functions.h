int meanAbs(int w, short *a);

int roundDiv(int n, int d);

void conv1(short buffer[BUFFERSIZE_4 + FLENGTH_2 - 1], short filter[FLENGTH_2], int *result, short n);

void conv2(short buffer[BUFFERSIZE_8 + FLENGTH_2 - 1], short filter[FLENGTH_2], int *result, short n);
