#include "cmplx_func.h"
#include "fix_math_func.h"
#define Q_VALUE 31


//复数相乘
void cmplx_mul(long a[], long b[], long store[], int right_shift)
{
	long tmp1, tmp2;
	if (right_shift)
	{
		tmp1 = (mul32_utility(a[RE], b[RE], Q_VALUE, Q_VALUE, Q_VALUE)>>1) - (mul32_utility(a[IM], b[IM], Q_VALUE, Q_VALUE, Q_VALUE)>>1);
		tmp2 = (mul32_utility(a[RE], b[IM], Q_VALUE, Q_VALUE, Q_VALUE)>>1) + (mul32_utility(a[IM], b[RE], Q_VALUE, Q_VALUE, Q_VALUE)>>1);
	}
	else
	{
		tmp1 = mul32_utility(a[RE], b[RE], Q_VALUE, Q_VALUE, Q_VALUE) - mul32_utility(a[IM], b[IM], Q_VALUE, Q_VALUE, Q_VALUE);
		tmp2 = mul32_utility(a[RE], b[IM], Q_VALUE, Q_VALUE, Q_VALUE) + mul32_utility(a[IM], b[RE], Q_VALUE, Q_VALUE, Q_VALUE);
	}
	store[RE] = tmp1;
	store[IM] = tmp2;
}//end cmplx_mul

//求能量
long cmplx_ampl(long a[])
{
	return mul32_utility(a[RE], a[RE], Q_VALUE, Q_VALUE, Q_VALUE) + mul32_utility(a[IM], a[IM], Q_VALUE, Q_VALUE, Q_VALUE);
}//end cmplx_ampl

void cmplx_move(long des[], long source[])
{
	des[RE] = source[RE];
	des[IM] = source[IM];
}//end cmplx_move
//复数值设定
void cmplx_set(long a[], long re, long im)
{
	a[RE] = re;
	a[IM] = im;
}//end cmplx_set
//复数相减
void cmplx_sub(long a[], long b[], long store[])
{
	store[RE] = a[RE] - b[RE];
	store[IM] = a[IM] - b[IM];
}//end cmplx_sub

//复数相加
void cmplx_add(long a[], long b[], long store[])
{
	store[RE] = a[RE] + b[RE];
	store[IM] = a[IM] + b[IM];
}//end cmplx_add

//共轭
void cmplx_conj(long source[], long des[])
{
	des[RE] = source[RE];
	des[IM] = 0 - source[IM];
}//end cmplx_conj



//左移
void cmplx_left_shift(long source[], long des[], int bits)
{
	des[RE] = source[RE] << bits;
	des[IM] = source[IM] << bits;
}//end cmplx_left_right

//右移
void cmplx_right_shift(long source[], long des[], int bits)
{
	des[RE] = source[RE] >> bits;
	des[IM] = source[IM] >> bits;
}//end cmplx_right_shift

//乘以 -j
void cmplx_mul_minus_j(long source[], long des[])
{
	des[RE] = source[IM];
	des[IM] = 0 - source[RE];
}//end cmplx_mul_minus_j
