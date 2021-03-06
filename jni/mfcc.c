/****************************************
 * File name: 	mfcc.c
 * Description: 对语音数据提取MFCC参数的模块
 * Author:		GouGin
 * Version:		1.0
 * Date:		2012/8/30
 * History:
 ****************************************/
#include "math.h"
#include "mfcc.h"
#include "fix_math_func.h"
static long hammingMatrix[256] = {
	171798692L, 172098549L, 172997940L, 174496318L, 176592774L, 179286034L, 182574464L, 186456068L, 190928488L, 195989010L, 
	201634561L, 207861715L, 214666690L, 222045355L, 229993231L, 238505492L, 247576971L, 257202161L, 267375218L, 278089966L, 
	289339901L, 301118192L, 313417688L, 326230924L, 339550120L, 353367189L, 367673745L, 382461101L, 397720279L, 413442017L, 
	429616770L, 446234717L, 463285771L, 480759579L, 498645534L, 516932776L, 535610204L, 554666479L, 574090031L, 593869070L, 
	613991586L, 634445364L, 655217987L, 676296842L, 697669134L, 719321888L, 741241958L, 763416036L, 785830661L, 808472224L, 
	831326981L, 854381056L, 877620454L, 901031065L, 924598677L, 948308983L, 972147587L, 996100019L, 1020151735L, 1044288135L, 
	1068494565L, 1092756330L, 1117058701L, 1141386923L, 1165726227L, 1190061837L, 1214378979L, 1238662889L, 1262898826L, 1287072076L, 
	1311167962L, 1335171858L, 1359069189L, 1382845449L, 1406486202L, 1429977096L, 1453303871L, 1476452364L, 1499408522L, 1522158409L, 
	1544688213L, 1566984256L, 1589033004L, 1610821069L, 1632335224L, 1653562409L, 1674489736L, 1695104501L, 1715394188L, 1735346479L, 
	1754949262L, 1774190636L, 1793058920L, 1811542658L, 1829630629L, 1847311852L, 1864575593L, 1881411371L, 1897808966L, 1913758421L, 
	1929250055L, 1944274463L, 1958822522L, 1972885402L, 1986454565L, 1999521772L, 2012079091L, 2024118899L, 2035633885L, 2046617060L, 
	2057061755L, 2066961630L, 2076310674L, 2085103212L, 2093333905L, 2100997757L, 2108090115L, 2114606674L, 2120543477L, 2125896920L, 
	2130663753L, 2134841081L, 2138426370L, 2141417443L, 2143812482L, 2145610036L, 2146809011L, 2147408681L, 2147408681L, 2146809011L, 
	2145610036L, 2143812482L, 2141417443L, 2138426370L, 2134841081L, 2130663753L, 2125896920L, 2120543477L, 2114606674L, 2108090115L, 
	2100997757L, 2093333905L, 2085103212L, 2076310674L, 2066961630L, 2057061755L, 2046617060L, 2035633885L, 2024118899L, 2012079091L, 
	1999521772L, 1986454565L, 1972885402L, 1958822522L, 1944274463L, 1929250055L, 1913758421L, 1897808966L, 1881411371L, 1864575593L, 
	1847311852L, 1829630629L, 1811542658L, 1793058920L, 1774190636L, 1754949262L, 1735346479L, 1715394188L, 1695104501L, 1674489736L, 
	1653562409L, 1632335224L, 1610821069L, 1589033004L, 1566984256L, 1544688213L, 1522158409L, 1499408522L, 1476452364L, 1453303871L, 
	1429977096L, 1406486202L, 1382845449L, 1359069189L, 1335171858L, 1311167962L, 1287072076L, 1262898826L, 1238662889L, 1214378979L, 
	1190061837L, 1165726227L, 1141386923L, 1117058701L, 1092756330L, 1068494565L, 1044288135L, 1020151735L, 996100019L, 972147587L, 
	948308983L, 924598677L, 901031065L, 877620454L, 854381056L, 831326981L, 808472224L, 785830661L, 763416036L, 741241958L, 
	719321888L, 697669134L, 676296842L, 655217987L, 634445364L, 613991586L, 593869070L, 574090031L, 554666479L, 535610204L, 
	516932776L, 498645534L, 480759579L, 463285771L, 446234717L, 429616770L, 413442017L, 397720279L, 382461101L, 367673745L, 
	353367189L, 339550120L, 326230924L, 313417688L, 301118192L, 289339901L, 278089966L, 267375218L, 257202161L, 247576971L, 
	238505492L, 229993231L, 222045355L, 214666690L, 207861715L, 201634561L, 195989010L, 190928488L, 186456068L, 182574464L, 
	179286034L, 176592774L, 174496318L, 172997940L, 172098549L, 171798692L
	};

