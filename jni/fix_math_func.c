 /**************************
  * File Name: fix_math_func.c
  * Description:��������������ѧ����
  * Author:GouGin
  * Date: 2012/11/27
  ***************************/

  #include "fix_math_func.h"
#include <stdio.h>
#include "cmplx_func.h"
 /***************************
  * Function:mul32_q10
  * Description:����32λ��q10���������
  * Parameters:
  * 	a, b:����Ҫ��˵���
  * Return:��˵Ļ���32λQ10
  * **************************/
long mul32_q10(long a, long b)
{
	int64_t result = (int64_t)a * b;
	return (long)(result >> 10);
}//end mul32_q10

 /***************************
  * Function:mul32_q15
  * Description:����32λ��q15���������
  * Parameters:
  * 	a, b:����Ҫ��˵���
  * Return:��˵Ļ���32λQ15
  * **************************/

long mul32_q15(long a, long b)
{
	int64_t result = (int64_t)a * b;
	return (long)(result >> 15);
}//end mul32_q15


 /***************************
  * Function:mul32_q23
  * Description:����32λ��q23���������
  * Parameters:
  * 	a, b:����Ҫ��˵���
  * Return:��˵Ļ���40λQ23
  * **************************/
long long mul32_q23(long a, long b)
{
	int64_t result = (int64_t)a * b;
	return (result >> 23);
}//end mul32_q23

 /***************************
  * Function:mul_40q23_32q31
  * Description:1��40λ��q23������1��32λq31�������
  * Parameters:
  * 	a, b:����Ҫ��˵���
  * Return:��˵Ļ���40λQ23
  * **************************/
long long mul_40q23_32q31(long long a, long b)
{
	long ae = (int)(((long long)(a & 0xff00000000)) >> 32);
	long ah = ((unsigned long)(a & 0x00ffff0000) >> 16);
	long al= (unsigned int)(a & 0x000000ffff);
	long bh = ((long)(b & 0x00ffff0000) >> 16);
	long bl= (unsigned int)(b & 0x000000ffff);

	long long result = ((long long)ae * bh) << 17;
	result += ((long long)ae * bl + (long long)ah * bh) << 1;
	result += ((long long)ah * bl + (long long)bh * al) >> 15;
	return result;
}//end mul_40q23_32q31


 /***************************
  * Function:mul32_q31
  * Description:����32λ��q31���������
  * Parameters:
  * 	a, b:����Ҫ��˵���
  * Return:��˵Ļ���32λQ31
  * **************************/
long mul32_q31(long a, long b)
{
	long long c = (long long)a * b;
	c >>= 31;
	return (long)c;
}//end mul32_q10

 /*****************************
  * Function:sqrt32_q10
  * Description:��һ��32λQ15�����Ŀ���,��ţ�ٷ�
  * Parameters:
  * 	a:Ҫ�󿪷�����
  * Return:�������ֵ��32ΪQ15
  * ****************************/
unsigned long sqrt32_q10(unsigned long a)
{
	unsigned long returnValue = 1024;
	unsigned long preValue = 1024;
	unsigned long long temp = a;
	temp <<= 10; //��Ϊq20����
	for (;;)
	{
		returnValue -= (returnValue - temp / returnValue) >> 1;
		if (returnValue == preValue)
			break;
		else
			preValue = returnValue;
	}
	return returnValue;
}//end sqrt32_a10

 /************************
  * Function:ln_40q23_to_32q15
  * Description:��һ��40λq23��������Ȼ���������Ϊ32q15
  * 			ԭ��Ϊln(x) = ln(n*2^m) = ln(n) + m * ln(2)
  * 					   = ln(2n) + (m-1)*ln(2)
  * 					   = ln(2n - 1 + 1) + (m - 1) * ln(2)
  * 					   = f(2n-1) + (m-1) * ln(2)
  * 			     ����f(x) = ln(x + 1)
  * 					   = x - x^2/2 + x^3/3 -... + (-1)^(k+1) * x^k / k    k�����������  
  * Parameters��
  * 	a��40λq23����
  * Return������Ȼ�������ֵ��32ΪQ15
  * ************************/
