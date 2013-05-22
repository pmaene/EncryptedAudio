#define QLENGTH 10 /*windowlength for the quantisation*/
#define FLENGTH 20
#define FLENGTH_2 10

/*Scaled filtercoefficients (with 2^15)*/
#define ANALYSISFILTER_EVEN {-20, 60, 0, -599, 3554, 15618, -3137, 1229, -398, 82}
#define ANALYSISFILTER_ODD {82, -398, 1229, -3137, 15618, 3554, -599, 0, 60, -20}
#define SYNTHESISFILTER_EVEN {-20<<1, 60<<1, 0<<1, -599<<1, 3554<<1, 15618<<1, -3137<<1, 1229<<1, -398<<1, 82<<1}
#define SYNTHESISFILTER_ODD {82<<1, -398<<1, 1229<<1, -3137<<1, 15618<<1, 3554<<1, -599<<1, 0<<1, 60<<1, -20<<1}

/*Scaled step sizes for quantisation for nbits = [5,3,2,2]*/
#define QSTART 33
#define QMIN 2

#define MU_1 19595
#define MU_2 6291
#define MU_3 -27525
#define MU_4 -131

#define PHI_1 2212
#define PHI_2 4342
#define PHI_3 10650
#define PHI_4 13107

#define QMAX_1 1024
#define QMAX_2 4096
#define QMAX_3 8192
#define QMAX_4 8192

#define MAX_LEVEL_1 15
#define MAX_LEVEL_2 3
#define MAX_LEVEL_3 1
#define MAX_LEVEL_4 1

#define MIN_LEVEL_1 -16
#define MIN_LEVEL_2 -4
#define MIN_LEVEL_3 -2
#define MIN_LEVEL_4 -2
