struct decode_chunk_struct {
    short t1_subband_1[FLENGTH/2 - 1 + BUFFERSIZE/4];
    short t2_subband_1[FLENGTH/2 - 1 + BUFFERSIZE/4];

    short t1_subband_2a[FLENGTH/2 - 1 + BUFFERSIZE/8];
    short t2_subband_2a[FLENGTH/2 - 1 + BUFFERSIZE/8];
    short t1_subband_2c[FLENGTH/2 - 1 + BUFFERSIZE/8];
    short t2_subband_2c[FLENGTH/2 - 1 + BUFFERSIZE/8];

    /*Quantisation*/
    short prediction[4];
    short Qstep[4];
    short diff_deq[4][QLENGTH];
};

void decode(struct decode_chunk_struct * chunk_left, struct decode_chunk_struct * chunk_right, short encoded[BUFFERSIZE], short decoded_0[BUFFERSIZE]);
