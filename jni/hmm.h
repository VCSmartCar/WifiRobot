#ifndef _HMM_H_
#define _HMM_H_

#define NAINF -327680L //-10 q15
#define pi 102944L  //3.14159 q15
#define LOGN_10_Q15 75451L
#define REALMIN 0.0000000001
#define CODE_SIZE 128  //码本的个数
#define STATE_SIZE 8	//状态的个数
#define HMM_SIZE 5
#define HMM_INIT 0
#define HMM_TRANS 1
#define HMM_PROBS 2
typedef struct
{				
    long init[STATE_SIZE];
    long trans[STATE_SIZE][STATE_SIZE];
    long probs[STATE_SIZE][CODE_SIZE];
}Hmm;

int readHmm(const char path[]);
int readCodebook(const char path[]);
void readMatrix(const char path[], long *matrix, int m, int n);
int hmm(long mfcc[], int length, long *probability);
#endif


