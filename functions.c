#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "codec.h"
#include "functions.h"

/*********************************/
/** Mean of the absolute values **/
/*********************************/
int meanAbs(int w, short *a) {
    int sum = 0;
    int i = 0;

    for (i = 0 ; i < w ; i++) {
    	sum += abs(a[i]);
    }

    return roundDiv(sum, w);
}

/**********************/
/** Divide and round **/
/**********************/
int roundDiv(int n, int d) {
    div_t temp = div(n, d);

    if((temp.rem << 1) > d && temp.quot > 0) {
        return temp.quot + 1;
    } else if (-(temp.rem << 1) > d && temp.quot < 0) {
        return temp.quot - 1;
    } else {
        return temp.quot;
    }
}

/******************************/
/** Convolution (first step) **/
/******************************/
void conv1(short buffer[BUFFERSIZE_4 + FLENGTH_2 - 1], short filter[FLENGTH_2], int *result, short n) {
    short k;
    *result = 0;

    for (k = 0; k < FLENGTH_2; k++) {
        *result += buffer[n-k]*filter[k];
    }
}

/*******************************/
/** Convolution (second step) **/
/*******************************/
void conv2(short buffer[BUFFERSIZE_8 + FLENGTH_2 - 1], short filter[FLENGTH_2], int *result, short n) {
    short k;
    *result = 0;

    for (k = 0; k < FLENGTH_2; k++) {
        *result += buffer[n-k]*filter[k];
    }
}
