 /**************************
  * File Name: fix_math_func.c
  * Description:几个定点数的数学函数
  * Author:GouGin
  * Date: 2012/11/27
  ***************************/

  #include "fix_math_func.h"
#include <stdio.h>
#include "cmplx_func.h"
 /***************************
  * Function:mul32_q10
  * Description:两个32位的q10的数的相乘
  * Parameters:
  * 	a, b:两个要相乘的数
  * Return:相乘的积，32位Q10
  * **************************/
long mul32_q10(long a, long b)
{
	int64_t result = (int64_t)a * b;
	return (long)(result >> 10);
}//end mul32_q10

 /***************************
  * Function:mul32_q15
  * Description:两个32位的q15的数的相乘
  * Parameters:
  * 	a, b:两个要相乘的数
  * Return:相乘的积，32位Q15
  * **************************/

long mul32_q15(long a, long b)
{
	int64_t result = (int64_t)a * b;
	return (long)(result >> 15);
}//end mul32_q15


 /***************************
  * Function:mul32_q23
  * Description:两个32位的q23的数的相乘
  * Parameters:
  * 	a, b:两个要相乘的数
  * Return:相乘的积，40位Q23
  * **************************/
long long mul32_q23(long a, long b)
{
	int64_t result = (int64_t)a * b;
	return (result >> 23);
}//end mul32_q23

 /***************************
  * Function:mul_40q23_32q31
  * Description:1个40位的q23的数与1个32位q31的数相乘
  * Parameters:
  * 	a, b:两个要相乘的数
  * Return:相乘的积，40位Q23
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
  * Description:两个32位的q31的数的相乘
  * Parameters:
  * 	a, b:两个要相乘的数
  * Return:相乘的积，32位Q31
  * **************************/
long mul32_q31(long a, long b)
{
	long long c = (long long)a * b;
	c >>= 31;
	return (long)c;
}//end mul32_q10

 /*****************************
  * Function:sqrt32_q10
  * Description:求一个32位Q15的数的开方,用牛顿法
  * Parameters:
  * 	a:要求开方的数
  * Return:开方后的值，32为Q15
  * ****************************/
