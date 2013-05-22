#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "codec.h"
#include "decode.h"

#define CONV(signal, index, filter) (signal)[(index)]*(filter)[0] + (signal)[(index)-1]*(filter)[1] + (signal)[(index)-2]*(filter)[2] + (signal)[(index)-3]*(filter)[3] + (signal)[(index)-4]*(filter)[4] + (signal)[(index)-5]*(filter)[5] + (signal)[(index)-6]*(filter)[6] + (signal)[(index)-7]*(filter)[7] + (signal)[(index)-8]*(filter)[8] + (signal)[(index)-9]*(filter)[9];
#define SUMABS(signal) abs((signal[0])) + abs((signal[1])) + abs((signal[2])) + abs((signal[3])) + abs((signal[4])) + abs((signal[5])) + abs((signal[6])) + abs((signal[7])) + abs((signal[8])) + abs((signal[9]));

void decode(struct decode_chunk_struct * restrict chunk_left, struct decode_chunk_struct * restrict chunk_right, short encoded[BUFFERSIZE], short decoded_0[BUFFERSIZE])
{
    short i, j, e;

    short decoded_left1a[BUFFERSIZE_4];
    short decoded_left1b[BUFFERSIZE_4];
    short decoded_right1a[BUFFERSIZE_4];
    short decoded_right1b[BUFFERSIZE_4];
    short encoded_tmp[BUFFERSIZE];

    short filter_even[FLENGTH_2] = SYNTHESISFILTER_EVEN;
    short filter_odd[FLENGTH_2] = SYNTHESISFILTER_ODD;
    int even;
    int odd;
    int mean;

    div_t temp_div;
    int temp_sum;


    /********************/
    /** Bit Degrouping **/
    /********************/
    encoded_tmp[0] = ((encoded[0] & 0xF800) >> 11) + MIN_LEVEL_1;
    encoded_tmp[1] = ((encoded[0] & 0x700) >> 8) + MIN_LEVEL_2;
    encoded_tmp[2] = ((encoded[0] & 0xC0) >> 6) + MIN_LEVEL_3;
    encoded_tmp[3] = ((encoded[0] & 0x30) >> 4) + MIN_LEVEL_4;
    encoded_tmp[6] = ((encoded[0] & 0xc) >> 2) + MIN_LEVEL_3;
    encoded_tmp[7] = (encoded[0] & 0x3) + MIN_LEVEL_4;

    encoded_tmp[4] = ((encoded[1] & 0xF800) >> 11) + MIN_LEVEL_1;
    encoded_tmp[5] = ((encoded[1] & 0x700) >> 8) + MIN_LEVEL_2;
    encoded_tmp[8] = ((encoded[1] & 0xF8) >> 3) + MIN_LEVEL_1;
    encoded_tmp[9] = (encoded[1] & 0x7) + MIN_LEVEL_2;

    encoded_tmp[10] = ((encoded[2] & 0xc000) >> 14) + MIN_LEVEL_3;
    encoded_tmp[11] = ((encoded[2] & 0x3000) >> 12) + MIN_LEVEL_4;
    encoded_tmp[12] = ((encoded[2] & 0xF80) >> 7) + MIN_LEVEL_1;
    encoded_tmp[13] = ((encoded[2] & 0x70) >> 4) + MIN_LEVEL_2;
    encoded_tmp[14] = ((encoded[2] & 0xc) >> 2) + MIN_LEVEL_3;
    encoded_tmp[15] = (encoded[2] & 0x3) + MIN_LEVEL_4;

    encoded_tmp[16] = ((encoded[3] & 0xF800) >> 11) + MIN_LEVEL_1;
    encoded_tmp[17] = ((encoded[3] & 0x700) >> 8) + MIN_LEVEL_2;
    encoded_tmp[18] = ((encoded[3] & 0xC0) >> 6) + MIN_LEVEL_3;
    encoded_tmp[19] = ((encoded[3] & 0x30) >> 4) + MIN_LEVEL_4;
    encoded_tmp[22] = ((encoded[3] & 0xc) >> 2) + MIN_LEVEL_3;
    encoded_tmp[23] = (encoded[3] & 0x3) + MIN_LEVEL_4;

    encoded_tmp[20] = ((encoded[4] & 0xF800) >> 11) + MIN_LEVEL_1;
    encoded_tmp[21] = ((encoded[4] & 0x700) >> 8) + MIN_LEVEL_2;
    encoded_tmp[24] = ((encoded[4] & 0xF8) >> 3) + MIN_LEVEL_1;
    encoded_tmp[25] = (encoded[4] & 0x7) + MIN_LEVEL_2;

    encoded_tmp[26] = ((encoded[5] & 0xc000) >> 14) + MIN_LEVEL_3;
    encoded_tmp[27] = ((encoded[5] & 0x3000) >> 12) + MIN_LEVEL_4;
    encoded_tmp[28] = ((encoded[5] & 0xF80) >> 7) + MIN_LEVEL_1;
    encoded_tmp[29] = ((encoded[5] & 0x70) >> 4) + MIN_LEVEL_2;
    encoded_tmp[30] = ((encoded[5] & 0xc) >> 2) + MIN_LEVEL_3;
    encoded_tmp[31] = (encoded[5] & 0x3) + MIN_LEVEL_4;

    encoded_tmp[32] = ((encoded[6] & 0xF800) >> 11) + MIN_LEVEL_1;
    encoded_tmp[33] = ((encoded[6] & 0x700) >> 8) + MIN_LEVEL_2;
    encoded_tmp[34] = ((encoded[6] & 0xC0) >> 6) + MIN_LEVEL_3;
    encoded_tmp[35] = ((encoded[6] & 0x30) >> 4) + MIN_LEVEL_4;
    encoded_tmp[38] = ((encoded[6] & 0xc) >> 2) + MIN_LEVEL_3;
    encoded_tmp[39] = (encoded[6] & 0x3) + MIN_LEVEL_4;

    encoded_tmp[36] = ((encoded[7] & 0xF800) >> 11) + MIN_LEVEL_1;
    encoded_tmp[37] = ((encoded[7] & 0x700) >> 8) + MIN_LEVEL_2;


    for (i = 0 ; i < BUFFERSIZE_8 ; i++) {
        chunk_left->diff_deq_index++;
        if (chunk_left->diff_deq_index >= QLENGTH) {
            chunk_left->diff_deq_index = 0;
        }

        /*******************************/
        /** Decoding Subband One Left **/
        /*******************************/
        chunk_left->diff_deq[0][chunk_left->diff_deq_index] = (short) (encoded_tmp[i << 3] * chunk_left->Qstep[0]);

        // meanAbs
        temp_sum = SUMABS(chunk_left->diff_deq[0]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            mean = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            mean = temp_div.quot - 1;
        } else {
            mean = temp_div.quot;
        }

        chunk_left->Qstep[0] = (short) (mean * PHI_1 >> 13);
        if (chunk_left->Qstep[0] < QMIN) {
            chunk_left->Qstep[0] = QMIN;
        } else if (chunk_left->Qstep[0] > QMAX_1) {
            chunk_left->Qstep[0] = QMAX_1;
        }

        encoded_tmp[i << 3] = (short) (chunk_left->diff_deq[0][chunk_left->diff_deq_index] + chunk_left->prediction[0]);
        chunk_left->prediction[0] = (short)((MU_1*(encoded_tmp[i << 3])) >> 15);

        /*******************************/
        /** Decoding Subband Two Left **/
        /*******************************/
        chunk_left->diff_deq[1][chunk_left->diff_deq_index] = (short) (encoded_tmp[(i << 3)+1] * chunk_left->Qstep[1]);

        //meanAbs
        temp_sum = SUMABS(chunk_left->diff_deq[1]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            mean = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            mean = temp_div.quot - 1;
        } else {
            mean = temp_div.quot;
        }

        chunk_left->Qstep[1] = (short) (mean * PHI_2 >> 13);
        if (chunk_left->Qstep[1] < QMIN) {
            chunk_left->Qstep[1] = QMIN;
        } else if (chunk_left->Qstep[1] > QMAX_2) {
            chunk_left->Qstep[1] = QMAX_2;
        }

        encoded_tmp[(i << 3)+1] = (short) (chunk_left->diff_deq[1][chunk_left->diff_deq_index] + chunk_left->prediction[1]);
        chunk_left->prediction[1] = (short)((MU_2*(encoded_tmp[(i << 3)+1])) >> 15);

        /*********************************/
        /** Decoding Subband Three Left **/
        /*********************************/
        chunk_left->diff_deq[2][chunk_left->diff_deq_index] = (short) (encoded_tmp[(i << 3)+2] * chunk_left->Qstep[2]);

        //meanAbs
        temp_sum = SUMABS(chunk_left->diff_deq[2]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            mean = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            mean = temp_div.quot - 1;
        } else {
            mean = temp_div.quot;
        }

        chunk_left->Qstep[2] = (short) (mean * PHI_3 >> 13);
        if (chunk_left->Qstep[2] < QMIN) {
            chunk_left->Qstep[2] = QMIN;
        } else if (chunk_left->Qstep[2] > QMAX_3) {
            chunk_left->Qstep[2] = QMAX_3;
        }

        encoded_tmp[(i << 3)+2] = (short) (chunk_left->diff_deq[2][chunk_left->diff_deq_index] + chunk_left->prediction[2]);
        chunk_left->prediction[2] = (short)((MU_3*(encoded_tmp[(i << 3)+2])) >> 15);

        /********************************/
        /** Decoding Subband Four Left **/
        /********************************/
        chunk_left->diff_deq[3][chunk_left->diff_deq_index] = (short) (encoded_tmp[(i << 3)+3] * chunk_left->Qstep[3]);

        //meanAbs
        temp_sum = SUMABS(chunk_left->diff_deq[3]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            mean = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            mean = temp_div.quot - 1;
        } else {
            mean = temp_div.quot;
        }

        chunk_left->Qstep[3] = (short) (mean * PHI_4 >> 13);
        if (chunk_left->Qstep[3] < QMIN) {
            chunk_left->Qstep[3] = QMIN;
        } else if (chunk_left->Qstep[3] > QMAX_4) {
            chunk_left->Qstep[3] = QMAX_4;
        }

        encoded_tmp[(i << 3)+3] = (short) (chunk_left->diff_deq[3][chunk_left->diff_deq_index] + chunk_left->prediction[3]);
        chunk_left->prediction[3] = (short)((MU_4*(encoded_tmp[(i << 3)+3])) >> 15);

        /********************************/
        /** Decoding Subband One Right **/
        /********************************/
        chunk_right->diff_deq[0][chunk_left->diff_deq_index] = (short) (encoded_tmp[(i << 3)+4] * chunk_right->Qstep[0]);

        //meanAbs
        temp_sum = SUMABS(chunk_right->diff_deq[0]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            mean = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            mean = temp_div.quot - 1;
        } else {
            mean = temp_div.quot;
        }

        chunk_right->Qstep[0] = (short) (mean * PHI_1 >> 13);
        if (chunk_right->Qstep[0] < QMIN) {
            chunk_right->Qstep[0] = QMIN;
        } else if (chunk_right->Qstep[0] > QMAX_1) {
            chunk_right->Qstep[0] = QMAX_1;
        }

        encoded_tmp[(i << 3)+4] = (short) (chunk_right->diff_deq[0][chunk_left->diff_deq_index] + chunk_right->prediction[0]);
        chunk_right->prediction[0] = (short)((MU_1*(encoded_tmp[(i << 3)+4])) >> 15);

        /********************************/
        /** Decoding Subband Two Right **/
        /********************************/
        chunk_right->diff_deq[1][chunk_left->diff_deq_index] = (short) (encoded_tmp[(i << 3)+5] * chunk_right->Qstep[1]);

        //meanAbs
        temp_sum = SUMABS(chunk_right->diff_deq[1]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            mean = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            mean = temp_div.quot - 1;
        } else {
            mean = temp_div.quot;
        }

        chunk_right->Qstep[1] = (short) (mean * PHI_2 >> 13);
        if (chunk_right->Qstep[1] < QMIN) {
            chunk_right->Qstep[1] = QMIN;
        } else if (chunk_right->Qstep[1] > QMAX_2) {
            chunk_right->Qstep[1] = QMAX_2;
        }

        encoded_tmp[(i << 3)+5] = (short) (chunk_right->diff_deq[1][chunk_left->diff_deq_index] + chunk_right->prediction[1]);
        chunk_right->prediction[1] = (short)((MU_2*(encoded_tmp[(i << 3)+5])) >> 15);

        /**********************************/
        /** Decoding Subband Three Right **/
        /**********************************/
        chunk_right->diff_deq[2][chunk_left->diff_deq_index] = (short) (encoded_tmp[(i << 3)+6] * chunk_right->Qstep[2]);

        //meanAbs
        temp_sum = SUMABS(chunk_right->diff_deq[2]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            mean = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            mean = temp_div.quot - 1;
        } else {
            mean = temp_div.quot;
        }

        chunk_right->Qstep[2] = (short) (mean * PHI_3 >> 13);
        if (chunk_right->Qstep[2] < QMIN) {
            chunk_right->Qstep[2] = QMIN;
        } else if (chunk_right->Qstep[2] > QMAX_3) {
            chunk_right->Qstep[2] = QMAX_3;
        }

        encoded_tmp[(i << 3)+6] = (short) (chunk_right->diff_deq[2][chunk_left->diff_deq_index] + chunk_right->prediction[2]);
        chunk_right->prediction[2] = (short)((MU_3*(encoded_tmp[(i << 3)+6])) >> 15);

        /*********************************/
        /** Decoding Subband Four Right **/
        /*********************************/
        chunk_right->diff_deq[3][chunk_left->diff_deq_index] = (short) (encoded_tmp[(i << 3)+7] * chunk_right->Qstep[3]);

        //meanAbs
        temp_sum = SUMABS(chunk_right->diff_deq[3]);

        temp_div = div(temp_sum, QLENGTH);

        if((temp_div.rem << 1) > QLENGTH && temp_div.quot > 0) {
            mean = temp_div.quot + 1;
        } else if (-(temp_div.rem << 1) > QLENGTH && temp_div.quot < 0) {
            mean = temp_div.quot - 1;
        } else {
            mean = temp_div.quot;
        }

        chunk_right->Qstep[3] = (short) (mean * PHI_4 >> 13);
        if (chunk_right->Qstep[3] < QMIN) {
            chunk_right->Qstep[3] = QMIN;
        } else if (chunk_right->Qstep[3] > QMAX_4) {
            chunk_right->Qstep[3] = QMAX_4;
        }

        encoded_tmp[(i << 3)+7] = (short) (chunk_right->diff_deq[3][chunk_left->diff_deq_index] + chunk_right->prediction[3]);
        chunk_right->prediction[3] = (short)((MU_4*(encoded_tmp[(i << 3)+7])) >> 15);
    }


    /***************/
    /** Synthesis **/
    /***************/
    for (i = 0 ; i < BUFFERSIZE_4 ; i+=2) {
        e = i >> 1;
        j = e + FLENGTH_2 - 1;
        chunk_left->t1_subband_2a[j] = encoded_tmp[e << 3] + encoded_tmp[(e << 3) + 1];
        chunk_left->t2_subband_2a[j] = encoded_tmp[e << 3] - encoded_tmp[(e << 3) + 1];
        chunk_left->t1_subband_2c[j] = encoded_tmp[(e << 3) + 2] + encoded_tmp[(e << 3) + 3];
        chunk_left->t2_subband_2c[j] = encoded_tmp[(e << 3) + 2] - encoded_tmp[(e << 3) + 3];
        chunk_right->t1_subband_2a[j] = encoded_tmp[(e << 3) + 4] + encoded_tmp[(e << 3) + 5];
        chunk_right->t2_subband_2a[j] = encoded_tmp[(e << 3) + 4] - encoded_tmp[(e << 3) + 5];
        chunk_right->t1_subband_2c[j] = encoded_tmp[(e << 3) + 6] + encoded_tmp[(e << 3) + 7];
        chunk_right->t2_subband_2c[j] = encoded_tmp[(e << 3) + 6] - encoded_tmp[(e << 3) + 7];

        even = CONV(chunk_left->t1_subband_2a, j, filter_even);
        odd = CONV(chunk_left->t2_subband_2a, j, filter_odd);

        decoded_left1a[e << 1] = even >> 14;
        decoded_left1a[(e << 1) + 1] = odd >> 14;

        even = CONV(chunk_left->t1_subband_2c, j, filter_even);
        odd = CONV(chunk_left->t2_subband_2c, j, filter_odd);

        decoded_left1b[e << 1] = even >> 14;
        decoded_left1b[(e << 1) + 1] = odd >> 14;

        even = CONV(chunk_right->t1_subband_2a, j, filter_even);
        odd = CONV(chunk_right->t2_subband_2a, j, filter_odd);

        decoded_right1a[e << 1] = even >> 14;
        decoded_right1a[(e << 1) + 1] = odd >> 14;

        even = CONV(chunk_right->t1_subband_2c, j, filter_even);
        odd = CONV(chunk_right->t2_subband_2c, j, filter_odd);

        decoded_right1b[e << 1] = even >> 14;
        decoded_right1b[(e << 1) + 1] = odd >> 14;

        j =  i + FLENGTH_2 - 1;
        chunk_left->t1_subband_1[j] = decoded_left1a[i] + decoded_left1b[i];
        chunk_left->t2_subband_1[j] = decoded_left1a[i] - decoded_left1b[i];
        chunk_right->t1_subband_1[j] = decoded_right1a[i] + decoded_right1b[i];
        chunk_right->t2_subband_1[j] = decoded_right1a[i] - decoded_right1b[i];

        even = CONV(chunk_left->t1_subband_1, j, filter_even);
        odd = CONV(chunk_left->t2_subband_1, j, filter_odd);

        j += 1;
        decoded_0[i << 2] = even >> 14;
        decoded_0[(i << 2) + 2] = odd >> 14;

        chunk_left->t1_subband_1[j] = decoded_left1a[i+1] + decoded_left1b[i+1];
        chunk_left->t2_subband_1[j] = decoded_left1a[i+1] - decoded_left1b[i+1];
        chunk_right->t1_subband_1[j] = decoded_right1a[i+1] + decoded_right1b[i+1];
        chunk_right->t2_subband_1[j] = decoded_right1a[i+1] - decoded_right1b[i+1];

        even = CONV(chunk_left->t1_subband_1, j, filter_even);
        odd = CONV(chunk_left->t2_subband_1, j, filter_odd);

        decoded_0[(i+1) << 2] = even >> 14;
        decoded_0[((i+1) << 2) + 2] = odd >> 14;

        even = CONV(chunk_right->t1_subband_1, j, filter_even);
        odd = CONV(chunk_right->t2_subband_1, j, filter_odd);

        decoded_0[((i+1) << 2) + 1] = even >> 14;
        decoded_0[((i+1) << 2) + 3] = odd >> 14;
    }

    // Last shift buffers
    for (i = 0 ; i < FLENGTH_2 - 1 ; i++) {
        chunk_left->t1_subband_2a[i] = chunk_left->t1_subband_2a[BUFFERSIZE_8 + i];
        chunk_left->t2_subband_2a[i] = chunk_left->t2_subband_2a[BUFFERSIZE_8 + i];
        chunk_left->t1_subband_2c[i] = chunk_left->t1_subband_2c[BUFFERSIZE_8 + i];
        chunk_left->t2_subband_2c[i] = chunk_left->t2_subband_2c[BUFFERSIZE_8 + i];
        chunk_right->t1_subband_2a[i] = chunk_right->t1_subband_2a[BUFFERSIZE_8 + i];
        chunk_right->t2_subband_2a[i] = chunk_right->t2_subband_2a[BUFFERSIZE_8 + i];
        chunk_right->t1_subband_2c[i] = chunk_right->t1_subband_2c[BUFFERSIZE_8 + i];
        chunk_right->t2_subband_2c[i] = chunk_right->t2_subband_2c[BUFFERSIZE_8 + i];

        chunk_left->t1_subband_1[i] = chunk_left->t1_subband_1[BUFFERSIZE_4 + i];
        chunk_left->t2_subband_1[i] = chunk_left->t2_subband_1[BUFFERSIZE_4 + i];
        chunk_right->t1_subband_1[i] = chunk_right->t1_subband_1[BUFFERSIZE_4 + i];
        chunk_right->t2_subband_1[i] = chunk_right->t2_subband_1[BUFFERSIZE_4 + i];
    }

    return;
}
