#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "codec.h"
#include "encode.h"
#include "functions.h"

void encode(short buffer[BUFFERSIZE], struct encode_chunk_struct * chunk_left, struct encode_chunk_struct * chunk_right, short encoded[BUFFERSIZE])
{
    short i, j, e, a;
    short encoded_tmp[BUFFERSIZE];

    short filter_even[FLENGTH_2] = ANALYSISFILTER_EVEN;
    short filter_odd[FLENGTH_2] = ANALYSISFILTER_ODD;
    int t1;
    int t2;

    /**************/
    /** Analysis **/
    /**************/
    // First shift buffers
    for (i = 0 ; i < FLENGTH_2 - 1 ; i++) {
        chunk_left->even_subband_1[i] = chunk_left->even_subband_1[BUFFERSIZE_4 + i];
        chunk_left->odd_subband_1[i] = chunk_left->odd_subband_1[BUFFERSIZE_4 + i];

        chunk_right->even_subband_1[i] = chunk_right->even_subband_1[BUFFERSIZE_4 + i];
        chunk_right->odd_subband_1[i] = chunk_right->odd_subband_1[BUFFERSIZE_4 + i];

        chunk_left->even_subband_2a[i] = chunk_left->even_subband_2a[BUFFERSIZE_8 + i];
        chunk_left->odd_subband_2a[i] = chunk_left->odd_subband_2a[BUFFERSIZE_8 + i];

        chunk_left->even_subband_2c[i] = chunk_left->even_subband_2c[BUFFERSIZE_8 + i];
        chunk_left->odd_subband_2c[i] = chunk_left->odd_subband_2c[BUFFERSIZE_8 + i];

        chunk_right->even_subband_2a[i] = chunk_right->even_subband_2a[BUFFERSIZE_8 + i];
        chunk_right->odd_subband_2a[i] = chunk_right->odd_subband_2a[BUFFERSIZE_8 + i];

        chunk_right->even_subband_2c[i] = chunk_right->even_subband_2c[BUFFERSIZE_8 + i];
        chunk_right->odd_subband_2c[i] = chunk_right->odd_subband_2c[BUFFERSIZE_8 + i];
    }

    chunk_left->even_subband_1[FLENGTH_2 - 1] = buffer[0];
    chunk_left->odd_subband_1[FLENGTH_2 - 1] = chunk_left->odd_1_lastvalue;

    chunk_right->even_subband_1[FLENGTH_2 - 1] = buffer[1];
    chunk_right->odd_subband_1[FLENGTH_2 - 1] = chunk_right->odd_1_lastvalue;

    for (i = 1 ; i < BUFFERSIZE_4 ; i++) {
        e = 4*i;
        chunk_left->even_subband_1[i + FLENGTH_2 - 1] = buffer[e];
        chunk_left->odd_subband_1[i + FLENGTH_2 - 1] = buffer[e - 2];

        chunk_right->even_subband_1[i + FLENGTH_2 - 1] = buffer[e + 1];
        chunk_right->odd_subband_1[i + FLENGTH_2 - 1] = buffer[e - 1];
    }

    chunk_left->odd_1_lastvalue = buffer[BUFFERSIZE - 2];
    chunk_right->odd_1_lastvalue = buffer[BUFFERSIZE - 1];

    // Stage One: loop for i = 0
    conv1(chunk_left->even_subband_1, filter_even, &t1, FLENGTH_2 - 1);
	conv1(chunk_left->odd_subband_1, filter_odd, &t2, FLENGTH_2 - 1);
    chunk_left->even_subband_2a[FLENGTH_2 - 1] = (t1 + t2) >> 16;
    chunk_left->odd_subband_2a[FLENGTH_2 - 1] = chunk_left->odd_2a_lastvalue;
    chunk_left->even_subband_2c[FLENGTH_2 - 1] = (t2 - t1) >> 16;
    chunk_left->odd_subband_2c[FLENGTH_2 - 1] = chunk_left->odd_2c_lastvalue;

	conv1(chunk_right->even_subband_1, filter_even, &t1, FLENGTH_2 - 1);
	conv1(chunk_right->odd_subband_1, filter_odd, &t2, FLENGTH_2 - 1);
    chunk_right->even_subband_2a[FLENGTH_2 - 1] = (t1 + t2) >> 16;
    chunk_right->odd_subband_2a[FLENGTH_2 - 1] = chunk_right->odd_2a_lastvalue;
    chunk_right->even_subband_2c[FLENGTH_2 - 1] = (t2 - t1) >> 16;
    chunk_right->odd_subband_2c[FLENGTH_2 - 1] = chunk_right->odd_2c_lastvalue;

    for (i = 1 ; i < BUFFERSIZE_4 ; i+=2) {
        // Stage One
        e = (i >> 1) + FLENGTH_2;
        a = i + FLENGTH_2 - 1;
        conv1(chunk_left->even_subband_1, filter_even, &t1, a);
        conv1(chunk_left->odd_subband_1, filter_odd, &t2, a);

		chunk_left->odd_subband_2a[e] = (t1 + t2) >> 16;
		chunk_left->odd_subband_2c[e] = (t2 - t1) >> 16;

        conv1(chunk_right->even_subband_1, filter_even, &t1, a);
        conv1(chunk_right->odd_subband_1, filter_odd, &t2, a);

		chunk_right->odd_subband_2a[e] = (t1 + t2) >> 16;
		chunk_right->odd_subband_2c[e] = (t2 - t1) >> 16;

        a = a + 1;

        conv1(chunk_left->even_subband_1, filter_even, &t1, a);
		conv1(chunk_left->odd_subband_1, filter_odd, &t2, a);

		chunk_left->even_subband_2a[e] = (t1 + t2) >> 16;
		chunk_left->even_subband_2c[e] = (t2 - t1) >> 16;

		conv1(chunk_right->even_subband_1, filter_even, &t1, a);
		conv1(chunk_right->odd_subband_1, filter_odd, &t2, a);

		chunk_right->even_subband_2a[e] = (t1 + t2) >> 16;
		chunk_right->even_subband_2c[e] = (t2 - t1) >> 16;

        // Stage Two
        j = 4*(i - 1);
        e = e - 1;
        conv2(chunk_left->even_subband_2a, filter_even, &t1, e);
        conv2(chunk_left->odd_subband_2a, filter_odd, &t2, e);

        encoded_tmp[j] = (t1 + t2) >> 16;
        encoded_tmp[j+1] = (t2 - t1) >> 16;

        conv2(chunk_left->even_subband_2c, filter_even, &t1, e);
        conv2(chunk_left->odd_subband_2c, filter_odd, &t2, e);

        encoded_tmp[j+2] = (t1 + t2) >> 16;
        encoded_tmp[j+3] = (t2 - t1) >> 16;

        conv2(chunk_right->even_subband_2a, filter_even, &t1, e);
        conv2(chunk_right->odd_subband_2a, filter_odd, &t2, e);

        encoded_tmp[j+4] = (t1 + t2) >> 16;
        encoded_tmp[j+5] = (t2 - t1) >> 16;

        conv2(chunk_right->even_subband_2c, filter_even, &t1, e);
        conv2(chunk_right->odd_subband_2c, filter_odd, &t2, e);

        encoded_tmp[j+6] = (t1 + t2) >> 16;
        encoded_tmp[j+7] = (t2 - t1) >> 16;
    }

    e = ((BUFFERSIZE_4 - 1) >> 1) + FLENGTH_2;
    chunk_left->odd_2a_lastvalue = chunk_left->odd_subband_2a[e];
    chunk_left->odd_2c_lastvalue = chunk_left->odd_subband_2c[e];
    chunk_right->odd_2a_lastvalue = chunk_right->odd_subband_2a[e];
    chunk_right->odd_2c_lastvalue = chunk_right->odd_subband_2c[e];

    for (i = 0 ; i < BUFFERSIZE_8 ; i++) {
        // Shift
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

        /*****************************/
        /** Encoding Subband 1 Left **/
        /*****************************/
        e = 8*i;
        encoded_tmp[e] = roundDiv((encoded_tmp[e] - chunk_left->prediction[0]), chunk_left->Qstep[0]);

        if (encoded_tmp[e] > MAX_LEVEL_1) {
            encoded_tmp[e] = MAX_LEVEL_1;
        } else if (encoded_tmp[e] < MIN_LEVEL_1) {
            encoded_tmp[e] = MIN_LEVEL_1;
        }
        chunk_left->diff_deq[0][0] = (short) (encoded_tmp[e] * chunk_left->Qstep[0]);
        chunk_left->Qstep[0] = (short)((meanAbs(QLENGTH, chunk_left->diff_deq[0]) * PHI_1) >> 13);

        if (chunk_left->Qstep[0] < QMIN) {
            chunk_left->Qstep[0] = QMIN;
        } else if (chunk_left->Qstep[0] > QMAX_1) {
            chunk_left->Qstep[0] = QMAX_1;
        }

        chunk_left->prediction[0] = (short)((MU_1*(chunk_left->diff_deq[0][0] + chunk_left->prediction[0])) >> 15);

        /*****************************/
        /** Encoding Subband 2 Left **/
        /*****************************/
        e += 1;
        encoded_tmp[e] = roundDiv((encoded_tmp[e] - chunk_left->prediction[1]), chunk_left->Qstep[1]);

        if (encoded_tmp[e] > MAX_LEVEL_2) {
            encoded_tmp[e] = MAX_LEVEL_2;
        } else if (encoded_tmp[e] < MIN_LEVEL_2) {
            encoded_tmp[e] = MIN_LEVEL_2;
        }

        chunk_left->diff_deq[1][0] = (short) (encoded_tmp[e] * chunk_left->Qstep[1]);
        chunk_left->Qstep[1] = (short)((meanAbs(QLENGTH, chunk_left->diff_deq[1]) * PHI_2) >> 13);

        if (chunk_left->Qstep[1] < QMIN) {
            chunk_left->Qstep[1] = QMIN;
        } else if (chunk_left->Qstep[1] > QMAX_2) {
            chunk_left->Qstep[1] = QMAX_2;
        }

        chunk_left->prediction[1] = (short)((MU_2*(chunk_left->diff_deq[1][0] + chunk_left->prediction[1])) >> 15);

        /*****************************/
        /** Encoding Subband 3 Left **/
        /*****************************/
        e += 1;
        encoded_tmp[e] = roundDiv((encoded_tmp[e] - chunk_left->prediction[2]), chunk_left->Qstep[2]);

        if (encoded_tmp[e] > MAX_LEVEL_3) {
            encoded_tmp[e] = MAX_LEVEL_3;
        } else if (encoded_tmp[e] < MIN_LEVEL_3) {
            encoded_tmp[e] = MIN_LEVEL_3;
        }

        chunk_left->diff_deq[2][0] = (short) (encoded_tmp[e] * chunk_left->Qstep[2]);
        chunk_left->Qstep[2] = (short)((meanAbs(QLENGTH, chunk_left->diff_deq[2]) * PHI_3) >> 13);

        if (chunk_left->Qstep[2] < QMIN) {
            chunk_left->Qstep[2] = QMIN;
        } else if (chunk_left->Qstep[2] > QMAX_3) {
            chunk_left->Qstep[2] = QMAX_3;
        }

        chunk_left->prediction[2] = (short)((MU_3*(chunk_left->diff_deq[2][0] + chunk_left->prediction[2])) >> 15);

        /*****************************/
        /** Encoding Subband 4 Left **/
        /*****************************/
        e += 1;
        encoded_tmp[e] = roundDiv((encoded_tmp[e] - chunk_left->prediction[3]), chunk_left->Qstep[3]);

        if (encoded_tmp[e] > MAX_LEVEL_4) {
            encoded_tmp[e] = MAX_LEVEL_4;
        } else if (encoded_tmp[e] < MIN_LEVEL_4) {
            encoded_tmp[e] = MIN_LEVEL_4;
        }

        chunk_left->diff_deq[3][0] = (short) (encoded_tmp[e] * chunk_left->Qstep[3]);
        chunk_left->Qstep[3] = (short)((meanAbs(QLENGTH, chunk_left->diff_deq[3]) * PHI_4) >> 13);

        if (chunk_left->Qstep[3] < QMIN) {
            chunk_left->Qstep[3] = QMIN;
        } else if (chunk_left->Qstep[3] > QMAX_4) {
            chunk_left->Qstep[3] = QMAX_4;
        }

        chunk_left->prediction[3] = (short)((MU_4*(chunk_left->diff_deq[3][0] + chunk_left->prediction[3])) >> 15);

        /******************************/
        /** Encoding Subband 1 Right **/
        /******************************/
        e += 1;
        encoded_tmp[e] = roundDiv((encoded_tmp[e] - chunk_right->prediction[0]), chunk_right->Qstep[0]);

        if (encoded_tmp[e] > MAX_LEVEL_1) {
            encoded_tmp[e] = MAX_LEVEL_1;
        } else if (encoded_tmp[e] < MIN_LEVEL_1) {
            encoded_tmp[e] = MIN_LEVEL_1;
        }

        chunk_right->diff_deq[0][0] = (short) (encoded_tmp[e] * chunk_right->Qstep[0]);
        chunk_right->Qstep[0] = (short)((meanAbs(QLENGTH, chunk_right->diff_deq[0]) * PHI_1) >> 13);

        if (chunk_right->Qstep[0] < QMIN) {
            chunk_right->Qstep[0] = QMIN;
        } else if (chunk_right->Qstep[0] > QMAX_1) {
            chunk_right->Qstep[0] = QMAX_1;
        }

        chunk_right->prediction[0] = (short)((MU_1*(chunk_right->diff_deq[0][0] + chunk_right->prediction[0])) >> 15);

        /******************************/
        /** Encoding Subband 2 Right **/
        /******************************/
        e += 1;
        encoded_tmp[e] = roundDiv((encoded_tmp[e] - chunk_right->prediction[1]), chunk_right->Qstep[1]);

        if (encoded_tmp[e] > MAX_LEVEL_2) {
            encoded_tmp[e] = MAX_LEVEL_2;
        } else if (encoded_tmp[e] < MIN_LEVEL_2) {
            encoded_tmp[e] = MIN_LEVEL_2;
        }

        chunk_right->diff_deq[1][0] = (short) (encoded_tmp[e] * chunk_right->Qstep[1]);
        chunk_right->Qstep[1] = (short)((meanAbs(QLENGTH, chunk_right->diff_deq[1]) * PHI_2) >> 13);

        if (chunk_right->Qstep[1] < QMIN) {
            chunk_right->Qstep[1] = QMIN;
        } else if (chunk_right->Qstep[1] > QMAX_2) {
            chunk_right->Qstep[1] = QMAX_2;
        }

        chunk_right->prediction[1] = (short)((MU_2*(chunk_right->diff_deq[1][0] + chunk_right->prediction[1])) >> 15);

        /******************************/
        /** Encoding Subband 3 Right **/
        /******************************/
        e += 1;
        encoded_tmp[e] = roundDiv((encoded_tmp[e] - chunk_right->prediction[2]), chunk_right->Qstep[2]);

        if (encoded_tmp[e] > MAX_LEVEL_3) {
            encoded_tmp[e] = MAX_LEVEL_3;
        } else if (encoded_tmp[e] < MIN_LEVEL_3) {
            encoded_tmp[e] = MIN_LEVEL_3;
        }

        chunk_right->diff_deq[2][0] = (short) (encoded_tmp[e] * chunk_right->Qstep[2]);
        chunk_right->Qstep[2] = (short)((meanAbs(QLENGTH, chunk_right->diff_deq[2]) * PHI_3) >> 13);

        if (chunk_right->Qstep[2] < QMIN) {
            chunk_right->Qstep[2] = QMIN;
        } else if (chunk_right->Qstep[2] > QMAX_3) {
            chunk_right->Qstep[2] = QMAX_3;
        }

        chunk_right->prediction[2] = (short)((MU_3*(chunk_right->diff_deq[2][0] + chunk_right->prediction[2])) >> 15);

        /******************************/
        /** Encoding Subband 4 Right **/
        /******************************/
        e += 1;
        encoded_tmp[e] = roundDiv((encoded_tmp[e] - chunk_right->prediction[3]), chunk_right->Qstep[3]);

        if (encoded_tmp[e] > MAX_LEVEL_4) {
            encoded_tmp[e] = MAX_LEVEL_4;
        } else if (encoded_tmp[e] < MIN_LEVEL_4) {
            encoded_tmp[e] = MIN_LEVEL_4;
        }

        chunk_right->diff_deq[3][0] = (short) (encoded_tmp[e] * chunk_right->Qstep[3]);
        chunk_right->Qstep[3] = (short)((meanAbs(QLENGTH, chunk_right->diff_deq[3]) * PHI_4) >> 13);

        if (chunk_right->Qstep[3] < QMIN) {
            chunk_right->Qstep[3] = QMIN;
        } else if (chunk_right->Qstep[3] > QMAX_4) {
            chunk_right->Qstep[3] = QMAX_4;
        }

        chunk_right->prediction[3] = (short)((MU_4*(chunk_right->diff_deq[3][0] + chunk_right->prediction[3])) >> 15);
    }

    /******************/
    /** Bit Grouping **/
    /******************/
    encoded[0] = ((encoded_tmp[0] - MIN_LEVEL_1) << 11)
                | ((encoded_tmp[1] - MIN_LEVEL_2) << 8)
                | ((encoded_tmp[2] - MIN_LEVEL_3) << 6)
                | ((encoded_tmp[3] - MIN_LEVEL_4) << 4)
                | ((encoded_tmp[6] - MIN_LEVEL_3) << 2)
                | ((encoded_tmp[7] - MIN_LEVEL_4));
    encoded[1] = ((encoded_tmp[4] - MIN_LEVEL_1) << 11)
                | ((encoded_tmp[5] - MIN_LEVEL_2) << 8)
                | ((encoded_tmp[8] - MIN_LEVEL_1) << 3)
                | ((encoded_tmp[9] - MIN_LEVEL_2));
    encoded[2] = ((encoded_tmp[10] - MIN_LEVEL_3) << 14)
                | ((encoded_tmp[11] - MIN_LEVEL_4) << 12)
                | ((encoded_tmp[12] - MIN_LEVEL_1) << 7)
                | ((encoded_tmp[13] - MIN_LEVEL_2) << 4)
                | ((encoded_tmp[14] - MIN_LEVEL_3) << 2)
                | ((encoded_tmp[15] - MIN_LEVEL_4));
    encoded[3] = ((encoded_tmp[16] - MIN_LEVEL_1) << 11)
                | ((encoded_tmp[17] - MIN_LEVEL_2) << 8)
                | ((encoded_tmp[18] - MIN_LEVEL_3) << 6)
                | ((encoded_tmp[19] - MIN_LEVEL_4) << 4)
                | ((encoded_tmp[22] - MIN_LEVEL_3) << 2)
                | ((encoded_tmp[23] - MIN_LEVEL_4));
    encoded[4] = ((encoded_tmp[20] - MIN_LEVEL_1) << 11)
                | ((encoded_tmp[21] - MIN_LEVEL_2) << 8)
                | ((encoded_tmp[24] - MIN_LEVEL_1) << 3)
                | ((encoded_tmp[25] - MIN_LEVEL_2));
    encoded[5] = ((encoded_tmp[26] - MIN_LEVEL_3) << 14)
                | ((encoded_tmp[27] - MIN_LEVEL_4) << 12)
                | ((encoded_tmp[28] - MIN_LEVEL_1) << 7)
                | ((encoded_tmp[29] - MIN_LEVEL_2) << 4)
                | ((encoded_tmp[30] - MIN_LEVEL_3) << 2)
                | ((encoded_tmp[31] - MIN_LEVEL_4));
    encoded[6] = ((encoded_tmp[32] - MIN_LEVEL_1) << 11)
                | ((encoded_tmp[33] - MIN_LEVEL_2) << 8)
                | ((encoded_tmp[34] - MIN_LEVEL_3) << 6)
                | ((encoded_tmp[35] - MIN_LEVEL_4) << 4)
                | ((encoded_tmp[38] - MIN_LEVEL_3) << 2)
                | ((encoded_tmp[39] - MIN_LEVEL_4));
    encoded[7] = ((encoded_tmp[36] - MIN_LEVEL_1) << 11)
                | ((encoded_tmp[37] - MIN_LEVEL_2) << 8);

    return;
}