unsigned long sqrt32_q10(unsigned long a)
{
	unsigned long returnValue = 1024;
	unsigned long preValue = 1024;
	unsigned long long temp = a;
	temp <<= 10; //变为q20的数
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
  * Description:求一个40位q23的数的自然对数，结果为32q15
  * 			原理为ln(x) = ln(n*2^m) = ln(n) + m * ln(2)
  * 					   = ln(2n) + (m-1)*ln(2)
  * 					   = ln(2n - 1 + 1) + (m - 1) * ln(2)
  * 					   = f(2n-1) + (m-1) * ln(2)
  * 			     其中f(x) = ln(x + 1)
  * 					   = x - x^2/2 + x^3/3 -... + (-1)^(k+1) * x^k / k    k趋向于无穷大  
  * Parameters：
  * 	a：40位q23的数
  * Return：求自然对数后的值，32为Q15
  * ************************/
long ln_40q23_to_32q15(long long a)
{
	int exp = -7; //因为先从q23转换成q15,移了8位，又由于移位后要算2*a+1, 2*a放在移位里算，因此算少了一次右移，加回1
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
	//令a的值小于2大于1
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
	Description:两个定点数相乘，乘数与结果的q值都由参数决定
	Parameters:
		a, b:乘数
		qa:a的q值
		qb:b的q值
		qr:结果的q值
	Return:32位乘积结果
 ***************************/
long mul32_utility(long a, long b, int qa, int qb, int qr)
{
	int q_left = qa + qb - qr; //相乘后要右移的位数
	int64_t result = (int64_t)a * b;
	if (q_left > 0)
		result >>= q_left;
	else
		result <<= (0 - q_left);
	return (long)result;
}//end mul32_utility

/***************************
	Name: sctfc_ntn_add
	Description:对于sctfc_ntn_t类型的数相加，返回sctfc_ntn_t类型的结果
	Parameters:
		a, b:相加的两个数
	Return:sctfc_ntn_t的结果
 **************************/
sctfc_ntn_t sctfc_ntn_add(sctfc_ntn_t a, sctfc_ntn_t b)
{
	long max = (long)1<<23; //base所允许的最大值,2^8
	long min = (long)1<<8;  //base所允许的最小值,2^-7
	long one_q15 = (long)1<<15;
	long half_q15 = (long)1<<14;
	long tmp_base;
	sctfc_ntn_t result;
	//对齐
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
	//相加
	result.base = a.base + b.base;
	//如果和为0，直接返回
	if (result.base == 0)
		return result;
	//取底数的绝对值
	if (result.base < 0)
		tmp_base = 0 - result.base;
	else
		tmp_base = result.base;
	//如果超出最大值或小于最小值，把base规范为小于1大于0.5的数
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
	//加上符号
	if (result.base < 0)
		result.base = 0 - tmp_base;
	else
		result.base = tmp_base;
	return result;
}//end sctfc_ntn_add

/***************************
	Name:sctfc_ntn_mul
	Description:两个sctfc_ntn_t类型的数相乘,返回sctfc_ntn_t类型的结果
	Parameter:
		a, b:相乘的两个数
	Return:sctfc_ntn_t的结果
 **************************/
sctfc_ntn_t sctfc_ntn_mul(sctfc_ntn_t a, sctfc_ntn_t b)
{
	long max = (long)1<<23; //base所允许的最大值,2^8
	long min = (long)1<<8;  //base所允许的最小值,2^-7
	long one_q15 = (long)1<<15;
	long half_q15 = (long)1<<14;
	long tmp_base;
	sctfc_ntn_t result;
	result.exp = a.exp + b.exp;
	result.base = mul32_utility(a.base, b.base, 15, 15, 15);

	//如果乘积为0，直接返回
	if (result.base == 0)
		return result;
	//取底数的绝对值
	if (result.base < 0)
		tmp_base = 0 - result.base;
	else
		tmp_base = result.base;
	//如果超出最大值或小于最小值，把base规范为小于1大于0.5的数
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
	//加上符号
	if (result.base < 0)
		result.base = 0 - tmp_base;
	else
		result.base = tmp_base;
	return result;
}//end sctfc_ntn_mul

/***************************
	Name:real_to_sctfc_ntn
	Description:把一个定点数转换为sctfc_ntn_t格式
	Parameters:
		a:要转换的定点数
		q：a的q值
	Return：值为a的sctfc_ntn_t类型
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
	//取a的值绝对值
	if (a < 0)
		tmp_a = 0 - a;
	else
		tmp_a = a;
	//规范base的值
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
	Description:sctfc_ntn_t的对数函数，返回long类型的结果
	Parameters:
		a:要求对数的数
		q:结果的q值
	Return：求对数的结果,32位
 **************************/
long sctfc_ntn_ln(sctfc_ntn_t a, int q)
{
	long result;
	long tmp;
	long long two = (long)1<<(q + 1);	//用long long是因为当q=31时，q+1=32,结果不会为负数
	long one = (long)1<<q;
	long nom;
	int exp = q - 15 + a.exp;
	int sign = 1;
	int denom = 1;
	int i;
	
	tmp = a.base;
	if (tmp <= 0)
		return (-10) * (long)(1<<q);

	//令a的值小于2大于1
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
	//如果tmp越接近1，循环次数越大
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
	Description:对输入的实数数据做傅里叶变换
	Parameters:
		xin:输入的数据
		power：数据的长度为2^power
	Return:
		结果保存在xin
*************************************/
void realfft(long xin[], int power)
{
	int L=0; // 级间运算层
	int J=0; // 级内运算层
	int K=0,KB=0; // 蝶形运算层
	int M=1,Nn=0;// N=2^M
	int B=0; // 蝶形运算两输入数据间隔
	int i;
	int N = 1 << power;
	/* 以下是为倒序新建的局部变量*/
	int LH=0,J2=0,N1=0,I,K2=0;

	long Vn[2];//每一级第一个旋转因子虚部为0，实部为1
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
	/*以下是倒序*/
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
	/* 以下为计算出M */
	Nn=N;
	M = power;

	/* 蝶形运算 */
	for(L=1;L<=M;L++)  // 级间
	{
		B= 1 << (L-1);
		for(J=0;J<B;J++)   // 级内
		{
			cmplx_set(Vn, re_matrix[J * 128 / B], im_matrix[J * 128 / B]);
			for(K=J;K<N;K+= B<<1)  // 蝶形因子运算
			{
				KB=K+B;

				xk = xin + (K<<1);
				xkb = xin + (KB<<1);

				cmplx_mul(xkb, Vn, a, 1);
				//为了防止溢出，每次蝶形运算都右移一位
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
			//先乘以0.5，可以防止溢出,但Q值仍然是之前的
			cmplx_right_shift(xi, xi, 1);  //0.5 * x[i]
			if (xi != x_n_i)  //防止同一个数重复乘以0.5
				cmplx_right_shift(x_n_i, x_n_i, 1);  //0.5 * x[-i]

			cmplx_conj(x_n_i, c);
			cmplx_add(xi, c, x_2n_i);  //x[2n - i] = (x[i] + x*[-i]) * 0.5  这是暂存的
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
