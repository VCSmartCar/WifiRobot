
#ifndef _MFCC_H_
#define _MFCC_H_

#define DIMENTION 24
#define FILTER_NUM 24
#define DCT_NUM 12
#define FRAME_LEN 256
#define LOG_FRAME_LEN 8
#define MAX_FRAMES 120
void mfcc(long *source, long *mfccMatrix, int framelen, int size);
#endif