static long melf16[242] = {
	1385422288L, 2074932745L, 710873243L, 
	244392952L, 1608452454L, 2029673745L, 724561748L, 
	289651953L, 1594763949L, 2082881629L, 943825425L, 
	236444069L, 1375500272L, 2147107792L, 1362572743L, 285555305L, 
	172217905L, 956752954L, 2033770393L, 1869470781L, 761312019L, 
	449854917L, 1558013678L, 2147186209L, 1540437084L, 506714228L, 
	172139488L, 778888613L, 1812611470L, 2119295898L, 1423945307L, 477010144L, 
	200029799L, 895380390L, 1842315553L, 2127712070L, 1531373288L, 621862803L, 173325988L, 
	191613628L, 787952410L, 1697462895L, 2145999709L, 1801876007L, 972231460L, 292947470L, 
	517449690L, 1347094238L, 2026378227L, 2083976219L, 1513825977L, 721193150L, 216087316L, 
	235349478L, 805499720L, 1598132547L, 2103238381L, 2029343545L, 1448705777L, 713779464L, 232104887L, 
	289982153L, 870619921L, 1605546234L, 2087220811L, 2074602066L, 1597580996L, 912309267L, 354198853L, 
	244723632L, 721744701L, 1407016431L, 1965126845L, 2145025563L, 1883544347L, 1313209632L, 686345805L, 259951581L, 
	174300135L, 435781350L, 1006116065L, 1632979892L, 2059374117L, 2125848816L, 1822145353L, 1276473706L, 695690412L, 284920937L, 
	193476881L, 497180344L, 1042851991L, 1623635286L, 2034404760L, 2141893464L, 1922452541L, 1459690018L, 908384440L, 440556671L, 192447983L, 
	177432233L, 396873156L, 859635680L, 1410941257L, 1878769027L, 2126877714L, 2091430419L, 1792908764L, 1323257181L, 815011013L, 402961864L, 189276616L, 
	227895278L, 526416934L, 996068517L, 1504314685L, 1916363834L, 2130049081L, 2098845778L, 1838810909L, 1419757596L, 944682214L, 523619213L, 248493750L, 
	220479919L, 480514788L, 899568101L, 1374643484L, 1795706484L, 2070831947L, 2144922076L, 2008710446L, 1697280311L, 1279609907L, 842280377L, 471228634L, 235298821L, 
	174403621L, 310615252L, 622045386L, 1039715791L, 1477045321L, 1848097064L, 2084026876L, 2144811256L, 2024591755L, 1750355019L, 1374899691L, 965927904L, 593539828L, 318400020L, 182456753L, 
	174514442L, 294733942L, 568970679L, 944426007L, 1353397794L, 1725785870L, 2000925678L, 2136868945L, 2115872515L, 1945609842L, 1656347164L, 1294918257L, 916717720L, 577085494L, 323386922L, 188845397L, 
	203453183L, 373715855L, 662978533L, 1024407440L, 1402607978L, 1742240204L, 1995938775L, 2130480300L, 2130505738L, 1999519636L, 1758287480L, 1441085374L, 1090487727L, 751492655L, 465782942L, 266818953L, 176284894L, 
	188819959L, 319806062L, 561038218L, 878240324L, 1228837970L, 1567833042L, 1853542755L, 2052506744L, 2143040803L, 2117132921L, 1980604774L, 1751671447L, 1458201874L, 1134098216L, 815267954L, 535659755L, 323780648L, 200020227L, 
	202192777L, 338720923L, 567654251L, 861123823L, 1185227482L, 1504057743L, 1783665942L, 1995545049L, 2119305471L, 2144333884L, 2070349698L, 1906893226L, 1671882635L, 1389454957L, 1087355327L, 794156900L, 536583615L, 337173416L, 212466846L, 
	174991813L, 248976000L, 412432472L, 647443063L, 929870740L, 1231970371L, 1525168798L, 1782742082L, 1982152282L, 2106858851L, 2147483647L, 2102276622L, 1976894960L, 1783566378L, 1539753201L, 1266465418L, 986386904L, 721980020L, 493721320L, 318597660L, 208960702L
	};
/**
 * @brief 三角带通滤波器数组下标索引，表示（滤波器数组中的起始位置，最小列号，最大列号）
 */