long ln_40q23_to_32q15(long long a)
{
	int exp = -7; //��Ϊ�ȴ�q23ת����q15,����8λ����������λ��Ҫ��2*a+1, 2*a������λ���㣬���������һ�����ƣ��ӻ�1
	int sign = 1;
	int denom = 1;
	int i;
	long nom;
	long two_q15 = (long)1 << 16;
	long one_q15 = (long)1 << 15;
	long result;
	long tmp;
	if (a <= 0)
		return -327680L;
	//��a��ֵС��2����1
	if (a >= two_q15)
	{
		while (a > two_q15)
		{
			a >>= 1;
			exp += 1;
		}
	}
	else
	{
		while (a < one_q15)
		{
			a <<= 1;
			exp -= 1;
		}
	}//end if
	a -= 32768L; //a -= 1

	tmp = (long)a;
	result = tmp;
	nom = tmp;
	denom = 1;
	for (i = 0; i < 5; i++)
	{
		nom = mul32_q15(nom, tmp);
		denom += 1;
		sign = 0 - sign;
		result += nom * sign / denom;
	}
	result += (exp - 1) * 22713L;  //22713 = ln(2) * 2^15; 
	return result;
}//end ln_40q23_to_32q15
 
/***************************
	Name:mul32_utility
	Description:������������ˣ�����������qֵ���ɲ�������
	Parameters:
		a, b:����
		qa:a��qֵ
		qb:b��qֵ
		qr:�����qֵ
	Return:32λ�˻����
 ***************************/
long mul32_utility(long a, long b, int qa, int qb, int qr)
{
	int q_left = qa + qb - qr; //��˺�Ҫ���Ƶ�λ��
	int64_t result = (int64_t)a * b;
	if (q_left > 0)
		result >>= q_left;
	else
		result <<= (0 - q_left);
	return (long)result;
}//end mul32_utility

/***************************
	Name: sctfc_ntn_add
	Description:����sctfc_ntn_t���͵�����ӣ�����sctfc_ntn_t���͵Ľ��
	Parameters:
		a, b:��ӵ�������
	Return:sctfc_ntn_t�Ľ��
 **************************/
sctfc_ntn_t sctfc_ntn_add(sctfc_ntn_t a, sctfc_ntn_t b)
{
	long max = (long)1<<23; //base����������ֵ,2^8
	long min = (long)1<<8;  //base���������Сֵ,2^-7
	long one_q15 = (long)1<<15;
	long half_q15 = (long)1<<14;
	long tmp_base;
	sctfc_ntn_t result;
	//����
	if (a.exp > b.exp)
	{
		result.exp = a.exp;
		b.base >>= a.exp - b.exp;
		b.exp = a.exp;
	}
	else
	{
		result.exp = b.exp;
		a.base >>= b.exp - a.exp;
		a.exp = b.exp;
	}
	//���
	result.base = a.base + b.base;
	//�����Ϊ0��ֱ�ӷ���
	if (result.base == 0)
		return result;
	//ȡ�����ľ���ֵ
	if (result.base < 0)
		tmp_base = 0 - result.base;
	else
		tmp_base = result.base;
	//����������ֵ��С����Сֵ����base�淶ΪС��1����0.5����
	if (tmp_base >= max)
	{
		while (tmp_base >= one_q15)
		{
			tmp_base >>= 1;
			result.exp += 1;
		}
	}
	else if (tmp_base <= min)
	{
		while (tmp_base < half_q15)
		{
			tmp_base <<= 1;
			result.exp -= 1;
		}
	}//end if
	//���Ϸ���
	if (result.base < 0)
		result.base = 0 - tmp_base;
	else
		result.base = tmp_base;
	return result;
}//end sctfc_ntn_add

/***************************
	Name:sctfc_ntn_mul
	Description:����sctfc_ntn_t���͵������,����sctfc_ntn_t���͵Ľ��
	Parameter:
		a, b:��˵�������
	Return:sctfc_ntn_t�Ľ��
 **************************/
