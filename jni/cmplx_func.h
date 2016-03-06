#ifndef _CMPLX_FUNC_H_
#define _CMPLX_FUNC_H_
#define RE 0
#define IM 1

void cmplx_mul(long a[], long b[], long store[], int right_shift);
long cmplx_ampl(long a[]);
//Êý¾ÝÒÆ¶¯
void cmplx_move(long des[], long source[]);
void cmplx_set(long a[], long re, long im);
void cmplx_sub(long a[], long b[], long store[]);
void cmplx_add(long a[], long b[], long store[]);
void cmplx_conj(long source[], long des[]);
void cmplx_left_shift(long source[], long des[], int bits);
void cmplx_right_shift(long source[], long des[], int bits);
void cmplx_mul_minus_j(long source[], long des[]);

#endif