static int indexM16[24][3] =
{
    {0,1,3},
	{3,2,5},
	{7,4,7},
	{11,6,10},
	{16,8,12},
	{21,11,15},
	{26,13,18},
	{32,16,22},
	{39,19,25},
	{46,23,29},
	{53,26,33},
	{61,30,37},
	{69,34,42},
	{78,38,47},
	{88,43,53},
	{99,48,59},
	{111,54,65},
	{123,60,72},
	{136,66,80},
	{151,73,88},
	{167,81,97},
	{184,89,106},
	{202,98,116},
	{221,107,127}
};

static long dctMatrix[288] = {
 	32698L, 32138L, 31029L, 29389L, 27246L, 24636L, 21605L, 18205L, 14493L, 10533L, 6393L, 2143L, -2143L, -6393L, -10533L, -14493L, -18205L, -21605L, -24636L, -27246L, -29389L, -31029L, -32138L, -32698L, 
	32488L, 30274L, 25997L, 19948L, 12540L, 4277L, -4277L, -12540L, -19948L, -25997L, -30274L, -32488L, -32488L, -30274L, -25997L, -19948L, -12540L, -4277L, 4277L, 12540L, 19948L, 25997L, 30274L, 32488L, 
	32138L, 27246L, 18205L, 6393L, -6393L, -18205L, -27246L, -32138L, -32138L, -27246L, -18205L, -6393L, 6393L, 18205L, 27246L, 32138L, 32138L, 27246L, 18205L, 6393L, -6393L, -18205L, -27246L, -32138L, 
	31651L, 23170L, 8481L, -8481L, -23170L, -31651L, -31651L, -23170L, -8481L, 8481L, 23170L, 31651L, 31651L, 23170L, 8481L, -8481L, -23170L, -31651L, -31651L, -23170L, -8481L, 8481L, 23170L, 31651L, 
	31029L, 18205L, -2143L, -21605L, -32138L, -29389L, -14493L, 6393L, 24636L, 32698L, 27246L, 10533L, -10533L, -27246L, -32698L, -24636L, -6393L, 14493L, 29389L, 32138L, 21605L, 2143L, -18205L, -31029L, 
	30274L, 12540L, -12540L, -30274L, -30274L, -12540L, 12540L, 30274L, 30274L, 12540L, -12540L, -30274L, -30274L, -12540L, 12540L, 30274L, 30274L, 12540L, -12540L, -30274L, -30274L, -12540L, 12540L, 30274L, 
	29389L, 6393L, -21605L, -32698L, -18205L, 10533L, 31029L, 27246L, 2143L, -24636L, -32138L, -14493L, 14493L, 32138L, 24636L, -2143L, -27246L, -31029L, -10533L, 18205L, 32698L, 21605L, -6393L, -29389L, 
	28378L, 0L, -28378L, -28378L, 0L, 28378L, 28378L, 0L, -28378L, -28378L, 0L, 28378L, 28378L, 0L, -28378L, -28378L, 0L, 28378L, 28378L, 0L, -28378L, -28378L, 0L, 28378L, 
	27246L, -6393L, -32138L, -18205L, 18205L, 32138L, 6393L, -27246L, -27246L, 6393L, 32138L, 18205L, -18205L, -32138L, -6393L, 27246L, 27246L, -6393L, -32138L, -18205L, 18205L, 32138L, 6393L, -27246L, 
	25997L, -12540L, -32488L, -4277L, 30274L, 19948L, -19948L, -30274L, 4277L, 32488L, 12540L, -25997L, -25997L, 12540L, 32488L, 4277L, -30274L, -19948L, 19948L, 30274L, -4277L, -32488L, -12540L, 25997L, 
	24636L, -18205L, -29389L, 10533L, 32138L, -2143L, -32698L, -6393L, 31029L, 14493L, -27246L, -21605L, 21605L, 27246L, -14493L, -31029L, 6393L, 32698L, 2143L, -32138L, -10533L, 29389L, 18205L, -24636L, 
	23170L, -23170L, -23170L, 23170L, 23170L, -23170L, -23170L, 23170L, 23170L, -23170L, -23170L, 23170L, 23170L, -23170L, -23170L, 23170L, 23170L, -23170L, -23170L, 23170L, 23170L, -23170L, -23170L, 23170L
 };
 
