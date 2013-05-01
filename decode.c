#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "codec.h"
#include "decode.h"
#include "functions.h"

void decode(struct decode_chunk_struct * chunk_left, struct decode_chunk_struct * chunk_right, short encoded[BUFFERSIZE], short decoded_0[BUFFERSIZE])
{
    short i, j;

    short decoded_left1a[BUFFERSIZE/4];
    short decoded_left1b[BUFFERSIZE/4];
    short decoded_right1a[BUFFERSIZE/4];
    short decoded_right1b[BUFFERSIZE/4];
    short encoded_tmp[BUFFERSIZE];

    short filter_even[FLENGTH/2] = SYNTHESISFILTER_EVEN;
    short filter_odd[FLENGTH/2] = SYNTHESISFILTER_ODD;
    int even;
    int odd;
    int mean;

    /********************/
    /** Bit Degrouping **/
    /********************/
    encoded_tmp[0] = (encoded[0] >> 11) + MIN_LEVEL_1;
    encoded_tmp[1] = (encoded[0] >> 8) + MIN_LEVEL_2;
    encoded_tmp[2] = (encoded[0] >> 6) + MIN_LEVEL_3;
    encoded_tmp[3] = (encoded[0] >> 4) + MIN_LEVEL_4;
    encoded_tmp[6] = (encoded[0] >> 2) + MIN_LEVEL_3;
    encoded_tmp[7] = encoded[0] + MIN_LEVEL_4;

    encoded_tmp[4] = (encoded[1] >> 11) + MIN_LEVEL_1;
    encoded_tmp[5] = (encoded[1] >> 8) + MIN_LEVEL_2;
    encoded_tmp[8] = (encoded[1] >> 3) + MIN_LEVEL_1;
    encoded_tmp[9] = encoded[1] + MIN_LEVEL_2;

    encoded_tmp[10] = (encoded[2] >> 14) + MIN_LEVEL_3;
    encoded_tmp[11] = (encoded[2] >> 12) + MIN_LEVEL_4;
    encoded_tmp[12] = (encoded[2] >> 7) + MIN_LEVEL_1;
    encoded_tmp[13] = (encoded[2] >> 4) + MIN_LEVEL_2;
    encoded_tmp[14] = (encoded[2] >> 2) + MIN_LEVEL_3;
    encoded_tmp[15] = encoded[2] + MIN_LEVEL_4;

    encoded_tmp[16] = (encoded[3] >> 11) + MIN_LEVEL_1;
    encoded_tmp[17] = (encoded[3] >> 8) + MIN_LEVEL_2;
    encoded_tmp[18] = (encoded[3] >> 6) + MIN_LEVEL_3;
    encoded_tmp[19] = (encoded[3] >> 4) + MIN_LEVEL_4;
    encoded_tmp[22] = (encoded[3] >> 2) + MIN_LEVEL_3;
    encoded_tmp[23] = encoded[3] + MIN_LEVEL_4;

    encoded_tmp[20] = (encoded[4] >> 11) + MIN_LEVEL_1;
    encoded_tmp[21] = (encoded[4] >> 8) + MIN_LEVEL_2;
    encoded_tmp[24] = (encoded[4] >> 3) + MIN_LEVEL_1;
    encoded_tmp[25] = encoded[4] + MIN_LEVEL_2;

    encoded_tmp[26] = (encoded[5] >> 14) + MIN_LEVEL_3;
    encoded_tmp[27] = (encoded[5] >> 12) + MIN_LEVEL_4;
    encoded_tmp[28] = (encoded[5] >> 7) + MIN_LEVEL_1;
    encoded_tmp[29] = (encoded[5] >> 4) + MIN_LEVEL_2;
    encoded_tmp[30] = (encoded[5] >> 2) + MIN_LEVEL_3;
    encoded_tmp[31] = encoded[5] + MIN_LEVEL_4;

    encoded_tmp[32] = (encoded[6] >> 11) + MIN_LEVEL_1;
    encoded_tmp[33] = (encoded[6] >> 8) + MIN_LEVEL_2;
    encoded_tmp[34] = (encoded[6] >> 6) + MIN_LEVEL_3;
    encoded_tmp[35] = (encoded[6] >> 4) + MIN_LEVEL_4;
    encoded_tmp[38] = (encoded[6] >> 2) + MIN_LEVEL_3;
    encoded_tmp[39] = encoded[6] + MIN_LEVEL_4;

    encoded_tmp[36] = (encoded[7] >> 11) + MIN_LEVEL_1;
    encoded_tmp[37] = (encoded[7] >> 8) + MIN_LEVEL_2;

    for (i = 0 ; i <= BUFFERSIZE/16 ; i++) {
        encoded_tmp[16*i] = ((encoded[3*i] & 0xF800) >> 11) + MIN_LEVEL_1;
        encoded_tmp[16*i + 1] = ((encoded[3*i] & 0x700) >> 8) + MIN_LEVEL_2;
        encoded_tmp[16*i + 2] = ((encoded[3*i] & 0xC0) >> 6) + MIN_LEVEL_3;
        encoded_tmp[16*i + 3] = ((encoded[3*i] & 0x30) >> 4) + MIN_LEVEL_4;

        if (3*i + 2 < 40*3/16) {
            encoded_tmp[16*i + 4] = ((encoded[3*i + 1] & 0xF800) >> 11) + MIN_LEVEL_1;
            encoded_tmp[16*i + 5] = ((encoded[3*i + 1] & 0x700) >> 8) + MIN_LEVEL_2;
            encoded_tmp[16*i + 8] = ((encoded[3*i + 1] & 0xF8) >> 3) + MIN_LEVEL_1;
            encoded_tmp[16*i + 9] = (encoded[3*i + 1] & 0x7) + MIN_LEVEL_2;

            encoded_tmp[16*i + 10] = ((encoded[3*i + 2] & 0xc000) >> 14) + MIN_LEVEL_3;
            encoded_tmp[16*i + 11] = ((encoded[3*i + 2] & 0x3000) >> 12) + MIN_LEVEL_4;
        }
    }

    
    for (i = 0 ; i < BUFFERSIZE/8 ; i++) {
        for (j = QLENGTH - 1 ; j > 0 ; j--) {
            chunk_left->diff_deq[0][j] = chunk_left->diff_deq[0][j - 1]; // Subband 1 Left
            chunk_left->diff_deq[1][j] = chunk_left->diff_deq[1][j - 1]; // Subband 2 Left
            chunk_left->diff_deq[2][j] = chunk_left->diff_deq[2][j - 1]; // Subband 3 Left
            chunk_left->diff_deq[3][j] = chunk_left->diff_deq[3][j - 1]; // Subband 4 Left
            chunk_right->diff_deq[0][j] = chunk_right->diff_deq[0][j - 1]; // Subband 1 Right 
            chunk_right->diff_deq[1][j] = chunk_right->diff_deq[1][j - 1]; // Subband 2 Right
            chunk_right->diff_deq[2][j] = chunk_right->diff_deq[2][j - 1]; // Subband 3 Right
            chunk_right->diff_deq[3][j] = chunk_right->diff_deq[3][j - 1]; // Subband 4 Right
        }

        /*******************************/
        /** Decoding Subband One Left **/
        /*******************************/
        chunk_left->diff_deq[0][0] = (short) (encoded_tmp[8*i] * chunk_left->Qstep[0]);

        mean = meanAbs(QLENGTH, chunk_left->diff_deq[0]);

        chunk_left->Qstep[0] = (short) (mean * PHI_1 >> 13);
        if (chunk_left->Qstep[0] < QMIN) {
            chunk_left->Qstep[0] = QMIN;
        } else if (chunk_left->Qstep[0] > QMAX_1) {
            chunk_left->Qstep[0] = QMAX_1;
        }

        encoded_tmp[8*i] = (short) (chunk_left->diff_deq[0][0] + chunk_left->prediction[0]);
        chunk_left->prediction[0] = (short)((MU_1*(encoded_tmp[8*i])) >> 15);

        /*******************************/
        /** Decoding Subband Two Left **/
        /*******************************/
        chunk_left->diff_deq[1][0] = (short) (encoded_tmp[8*i+1] * chunk_left->Qstep[1]);

        mean = meanAbs(QLENGTH, chunk_left->diff_deq[1]);

        chunk_left->Qstep[1] = (short) (mean * PHI_2 >> 13);
        if (chunk_left->Qstep[1] < QMIN) {
            chunk_left->Qstep[1] = QMIN;
        } else if (chunk_left->Qstep[1] > QMAX_2) {
            chunk_left->Qstep[1] = QMAX_2;
        }

        encoded_tmp[8*i+1] = (short) (chunk_left->diff_deq[1][0] + chunk_left->prediction[1]);
        chunk_left->prediction[1] = (short)((MU_2*(encoded_tmp[8*i+1])) >> 15);

        /*********************************/
        /** Decoding Subband Three Left **/
        /*********************************/
        chunk_left->diff_deq[2][0] = (short) (encoded_tmp[8*i+2] * chunk_left->Qstep[2]);

        mean = meanAbs(QLENGTH, chunk_left->diff_deq[2]);

        chunk_left->Qstep[2] = (short) (mean * PHI_3 >> 13);
        if (chunk_left->Qstep[2] < QMIN) {
            chunk_left->Qstep[2] = QMIN;
        } else if (chunk_left->Qstep[2] > QMAX_3) {
            chunk_left->Qstep[2] = QMAX_3;
        }

        encoded_tmp[8*i+2] = (short) (chunk_left->diff_deq[2][0] + chunk_left->prediction[2]);
        chunk_left->prediction[2] = (short)((MU_3*(encoded_tmp[8*i+2])) >> 15);

        /********************************/
        /** Decoding Subband Four Left **/
        /********************************/
        chunk_left->diff_deq[3][0] = (short) (encoded_tmp[8*i+3] * chunk_left->Qstep[3]);

        mean = meanAbs(QLENGTH, chunk_left->diff_deq[3]);

        chunk_left->Qstep[3] = (short) (mean * PHI_4 >> 13);
        if (chunk_left->Qstep[3] < QMIN) {
            chunk_left->Qstep[3] = QMIN;
        } else if (chunk_left->Qstep[3] > QMAX_4) {
            chunk_left->Qstep[3] = QMAX_4;
        }

        encoded_tmp[8*i+3] = (short) (chunk_left->diff_deq[3][0] + chunk_left->prediction[3]);
        chunk_left->prediction[3] = (short)((MU_4*(encoded_tmp[8*i+3])) >> 15);

        /********************************/
        /** Decoding Subband One Right **/
        /********************************/
        chunk_right->diff_deq[0][0] = (short) (encoded_tmp[8*i+4] * chunk_right->Qstep[0]);

        mean = meanAbs(QLENGTH, chunk_right->diff_deq[0]);

        chunk_right->Qstep[0] = (short) (mean * PHI_1 >> 13);
        if (chunk_right->Qstep[0] < QMIN) {
            chunk_right->Qstep[0] = QMIN;
        } else if (chunk_right->Qstep[0] > QMAX_1) {
            chunk_right->Qstep[0] = QMAX_1;
        }

        encoded_tmp[8*i+4] = (short) (chunk_right->diff_deq[0][0] + chunk_right->prediction[0]);
        chunk_right->prediction[0] = (short)((MU_1*(encoded_tmp[8*i+4])) >> 15);

        /********************************/
        /** Decoding Subband Two Right **/
        /********************************/
        chunk_right->diff_deq[1][0] = (short) (encoded_tmp[8*i+5] * chunk_right->Qstep[1]);

        mean = meanAbs(QLENGTH, chunk_right->diff_deq[1]);

        chunk_right->Qstep[1] = (short) (mean * PHI_2 >> 13);
        if (chunk_right->Qstep[1] < QMIN) {
            chunk_right->Qstep[1] = QMIN;
        } else if (chunk_right->Qstep[1] > QMAX_2) {
            chunk_right->Qstep[1] = QMAX_2;
        }

        encoded_tmp[8*i+5] = (short) (chunk_right->diff_deq[1][0] + chunk_right->prediction[1]);
        chunk_right->prediction[1] = (short)((MU_2*(encoded_tmp[8*i+5])) >> 15);

        /**********************************/
        /** Decoding Subband Three Right **/
        /**********************************/
        chunk_right->diff_deq[2][0] = (short) (encoded_tmp[8*i+6] * chunk_right->Qstep[2]);

        mean = meanAbs(QLENGTH, chunk_right->diff_deq[2]);

        chunk_right->Qstep[2] = (short) (mean * PHI_3 >> 13);
        if (chunk_right->Qstep[2] < QMIN) {
            chunk_right->Qstep[2] = QMIN;
        } else if (chunk_right->Qstep[2] > QMAX_3) {
            chunk_right->Qstep[2] = QMAX_3;
        }

        encoded_tmp[8*i+6] = (short) (chunk_right->diff_deq[2][0] + chunk_right->prediction[2]);
        chunk_right->prediction[2] = (short)((MU_3*(encoded_tmp[8*i+6])) >> 15);

        /*********************************/
        /** Decoding Subband Four Right **/
        /*********************************/
        chunk_right->diff_deq[3][0] = (short) (encoded_tmp[8*i+7] * chunk_right->Qstep[3]);

        mean = meanAbs(QLENGTH, chunk_right->diff_deq[3]);

        chunk_right->Qstep[3] = (short) (mean * PHI_4 >> 13);
        if (chunk_right->Qstep[3] < QMIN) {
            chunk_right->Qstep[3] = QMIN;
        } else if (chunk_right->Qstep[3] > QMAX_4) {
            chunk_right->Qstep[3] = QMAX_4;
        }

        encoded_tmp[8*i+7] = (short) (chunk_right->diff_deq[3][0] + chunk_right->prediction[3]);
        chunk_right->prediction[3] = (short)((MU_4*(encoded_tmp[8*i+7])) >> 15);
    }


    /*************************/
    /** Synthesis Stage Two **/
    /*************************/
    for (i = 0 ; i < BUFFERSIZE/8 ; i++) {
        chunk_left->t1_subband_2a[i + FLENGTH/2 - 1] = encoded_tmp[8*i] + encoded_tmp[8*i+1];
        chunk_left->t2_subband_2a[i + FLENGTH/2 - 1] = encoded_tmp[8*i] - encoded_tmp[8*i+1];
        chunk_left->t1_subband_2c[i + FLENGTH/2 - 1] = encoded_tmp[8*i+2] + encoded_tmp[8*i+3];
        chunk_left->t2_subband_2c[i + FLENGTH/2 - 1] = encoded_tmp[8*i+2] - encoded_tmp[8*i+3];
        chunk_right->t1_subband_2a[i + FLENGTH/2 - 1] = encoded_tmp[8*i+4] + encoded_tmp[8*i+5];
        chunk_right->t2_subband_2a[i + FLENGTH/2 - 1] = encoded_tmp[8*i+4] - encoded_tmp[8*i+5];
        chunk_right->t1_subband_2c[i + FLENGTH/2 - 1] = encoded_tmp[8*i+6] + encoded_tmp[8*i+7];
        chunk_right->t2_subband_2c[i + FLENGTH/2 - 1] = encoded_tmp[8*i+6] - encoded_tmp[8*i+7];
    }

    for (i = 0 ; i < BUFFERSIZE/8 ; i++) {
        conv2(chunk_left->t1_subband_2a, filter_even, &even, i + FLENGTH/2 - 1);
        conv2(chunk_left->t2_subband_2a, filter_odd, &odd, i + FLENGTH/2 - 1);

        decoded_left1a[2*i] = even >> 14;
        decoded_left1a[2*i + 1] = odd >> 14;

        conv2(chunk_left->t1_subband_2c, filter_even, &even, i + FLENGTH/2 - 1);
        conv2(chunk_left->t2_subband_2c, filter_odd, &odd, i + FLENGTH/2 - 1);

        decoded_left1b[2*i] = even >> 14;
        decoded_left1b[2*i + 1] = odd >> 14;

        conv2(chunk_right->t1_subband_2a, filter_even, &even, i + FLENGTH/2 - 1);
        conv2(chunk_right->t2_subband_2a, filter_odd, &odd, i + FLENGTH/2 - 1);

        decoded_right1a[2*i] = even >> 14;
        decoded_right1a[2*i + 1] = odd >> 14;

        conv2(chunk_right->t1_subband_2c, filter_even, &even, i + FLENGTH/2 - 1);
        conv2(chunk_right->t2_subband_2c, filter_odd, &odd, i + FLENGTH/2 - 1);

        decoded_right1b[2*i] = even >> 14;
        decoded_right1b[2*i + 1] = odd >> 14;
    }

    for (i = 0 ; i < FLENGTH/2 - 1 ; i++) {
        chunk_left->t1_subband_2a[i] = chunk_left->t1_subband_2a[BUFFERSIZE/8 + i];
        chunk_left->t2_subband_2a[i] = chunk_left->t2_subband_2a[BUFFERSIZE/8 + i];

        chunk_left->t1_subband_2c[i] = chunk_left->t1_subband_2c[BUFFERSIZE/8 + i];
        chunk_left->t2_subband_2c[i] = chunk_left->t2_subband_2c[BUFFERSIZE/8 + i];

        chunk_right->t1_subband_2a[i] = chunk_right->t1_subband_2a[BUFFERSIZE/8 + i];
        chunk_right->t2_subband_2a[i] = chunk_right->t2_subband_2a[BUFFERSIZE/8 + i];

        chunk_right->t1_subband_2c[i] = chunk_right->t1_subband_2c[BUFFERSIZE/8 + i];
        chunk_right->t2_subband_2c[i] = chunk_right->t2_subband_2c[BUFFERSIZE/8 + i];
    }

    /*************************/
    /** Synthesis Stage One **/
    /*************************/
    for (i = 0 ; i < BUFFERSIZE/4 ; i++) {
        chunk_left->t1_subband_1[i + FLENGTH/2 - 1] = decoded_left1a[i] + decoded_left1b[i];
        chunk_left->t2_subband_1[i + FLENGTH/2 - 1] = decoded_left1a[i] - decoded_left1b[i];
        chunk_right->t1_subband_1[i + FLENGTH/2 - 1] = decoded_right1a[i] + decoded_right1b[i];
        chunk_right->t2_subband_1[i + FLENGTH/2 - 1] = decoded_right1a[i] - decoded_right1b[i];
    }

    for (i = 0 ; i < BUFFERSIZE/4 ; i++) {
        conv1(chunk_left->t1_subband_1, filter_even, &even, i + FLENGTH/2 - 1);
        conv1(chunk_left->t2_subband_1, filter_odd, &odd, i + FLENGTH/2 - 1);

        decoded_0[4*i] = even >> 14;
        decoded_0[4*i + 2] = odd >> 14;

        conv1(chunk_right->t1_subband_1, filter_even, &even, i + FLENGTH/2 - 1);
        conv1(chunk_right->t2_subband_1, filter_odd, &odd, i + FLENGTH/2 - 1);

        decoded_0[4*i + 1] = even >> 14;
        decoded_0[4*i + 3] = odd >> 14;
    }

    for (i = 0 ; i < FLENGTH/2 - 1 ; i++) {
        chunk_left->t1_subband_1[i] = chunk_left->t1_subband_1[BUFFERSIZE/4 + i];
        chunk_left->t2_subband_1[i] = chunk_left->t2_subband_1[BUFFERSIZE/4 + i];
        chunk_right->t1_subband_1[i] = chunk_right->t1_subband_1[BUFFERSIZE/4 + i];
        chunk_right->t2_subband_1[i] = chunk_right->t2_subband_1[BUFFERSIZE/4 + i];
    }

    return;
}
