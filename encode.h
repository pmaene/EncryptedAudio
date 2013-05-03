struct encode_chunk_struct {
    /*Samples of the previous buffer needed for convolution*/
    short even_subband_1[FLENGTH/2 - 1 + BUFFERSIZE/4];
    short odd_subband_1[FLENGTH/2 - 1 + BUFFERSIZE/4];
    short odd_1_lastvalue;

    short even_subband_2a[FLENGTH/2 - 1 + BUFFERSIZE/8];
    short odd_subband_2a[FLENGTH/2 - 1 + BUFFERSIZE/8];
    short odd_2a_lastvalue;
    short even_subband_2c[FLENGTH/2 - 1 + BUFFERSIZE/8];
    short odd_subband_2c[FLENGTH/2 - 1 + BUFFERSIZE/8];
    short odd_2c_lastvalue;

    /*Quantisation*/
    short prediction[4];
    short Qstep[4];
    short diff_deq[4][QLENGTH];
};

void encode(short buffer[BUFFERSIZE], struct encode_chunk_struct * chunk_left, struct encode_chunk_struct * chunk_right, short encoded[BUFFERSIZE]);