sctfc_ntn_t sctfc_ntn_mul(sctfc_ntn_t a, sctfc_ntn_t b)
{
	long max = (long)1<<23; //base����������ֵ,2^8
	long min = (long)1<<8;  //base���������Сֵ,2^-7
	long one_q15 = (long)1<<15;
	long half_q15 = (long)1<<14;
	long tmp_base;
	sctfc_ntn_t result;
	result.exp = a.exp + b.exp;
	result.base = mul32_utility(a.base, b.base, 15, 15, 15);

	//����˻�Ϊ0��ֱ�ӷ���
	if (result.base == 0)
		return result;
	//ȡ�����ľ���ֵ
	if (result.base < 0)
		tmp_base = 0 - result.base;
	else
		tmp_base = result.base;
	//����������ֵ��С����Сֵ����base�淶ΪС��1����0.5����
	if (tmp_base >= max)
	{
		while (tmp_base >= one_q15)
		{
			tmp_base >>= 1;
			result.exp += 1;
		}
	}
	else if (tmp_base <= min)
	{
		while (tmp_base < half_q15)
		{
			tmp_base <<= 1;
			result.exp -= 1;
		}
	}//end if
	//���Ϸ���
	if (result.base < 0)
		result.base = 0 - tmp_base;
	else
		result.base = tmp_base;
	return result;
}//end sctfc_ntn_mul

/***************************
	Name:real_to_sctfc_ntn
	Description:��һ��������ת��Ϊsctfc_ntn_t��ʽ
	Parameters:
		a:Ҫת���Ķ�����
		q��a��qֵ
	Return��ֵΪa��sctfc_ntn_t����
 ***************************/
sctfc_ntn_t real_to_sctfc_ntn(long a, int q)
{
	long one_q15 = (long)1<<15;
	long half_q15 = (long)1<<14;
	long tmp_a;
	sctfc_ntn_t result;
	if (a == 0)
	{
		result.base = 0;
		result.exp = 0;
		return result;
	}
	//ȡa��ֵ����ֵ
	if (a < 0)
		tmp_a = 0 - a;
	else
		tmp_a = a;
	//�淶base��ֵ
	if (tmp_a >= one_q15)
	{
		while (tmp_a >= one_q15)
		{
			tmp_a >>= 1;
			q++;
		}
	}
	else if (tmp_a < half_q15)
	{
		while (tmp_a < half_q15)
		{
			tmp_a <<= 1;
			q--;
		}
	}//end if
	
	if (a < 0)
		result.base = 0 - tmp_a;
	else
		result.base = tmp_a;
	result.exp = q - 15;
	return result;
}//end real_to_sctfc_ntn
	


/***************************
	Name:sctfc_ntn_ln
	Description:sctfc_ntn_t�Ķ�������������long���͵Ľ��
	Parameters:
		a:Ҫ���������
		q:�����qֵ
	Return��������Ľ��,32λ
 **************************/
long sctfc_ntn_ln(sctfc_ntn_t a, int q)
{
	long result;
	long tmp;
	long long two = (long)1<<(q + 1);	//��long long����Ϊ��q=31ʱ��q+1=32,�������Ϊ����
	long one = (long)1<<q;
	long nom;
	int exp = q - 15 + a.exp;
	int sign = 1;
	int denom = 1;
	int i;
	
	tmp = a.base;
	if (tmp <= 0)
		return (-10) * (long)(1<<q);

	//��a��ֵС��2����1
	if (tmp >= two)
	{
		while (tmp >= two)
		{
			tmp >>= 1;
			exp += 1;
		}
	}
	else
	{
		while (tmp < one)
		{
			tmp <<= 1;
			exp -= 1;
		}
	}//end if
	tmp -= (long)1 << q; //tmp -= 1

	result = tmp;
	nom = tmp;
	denom = 1;
	i = 0;
	//���tmpԽ�ӽ�1��ѭ������Խ��
	for (; ; )
	{
		nom = mul32_utility(nom, tmp, q, q, q);
		i++;
		if (nom == 0)
			break;
		denom += 1;
		sign = 0 - sign;
		result += nom * sign / denom;
	}
	result += exp * (long)(1488522236L >> (31 - q));  //1488522236 = ln(2) * 2^31
	return result;
}//end sctfc_ntn_ln
 