static void hamming(long data[], int length);
static void getDiffPara(long *mfccMatrix, int cols, int start, int height);
static void triangleFilter(sctfc_ntn_t *powersample, int samplelen, int filterNum, long lnpower[]);
static void dct(long *lnpower,int cols,int dctNum, long dct_store[]);
static void cepstrumWindow(long *input, int len);
/*
 * 函数名：mfcc
 * 作用：  对于输入的已经预处理好的音频数据提取mfcc特征
 * 输入参数：
 *   source:音频数据二维数据，每一行为一帧
 *   framelen：一帧的长度
 *   size：  帧数
 * 输出参数：
 *   mfccMatrix：mfcc的特征值，24维
*/

void mfcc(long *source, long *mfccMatrix, int framelen, int size)
{
	long frame[FRAME_LEN];
	long filterResult[FILTER_NUM];
	long dctResult[DCT_NUM];

	sctfc_ntn_t sctfc_ntn_tmp;
	sctfc_ntn_t sctfc_ntn_frame[FRAME_LEN];

    int i, j;

    for (i = 0; i < size; i++)
    {
    	//加汉明窗
    	for (j = 0; j < framelen; j++)
    	{
    		frame[j] = source[i * framelen + j];
    	}
    	hamming(frame, framelen);  //结果为Q31
    	realfft(frame, LOG_FRAME_LEN - 1);  //输入q31数据，SCALE，得q23，noscale，得q31
    	//变成q23, 这是因为每个层蝶运算每个数都右移一位，有256个数据，即8层蝶形运算，31-8 = 23
    	//计算能量
    	for (j = 0; j < framelen / 2; j++)
    	{
    		sctfc_ntn_frame[j] = real_to_sctfc_ntn(frame[2*j], 23);
    		sctfc_ntn_frame[j] = sctfc_ntn_mul(sctfc_ntn_frame[j], sctfc_ntn_frame[j]);
    		
    		sctfc_ntn_tmp = real_to_sctfc_ntn(frame[2*j + 1], 23);
    		sctfc_ntn_tmp = sctfc_ntn_mul(sctfc_ntn_tmp, sctfc_ntn_tmp);
    		
    		sctfc_ntn_frame[j] = sctfc_ntn_add(sctfc_ntn_frame[j], sctfc_ntn_tmp);
    	}//end for j
		//三角滤波器
    	triangleFilter(sctfc_ntn_frame, framelen, FILTER_NUM, filterResult); //结果为Q15
    	//离散余弦变换
    	dct(filterResult,FILTER_NUM,DCT_NUM, dctResult);  //结果为Q15
    	//又变回了q15？
    	//倒谱提升窗口
    	cepstrumWindow(dctResult, DCT_NUM);
    	
    	//把数据复制到mfccMatrix中,默认放在矩阵的最左边
    	for (j = 0; j < DCT_NUM; j++)
    	{
    		//ss = (double)dctResult[j] / 32768L;
    		mfccMatrix[i * DIMENTION + j] = dctResult[j];
    	}	
    }
    //加上差分参数
    getDiffPara(mfccMatrix, DCT_NUM, DCT_NUM, size);
}//end mfcc


/*
 * 函数名:hamming
 * 作用:  对数组加汉明窗
 * 输入参数:
 *   data:要加汉明窗的数组
 *   length:数组的长度
 *   hammingMatrix:系数矩阵
*/
static void hamming(long data[], int length)
{
	//mul32_(data, hammingMatrix, data, length);  //32位Q31乘法

	int i;
	for (i = 0; i < length; i++)
	{
		data[i] = mul32_utility(data[i], hammingMatrix[i], 31, 31, 31);
	}
}//end hamming

/*
 * 函数名:getDiffPara
 * 作用:  对一个二维数组求差分参数
 * 		 dtm[i,:]=-2*source[i-2,:]-source[i-1,:]+source[i+1,:]+2*source[i+2,:]
 * 输入参数:
 *   mfccMatrix:一个二维数组,左边的一部分已经存了标准的mfcc参数
 *   cols: 标准的mfcc参数的维数(默认从最左边开始存)
 *   start:差分参数开始存的的列数
 *   height:二维数组的行数
*/
static void getDiffPara(long *mfccMatrix, int cols, int start, int height)
{
	int i, j;
	for (i = 2; i < height -2; i++)
	{
		for (j = 0; j < cols; j++)
		{
			mfccMatrix[i * DIMENTION + j + start] = -2 * mfccMatrix[(i - 2) * DIMENTION + j]
									   - mfccMatrix[(i - 1) * DIMENTION + j]
									   + mfccMatrix[(i + 1) * DIMENTION + j]
									   + 2 * mfccMatrix[(i + 2) * DIMENTION + j];
			
		    mfccMatrix[i * DIMENTION + j + start] /= 3;
		}
	}
}//end getDiffPara

