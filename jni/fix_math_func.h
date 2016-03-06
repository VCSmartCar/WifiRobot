#ifndef FIX_MATH_FUNC_H_
#define FIX_MATH_FUNC_H_

/*******************
 Struct: sctfc_ntn_t
 Description:
	��α��ѧ����������ʾ��, base�����������32λq15������ʾ������base��ʵֵ����q0ʱ��ֵ���ķ�Χ��(2^-7, 2^8)��exp������
 *******************/
typedef struct
{
	long base;
	int exp;
}sctfc_ntn_t;

 long mul32_q15(long a, long b);
 long long mul32_q23(long a, long b);
 long long mul_40q23_32q31(long long a, long b);
 long mul32_q31(long a, long b);
 unsigned long sqrt32_q10(unsigned long a);
 long ln_40q23_to_32q15(long long a);
 
 sctfc_ntn_t sctfc_ntn_add(sctfc_ntn_t a, sctfc_ntn_t b);
 sctfc_ntn_t sctfc_ntn_mul(sctfc_ntn_t a, sctfc_ntn_t b);
 sctfc_ntn_t real_to_sctfc_ntn(long a, int q);
 long sctfc_ntn_ln(sctfc_ntn_t a, int q);
 
 inline long mul32_utility(long a, long b, int qa, int qb, int qr);
 long long mul32_utility_extended(long a, long b, int qa, int qb, int qr);
 long long mul40_utility(long long a, long long b, int qa, int qb, int qr);

 void realfft(long xin[], int power);
#endif /*FIX_MATH_FUNC_H_*/