/**********************************
	Name: realfft_v2
	Description:�������ʵ������������Ҷ�任
	Parameters:
		xin:���������
		power�����ݵĳ���Ϊ2^power
	Return:
		���������xin
*************************************/
void realfft(long xin[], int power)
{
	int L=0; // ���������
	int J=0; // ���������
	int K=0,KB=0; // ���������
	int M=1,Nn=0;// N=2^M
	int B=0; // �����������������ݼ��
	int i;
	int N = 1 << power;
	/* ������Ϊ�����½��ľֲ�����*/
	int LH=0,J2=0,N1=0,I,K2=0;

	long Vn[2];//ÿһ����һ����ת�����鲿Ϊ0��ʵ��Ϊ1
	long a[2];
	long b[2];
	long c[2];
	long *xi, *x_n_i, *x_2n_i;
	long *xk, *xkb;
	long re_matrix[] = {
		2147483647, 2146836866, 2144896910, 2141664948, 2137142927, 2131333572, 2124240380, 2115867626, 2106220352, 2095304370,
2083126254, 2069693342, 2055013723, 2039096241, 2021950484, 2003586779, 1984016189, 1963250501, 1941302225, 1918184581,
1893911494, 1868497586, 1841958164, 1814309216, 1785567396, 1755750017, 1724875040, 1692961062, 1660027308, 1626093616,
1591180426, 1555308768, 1518500250, 1480777044, 1442161874, 1402678000, 1362349204, 1321199781, 1279254516, 1236538675,
1193077991, 1148898640, 1104027237, 1058490808, 1012316784, 965532978, 918167572, 870249095, 821806413, 772868706,
723465451, 673626408, 623381598, 572761285, 521795963, 470516330, 418953276, 367137861, 315101295, 262874923,
210490206, 157978697, 105372028, 52701887, 0, -52701887, -105372028, -157978697, -210490206, -262874923,
-315101295, -367137861, -418953276, -470516330, -521795963, -572761285, -623381598, -673626408, -723465451, -772868706,
-821806413, -870249095, -918167572, -965532978, -1012316784, -1058490808, -1104027237, -1148898640, -1193077991, -1236538675,
-1279254516, -1321199781, -1362349204, -1402678000, -1442161874, -1480777044, -1518500250, -1555308768, -1591180426, -1626093616,
-1660027308, -1692961062, -1724875040, -1755750017, -1785567396, -1814309216, -1841958164, -1868497586, -1893911494, -1918184581,
-1941302225, -1963250501, -1984016189, -2003586779, -2021950484, -2039096241, -2055013723, -2069693342, -2083126254, -2095304370,
-2106220352, -2115867626, -2124240380, -2131333572, -2137142927, -2141664948, -2144896910, -2146836866 };
	long im_matrix[] = {
		0, -52701887, -105372028, -157978697, -210490206, -262874923, -315101295, -367137861, -418953276, -470516330,
-521795963, -572761285, -623381598, -673626408, -723465451, -772868706, -821806413, -870249095, -918167572, -965532978,
-1012316784, -1058490808, -1104027237, -1148898640, -1193077991, -1236538675, -1279254516, -1321199781, -1362349204, -1402678000,
-1442161874, -1480777044, -1518500250, -1555308768, -1591180426, -1626093616, -1660027308, -1692961062, -1724875040, -1755750017,
-1785567396, -1814309216, -1841958164, -1868497586, -1893911494, -1918184581, -1941302225, -1963250501, -1984016189, -2003586779,
-2021950484, -2039096241, -2055013723, -2069693342, -2083126254, -2095304370, -2106220352, -2115867626, -2124240380, -2131333572,
-2137142927, -2141664948, -2144896910, -2146836866, -2147483647, -2146836866, -2144896910, -2141664948, -2137142927, -2131333572,
-2124240380, -2115867626, -2106220352, -2095304370, -2083126254, -2069693342, -2055013723, -2039096241, -2021950484, -2003586779,
-1984016189, -1963250501, -1941302225, -1918184581, -1893911494, -1868497586, -1841958164, -1814309216, -1785567396, -1755750017,
-1724875040, -1692961062, -1660027308, -1626093616, -1591180426, -1555308768, -1518500250, -1480777044, -1442161874, -1402678000,
-1362349204, -1321199781, -1279254516, -1236538675, -1193077991, -1148898640, -1104027237, -1058490808, -1012316784, -965532978,
-918167572, -870249095, -821806413, -772868706, -723465451, -673626408, -623381598, -572761285, -521795963, -470516330,
-418953276, -367137861, -315101295, -262874923, -210490206, -157978697, -105372028, -52701887 	};
	/*�����ǵ���*/
	LH=N>>1; // LH=N/2
	J2=LH;
	N1=N-2;
	for(I=1;I<=N1;I++)
	{
		if(I<J2)
		{
			cmplx_move(a, xin + (I<<1));
			cmplx_move(xin + (I<<1), xin + (J2<<1));
			cmplx_move(xin + (J2<<1), a);
		}
		K2=LH;
		while(J2>=K2)
		{
			J2-=K2;
			K2=K2/2;// K2=K2/2
		}
		J2+=K2;
	}
	/* ����Ϊ�����M */
	Nn=N;
	M = power;

	/* �������� */
	for(L=1;L<=M;L++)  // ����
	{
		B= 1 << (L-1);
		for(J=0;J<B;J++)   // ����
		{
			cmplx_set(Vn, re_matrix[J * 128 / B], im_matrix[J * 128 / B]);
			for(K=J;K<N;K+= B<<1)  // ������������
			{
				KB=K+B;

				xk = xin + (K<<1);
				xkb = xin + (KB<<1);

				cmplx_mul(xkb, Vn, a, 1);
				//Ϊ�˷�ֹ�����ÿ�ε������㶼����һλ
				cmplx_right_shift(xk, xk, 1);

				cmplx_sub(xk, a, xkb); //x[kb] = x[k] - vn * x[kb]
				cmplx_add(xk, a, xk);	 //x[k] = x[k] + vn * x[kb]

			}
		}
	}

	for (i = 0; i <= N / 2; i++)
	{
		if (i == 0)
		{
			xin[(N<<1) + RE] = xin[(0<<1) + IM] ;
			xin[(N<<1) + IM] = 0;
			xin[(0<<1) + IM] = 0;
		}
		else
		{
			xi = xin + (i<<1);
			x_n_i = xin + ((N-i)<<1);
			x_2n_i = xin + (((N<<1) - i)<<1);
			//�ȳ���0.5�����Է�ֹ���,��Qֵ��Ȼ��֮ǰ��
			cmplx_right_shift(xi, xi, 1);  //0.5 * x[i]
			if (xi != x_n_i)  //��ֹͬһ�����ظ�����0.5
				cmplx_right_shift(x_n_i, x_n_i, 1);  //0.5 * x[-i]

			cmplx_conj(x_n_i, c);
			cmplx_add(xi, c, x_2n_i);  //x[2n - i] = (x[i] + x*[-i]) * 0.5  �����ݴ��
			cmplx_sub(xi, c, b);  //b = (x[i] - x*[-i]) * 0.5
			cmplx_mul_minus_j(b, xin + ((N + i)<<1));  //x[n + i] = (x[i] - x*[-i]) * (-j)*0.5

			cmplx_conj(xi, c);
			cmplx_add(x_n_i, c, a); // a = (x[n-i] + x*[i-n]) * 0.5
			cmplx_sub(x_n_i, c, b); //b = (x[n-i] - x*[i-n]) * 0.5

			cmplx_move((xi), x_2n_i);  //x[i] = (x[i] + x*[-i]) * 0.5;
			cmplx_move(x_n_i, a); // x[n-i] = (x[n-i] + x*[i-n]) * 0.5
			cmplx_mul_minus_j(b, x_2n_i);  //x[2n - i] = (x[n-i] - x*[i-n]) * (-j) * 0.5
		}
	}//end for


	for (i = 0; i < N; i++)
	{
		cmplx_set(Vn, re_matrix[i * 128 / N], im_matrix[i * 128 / N]);
		KB = N + i;

		xi = xin + (i<<1);
		xkb = xin + (KB<<1);

		cmplx_mul(xkb, Vn, a, 1);

		cmplx_right_shift(xi, xi, 1);

		cmplx_sub(xi, a, xkb); //x[kb] = x[i] - vn * x[kb]
		cmplx_add(xi, a, xi);	 //x[i] = x[i] + vn * x[kb]
	}

}//end realfft