/*
 * 函数名：triangleFilter
 * 作用 : 对输入的能量谱加若干个三角滤波器
 * 输入参数：
 *   powersample:能量谱
 *   samplelen  :能量谱数组的长度
 *   filterNum：    三角滤波器的个数
 *   indexM16:   三角滤波器系数在数组melf16中的索引
 * 			     indexM16的行与滤波器系数原矩阵的行相对应，
 * 			      每行第1个元素为原矩阵该行的非0元素的第1个元素在mel16中的索引，
 * 				  第2元素为原矩阵该行的非0元素的第1个元素在该行的列数，
 * 				  第3元素为非0最后元素在该行的列数。
 *   melf16:     三角滤波器系数非0元素的集合 
 * 输出参数：
 *   lnpower:保存结果
 * 返回值：
 *   无
*/
static void triangleFilter(sctfc_ntn_t *powersample, int samplelen, int filterNum, long lnpower[])
{
	int i, j, k;
	sctfc_ntn_t ltmp[FILTER_NUM];
	sctfc_ntn_t mtmp;
	for (i = 0; i < filterNum; i++)
	{
		ltmp[i].exp = 0;
		ltmp[i].base = 0;
		for (k = indexM16[i][1]; k <= indexM16[i][2]; k++)
		{
			mtmp = real_to_sctfc_ntn(melf16[indexM16[i][0] + k - indexM16[i][1]], 31);
			mtmp = sctfc_ntn_mul(powersample[k], mtmp);
			ltmp[i] = sctfc_ntn_add(ltmp[i], mtmp);
		}//end for k
		lnpower[i] = sctfc_ntn_ln(ltmp[i], 15);
	}//end for i
}//end triangleFilter
		
/*
static long *triangleFilter(long *powersample, int samplelen, int filterNum)
{
 int i;                // 矩阵列号
 int j;                // 矩阵行号
 int k;
 long *lnpower = longtmp2;    // 滤波器输出的对数能量
 long ltmp[21];
 
	for (i = 0; i < filterNum; i++)
	{
		lnpower[i] = 0;
		k = indexM16[i][2] - indexM16[i][1] + 1;
		mul32_q15(powersample + indexM16[i][1], melf16 + indexM16[i][0], ltmp, k);
		for (j = 0; j < k; j++)
			lnpower[i] += ltmp[j];
	}

 //滤波器能量求对数,得到类似正态分布
 logn32_q15(lnpower, lnpower, filterNum);
 return lnpower;//返回对数能量
}//end triangleFilter
*/


//离散余弦变换,lnpower(长度为20)为滤波器内积对数能量（即是对数能量）,dctnum为dct变换级数
//函数返回,dct变换后的倒频参数
/*
 * 函数名：dct
 * 作用  ：对一维数组作离散余弦变换
 * 输入参数：
 *    lnpower：输入的数组
 *    cols   ：数组的长度
 *    dctNum ：变换级数矩阵
 * 	  dctMatrix:系数
 * 输出参数
 * 	  dct_store :保存结果
 * 返回值：
 *    无
*/
static void dct(long *lnpower,int cols,int dctNum, long dct_store[])
{
 long tmpResult[50];
 int i, j;
 
 for(i=0;i<dctNum;i++)
 {
	dct_store[i]=0;
	for (j = 0; j < cols; j++)
	{
		tmpResult[j] = mul32_q15(lnpower[j], dctMatrix[i * cols + j]);
	}
  	for(j=0;j<cols;j++)
  	{
   		//temp[i]+=lnpower[j]*cos(i*(j+0.5)*3.141592653589793/cols);
   		dct_store[i] += tmpResult[j];
  	}
 }
}//end dct

/*
 * 函数名：cepstrumWindow
 * 作用  ：为输入的数组加一个倒谱提升窗口
 * 输入参数：
 *  input:输入的数组
 *  len  :数组的长度
 * 输入参数：
 *  input：保存变换的值
*/
static void cepstrumWindow(long *input, int len)
{
	int i;
	long cepMatrix[12] = {
		11951L, 18725L, 24542L, 29005L, 31811L, 32768L, 31811L, 29005L, 24542L, 18725L, 11951L, 4681L
	};
	for (i = 0; i < len; i++)
	{
		input[i] = mul32_q15(input[i], cepMatrix[i]);
	}
}//end cepstrumWindow
