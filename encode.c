#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "codec.h"
#include "encode.h"

#define CONV(signal, index, filter) (signal)[(index)]*(filter)[0] + (signal)[(index)-1]*(filter)[1] + (signal)[(index)-2]*(filter)[2] + (signal)[(index)-3]*(filter)[3] + (signal)[(index)-4]*(filter)[4] + (signal)[(index)-5]*(filter)[5] + (signal)[(index)-6]*(filter)[6] + (signal)[(index)-7]*(filter)[7] + (signal)[(index)-8]*(filter)[8] + (signal)[(index)-9]*(filter)[9];
#define SUMABS(signal) abs((signal[0])) + abs((signal[1])) + abs((signal[2])) + abs((signal[3])) + abs((signal[4])) + abs((signal[5])) + abs((signal[6])) + abs((signal[7])) + abs((signal[8])) + abs((signal[9]));

void encode(short buffer[BUFFERSIZE], struct encode_chunk_struct * restrict chunk_left, struct encode_chunk_struct * restrict chunk_right, short encoded[BUFFERSIZE])
{
    short i, j, e;
    short encoded_tmp[BUFFERSIZE];

    short filter_even[FLENGTH_2] = ANALYSISFILTER_EVEN;
    short filter_odd[FLENGTH_2] = ANALYSISFILTER_ODD;
    int t1;
    int t2;

    div_t temp_div;
    int temp_sum;

    /**************/
    /** Analysis **/
    /**************/
    // First shift buffers
    for (i = 0 ; i < FLENGTH_2 - 1 ; i++) {
        e = BUFFERSIZE_4 + i;
        chunk_left->even_subband_1[i] = chunk_left->even_subband_1[e];
        chunk_left->odd_subband_1[i] = chunk_left->odd_subband_1[e];

        chunk_right->even_subband_1[i] = chunk_right->even_subband_1[e];
        chunk_right->odd_subband_1[i] = chunk_right->odd_subband_1[e];

        e = BUFFERSIZE_8 + i;
        chunk_left->even_subband_2a[i] = chunk_left->even_subband_2a[e];
        chunk_left->odd_subband_2a[i] = chunk_left->odd_subband_2a[e];

        chunk_left->even_subband_2c[i] = chunk_left->even_subband_2c[e];
        chunk_left->odd_subband_2c[i] = chunk_left->odd_subband_2c[e];

        chunk_right->even_subband_2a[i] = chunk_right->even_subband_2a[e];
        chunk_right->odd_subband_2a[i] = chunk_right->odd_subband_2a[e];

        chunk_right->even_subband_2c[i] = chunk_right->even_subband_2c[e];
        chunk_right->odd_subband_2c[i] = chunk_right->odd_subband_2c[e];
    }

    i = FLENGTH_2 - 1;
    chunk_left->even_subband_1[i] = buffer[0];
    chunk_left->odd_subband_1[i] = chunk_left->odd_1_lastvalue;

    chunk_right->even_subband_1[i] = buffer[1];
    chunk_right->odd_subband_1[i] = chunk_right->odd_1_lastvalue;

    chunk_left->odd_1_lastvalue = buffer[BUFFERSIZE - 2];
    chunk_right->odd_1_lastvalue = buffer[BUFFERSIZE - 1];

    // Stage One: loop for i = 0
    e = FLENGTH_2 - 1;
    t1 = CONV(chunk_left->even_subband_1, e, filter_even);
    t2 = CONV(chunk_left->odd_subband_1, e, filter_odd);
    chunk_left->odd_subband_2a[e] = chunk_left->odd_2a_lastvalue;
    chunk_left->odd_subband_2c[e] = chunk_left->odd_2c_lastvalue;
    chunk_left->even_subband_2a[e] = (t1 + t2) >> 16;
    chunk_left->even_subband_2c[e] = (t2 - t1) >> 16;

    t1 = CONV(chunk_right->even_subband_1, e, filter_even);
    t2 = CONV(chunk_right->odd_subband_1, e, filter_odd);
    chunk_right->even_subband_2a[e] = (t1 + t2) >> 16;
    chunk_right->even_subband_2c[e] = (t2 - t1) >> 16;
    chunk_right->odd_subband_2a[e] = chunk_right->odd_2a_lastvalue;
    chunk_right->odd_subband_2c[e] = chunk_right->odd_2c_lastvalue;

    for (i = 1 ; i < BUFFERSIZE_4 ; i+=2) {
        e = i << 2;
        j = i + FLENGTH_2 - 1;
        chunk_left->even_subband_1[j] = buffer[e];
        chunk_left->odd_subband_1[j] = buffer[e - 2];

        chunk_right->even_subband_1[j] = buffer[e + 1];
        chunk_right->odd_subband_1[j] = buffer[e - 1];

        e = (i + 1) << 2;
        j += 1;
        chunk_left->even_subband_1[j] = buffer[e];
        chunk_left->odd_subband_1[j] = buffer[e - 2];

        chunk_right->even_subband_1[j] = buffer[e + 1];
        chunk_right->odd_subband_1[j] = buffer[e - 1];

        // Stage One
        e = (i >> 1) + FLENGTH_2;
        j -= 1;
        t1 = CONV(chunk_left->even_subband_1, j, filter_even);
        t2 = CONV(chunk_left->odd_subband_1, j, filter_odd);

        chunk_left->odd_subband_2a[e] = (t1 + t2) >> 16;
        chunk_left->odd_subband_2c[e] = (t2 - t1) >> 16;

        t1 = CONV(chunk_right->even_subband_1, j, filter_even);
        t2 = CONV(chunk_right->odd_subband_1, j, filter_odd);

        chunk_right->odd_subband_2a[e] = (t1 + t2) >> 16;
        chunk_right->odd_subband_2c[e] = (t2 - t1) >> 16;

        if (i + 1 < BUFFERSIZE_4) {
            j += 1;
            t1 = CONV(chunk_left->even_subband_1, j, filter_even);
            t2 = CONV(chunk_left->odd_subband_1, j, filter_odd);

            chunk_left->even_subband_2a[e] = (t1 + t2) >> 16;
            chunk_left->even_subband_2c[e] = (t2 - t1) >> 16;

            t1 = CONV(chunk_right->even_subband_1, j, filter_even);
            t2 = CONV(chunk_right->odd_subband_1, j, filter_odd);

            chunk_right->even_subband_2a[e] = (t1 + t2) >> 16;
            chunk_right->even_subband_2c[e] = (t2 - t1) >> 16;
        }

        // Stage Two
        j = (i - 1) << 2;
        e = e - 1;
        t1 = CONV(chunk_left->even_subband_2a, e, filter_even);
        t2 = CONV(chunk_left->odd_subband_2a, e, filter_odd);

        encoded_tmp[j] = (t1 + t2) >> 16;
        encoded_tmp[j+1] = (t2 - t1) >> 16;

        t1 = CONV(chunk_left->even_subband_2c, e, filter_even);
        t2 = CONV(chunk_left->odd_subband_2c, e, filter_odd);

        encoded_tmp[j+2] = (t1 + t2) >> 16;
        encoded_tmp[j+3] = (t2 - t1) >> 16;

        t1 = CONV(chunk_right->even_subband_2a, e, filter_even);
        t2 = CONV(chunk_right->odd_subband_2a, e, filter_odd);

        encoded_tmp[j+4] = (t1 + t2) >> 16;
        encoded_tmp[j+5] = (t2 - t1) >> 16;

        t1 = CONV(chunk_right->even_subband_2c, e, filter_even);
        t2 = CONV(chunk_right->odd_subband_2c, e, filter_odd);

        encoded_tmp[j+6] = (t1 + t2) >> 16;
        encoded_tmp[j+7] = (t2 - t1) >> 16;
    }

    e = ((BUFFERSIZE_4 - 1) >> 1) + FLENGTH_2;
    chunk_left->odd_2a_lastvalue = chunk_left->odd_subband_2a[e];
    chunk_left->odd_2c_lastvalue = chunk_left->odd_subband_2c[e];
    chunk_right->odd_2a_lastvalue = chunk_right->odd_subband_2a[e];
    chunk_right->odd_2c_lastvalue = chunk_right->odd_subband_2c[e];

    for (i = 0 ; i < BUFFERSIZE_8 ; i++) {
        chunk_left->diff_deq_index++;
        e = i << 3;
        // for roundDiv precalculation
        temp_div = div(encoded_tmp[e] - chunk_left->prediction[0], chunk_left->Qstep[0]);

        if (chunk_left->diff_deq_index >= QLENGTH) {
            chunk_left->diff_deq_index = 0;
        }

        /*****************************/
        /** Encoding Subband 1 Left **/
        /*****************************/
        // roundDiv
        if((temp_div.rem << 1) > chunk_left->Qstep[0] && temp_div.quot > 0) {
            encoded_tmp[e] = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > chunk_left->Qstep[0] && temp_div.quot < 0) {
            encoded_tmp[e] = temp_div.quot - 1;
        } else {
            encoded_tmp[e] = temp_div.quot;
        }

        if (encoded_tmp[e] > MAX_LEVEL_1) {
            encoded_tmp[e] = MAX_LEVEL_1;
        } else if (encoded_tmp[e] < MIN_LEVEL_1) {
            encoded_tmp[e] = MIN_LEVEL_1;
        }
        chunk_left->diff_deq[0][chunk_left->diff_deq_index] = (short) (encoded_tmp[e] * chunk_left->Qstep[0]);

        // meanAbs
        temp_sum = SUMABS(chunk_left->diff_deq[0]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            temp_sum = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            temp_sum = temp_div.quot - 1;
        } else {
            temp_sum = temp_div.quot;
        }
        chunk_left->Qstep[0] = (short)((temp_sum * PHI_1) >> 13);

        // Precalculation subband 2 left
        e += 1;
        // roundDiv
        temp_div = div(encoded_tmp[e] - chunk_left->prediction[1], chunk_left->Qstep[1]);

        chunk_left->prediction[0] = (short)((MU_1*(chunk_left->diff_deq[0][chunk_left->diff_deq_index] + chunk_left->prediction[0])) >> 15);

        if (chunk_left->Qstep[0] < QMIN) {
            chunk_left->Qstep[0] = QMIN;
        } else if (chunk_left->Qstep[0] > QMAX_1) {
            chunk_left->Qstep[0] = QMAX_1;
        }

        /*****************************/
        /** Encoding Subband 2 Left **/
        /*****************************/
        if((temp_div.rem << 1) > chunk_left->Qstep[1] && temp_div.quot > 0) {
            encoded_tmp[e] = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > chunk_left->Qstep[1] && temp_div.quot < 0) {
            encoded_tmp[e] = temp_div.quot - 1;
        } else {
            encoded_tmp[e] = temp_div.quot;
        }

        if (encoded_tmp[e] > MAX_LEVEL_2) {
            encoded_tmp[e] = MAX_LEVEL_2;
        } else if (encoded_tmp[e] < MIN_LEVEL_2) {
            encoded_tmp[e] = MIN_LEVEL_2;
        }

        chunk_left->diff_deq[1][chunk_left->diff_deq_index] = (short) (encoded_tmp[e] * chunk_left->Qstep[1]);

        // meanAbs
        temp_sum = SUMABS(chunk_left->diff_deq[1]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            temp_sum = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            temp_sum = temp_div.quot - 1;
        } else {
            temp_sum = temp_div.quot;
        }
        chunk_left->Qstep[1] = (short)((temp_sum * PHI_2) >> 13);

        // Precalculation Subband 3 left
        e += 1;
        // roundDiv
        temp_div = div(encoded_tmp[e] - chunk_left->prediction[2], chunk_left->Qstep[2]);

        chunk_left->prediction[1] = (short)((MU_2*(chunk_left->diff_deq[1][chunk_left->diff_deq_index] + chunk_left->prediction[1])) >> 15);

        if (chunk_left->Qstep[1] < QMIN) {
            chunk_left->Qstep[1] = QMIN;
        } else if (chunk_left->Qstep[1] > QMAX_2) {
            chunk_left->Qstep[1] = QMAX_2;
        }

        /*****************************/
        /** Encoding Subband 3 Left **/
        /*****************************/
        if((temp_div.rem << 1) > chunk_left->Qstep[2] && temp_div.quot > 0) {
            encoded_tmp[e] = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > chunk_left->Qstep[2] && temp_div.quot < 0) {
            encoded_tmp[e] = temp_div.quot - 1;
        } else {
            encoded_tmp[e] = temp_div.quot;
        }

        if (encoded_tmp[e] > MAX_LEVEL_3) {
            encoded_tmp[e] = MAX_LEVEL_3;
        } else if (encoded_tmp[e] < MIN_LEVEL_3) {
            encoded_tmp[e] = MIN_LEVEL_3;
        }

        chunk_left->diff_deq[2][chunk_left->diff_deq_index] = (short) (encoded_tmp[e] * chunk_left->Qstep[2]);

        // meanAbs
        temp_sum = SUMABS(chunk_left->diff_deq[2]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            temp_sum = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            temp_sum = temp_div.quot - 1;
        } else {
            temp_sum = temp_div.quot;
        }
        chunk_left->Qstep[2] = (short)((temp_sum * PHI_3) >> 13);

        // Precalculation Subband 4 Left
        e += 1;
        // roundDiv
        temp_div = div(encoded_tmp[e] - chunk_left->prediction[3], chunk_left->Qstep[3]);

        chunk_left->prediction[2] = (short)((MU_3*(chunk_left->diff_deq[2][chunk_left->diff_deq_index] + chunk_left->prediction[2])) >> 15);

        if (chunk_left->Qstep[2] < QMIN) {
            chunk_left->Qstep[2] = QMIN;
        } else if (chunk_left->Qstep[2] > QMAX_3) {
            chunk_left->Qstep[2] = QMAX_3;
        }

        /*****************************/
        /** Encoding Subband 4 Left **/
        /*****************************/

        if((temp_div.rem << 1) > chunk_left->Qstep[3] && temp_div.quot > 0) {
            encoded_tmp[e] = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > chunk_left->Qstep[3] && temp_div.quot < 0) {
            encoded_tmp[e] = temp_div.quot - 1;
        } else {
            encoded_tmp[e] = temp_div.quot;
        }

        if (encoded_tmp[e] > MAX_LEVEL_4) {
            encoded_tmp[e] = MAX_LEVEL_4;
        } else if (encoded_tmp[e] < MIN_LEVEL_4) {
            encoded_tmp[e] = MIN_LEVEL_4;
        }

        chunk_left->diff_deq[3][chunk_left->diff_deq_index] = (short) (encoded_tmp[e] * chunk_left->Qstep[3]);
        // meanAbs
        temp_sum = SUMABS(chunk_left->diff_deq[3]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            temp_sum = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            temp_sum = temp_div.quot - 1;
        } else {
            temp_sum = temp_div.quot;
        }
        chunk_left->Qstep[3] = (short)((temp_sum * PHI_4) >> 13);

        // Precalculations Subband 1 Right
        e += 1;
        // roundDiv
        temp_div = div(encoded_tmp[e] - chunk_right->prediction[0], chunk_right->Qstep[0]);

        chunk_left->prediction[3] = (short)((MU_4*(chunk_left->diff_deq[3][chunk_left->diff_deq_index] + chunk_left->prediction[3])) >> 15);

        if (chunk_left->Qstep[3] < QMIN) {
            chunk_left->Qstep[3] = QMIN;
        } else if (chunk_left->Qstep[3] > QMAX_4) {
            chunk_left->Qstep[3] = QMAX_4;
        }

        /******************************/
        /** Encoding Subband 1 Right **/
        /******************************/

        if((temp_div.rem << 1) > chunk_right->Qstep[0] && temp_div.quot > 0) {
            encoded_tmp[e] = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > chunk_right->Qstep[0] && temp_div.quot < 0) {
            encoded_tmp[e] = temp_div.quot - 1;
        } else {
            encoded_tmp[e] = temp_div.quot;
        }

        if (encoded_tmp[e] > MAX_LEVEL_1) {
            encoded_tmp[e] = MAX_LEVEL_1;
        } else if (encoded_tmp[e] < MIN_LEVEL_1) {
            encoded_tmp[e] = MIN_LEVEL_1;
        }

        chunk_right->diff_deq[0][chunk_left->diff_deq_index] = (short) (encoded_tmp[e] * chunk_right->Qstep[0]);

        // meanAbs
        temp_sum = SUMABS(chunk_right->diff_deq[0]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            temp_sum = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            temp_sum = temp_div.quot - 1;
        } else {
            temp_sum = temp_div.quot;
        }
        chunk_right->Qstep[0] = (short)((temp_sum * PHI_1) >> 13);

        // Precalculation Subband 2 Right
        e += 1;
        // roundDiv
        temp_div = div(encoded_tmp[e] - chunk_right->prediction[1], chunk_right->Qstep[1]);

        chunk_right->prediction[0] = (short)((MU_1*(chunk_right->diff_deq[0][chunk_left->diff_deq_index] + chunk_right->prediction[0])) >> 15);

        if (chunk_right->Qstep[0] < QMIN) {
            chunk_right->Qstep[0] = QMIN;
        } else if (chunk_right->Qstep[0] > QMAX_1) {
            chunk_right->Qstep[0] = QMAX_1;
        }

        /******************************/
        /** Encoding Subband 2 Right **/
        /******************************/

        if((temp_div.rem << 1) > chunk_right->Qstep[1] && temp_div.quot > 0) {
            encoded_tmp[e] = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > chunk_right->Qstep[1] && temp_div.quot < 0) {
            encoded_tmp[e] = temp_div.quot - 1;
        } else {
            encoded_tmp[e] = temp_div.quot;
        }

        if (encoded_tmp[e] > MAX_LEVEL_2) {
            encoded_tmp[e] = MAX_LEVEL_2;
        } else if (encoded_tmp[e] < MIN_LEVEL_2) {
            encoded_tmp[e] = MIN_LEVEL_2;
        }

        chunk_right->diff_deq[1][chunk_left->diff_deq_index] = (short) (encoded_tmp[e] * chunk_right->Qstep[1]);

        // meanAbs
        temp_sum = SUMABS(chunk_right->diff_deq[1]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            temp_sum = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            temp_sum = temp_div.quot - 1;
        } else {
            temp_sum = temp_div.quot;
        }
        chunk_right->Qstep[1] = (short)((temp_sum * PHI_2) >> 13);

        // Precalculation Subband 3 Right
        e += 1;
        // roundDiv
        temp_div = div(encoded_tmp[e] - chunk_right->prediction[2], chunk_right->Qstep[2]);

        chunk_right->prediction[1] = (short)((MU_2*(chunk_right->diff_deq[1][chunk_left->diff_deq_index] + chunk_right->prediction[1])) >> 15);

        if (chunk_right->Qstep[1] < QMIN) {
            chunk_right->Qstep[1] = QMIN;
        } else if (chunk_right->Qstep[1] > QMAX_2) {
            chunk_right->Qstep[1] = QMAX_2;
        }

        /******************************/
        /** Encoding Subband 3 Right **/
        /******************************/

        if((temp_div.rem << 1) > chunk_right->Qstep[2] && temp_div.quot > 0) {
            encoded_tmp[e] = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > chunk_right->Qstep[2] && temp_div.quot < 0) {
            encoded_tmp[e] = temp_div.quot - 1;
        } else {
            encoded_tmp[e] = temp_div.quot;
        }

        if (encoded_tmp[e] > MAX_LEVEL_3) {
            encoded_tmp[e] = MAX_LEVEL_3;
        } else if (encoded_tmp[e] < MIN_LEVEL_3) {
            encoded_tmp[e] = MIN_LEVEL_3;
        }

        chunk_right->diff_deq[2][chunk_left->diff_deq_index] = (short) (encoded_tmp[e] * chunk_right->Qstep[2]);

        // meanAbs
        temp_sum = SUMABS(chunk_right->diff_deq[2]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            temp_sum = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            temp_sum = temp_div.quot - 1;
        } else {
            temp_sum = temp_div.quot;
        }

        chunk_right->Qstep[2] = (short)((temp_sum * PHI_3) >> 13);

        // Precalculation Subband 4 Right
        e += 1;
        // roundDiv
        temp_div = div(encoded_tmp[e] - chunk_right->prediction[3], chunk_right->Qstep[3]);

        chunk_right->prediction[2] = (short)((MU_3*(chunk_right->diff_deq[2][chunk_left->diff_deq_index] + chunk_right->prediction[2])) >> 15);

        if (chunk_right->Qstep[2] < QMIN) {
            chunk_right->Qstep[2] = QMIN;
        } else if (chunk_right->Qstep[2] > QMAX_3) {
            chunk_right->Qstep[2] = QMAX_3;
        }

        /******************************/
        /** Encoding Subband 4 Right **/
        /******************************/

        if((temp_div.rem << 1) > chunk_right->Qstep[3] && temp_div.quot > 0) {
            encoded_tmp[e] = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > chunk_right->Qstep[3] && temp_div.quot < 0) {
            encoded_tmp[e] = temp_div.quot - 1;
        } else {
            encoded_tmp[e] = temp_div.quot;
        }

        if (encoded_tmp[e] > MAX_LEVEL_4) {
            encoded_tmp[e] = MAX_LEVEL_4;
        } else if (encoded_tmp[e] < MIN_LEVEL_4) {
            encoded_tmp[e] = MIN_LEVEL_4;
        }

        chunk_right->diff_deq[3][chunk_left->diff_deq_index] = (short) (encoded_tmp[e] * chunk_right->Qstep[3]);

        // meanAbs
        temp_sum = SUMABS(chunk_right->diff_deq[3]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            temp_sum = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            temp_sum = temp_div.quot - 1;
        } else {
            temp_sum = temp_div.quot;
        }
        chunk_right->Qstep[3] = (short)((temp_sum * PHI_4) >> 13);

        chunk_right->prediction[3] = (short)((MU_4*(chunk_right->diff_deq[3][chunk_left->diff_deq_index] + chunk_right->prediction[3])) >> 15);

        if (chunk_right->Qstep[3] < QMIN) {
            chunk_right->Qstep[3] = QMIN;
        } else if (chunk_right->Qstep[3] > QMAX_4) {
            chunk_right->Qstep[3] = QMAX_4;
        }
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
