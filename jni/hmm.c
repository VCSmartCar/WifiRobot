/*************************************
 * File name:		hmm.c
 * Description:		用HMM模型识别语音
 * Author:			GouGin
 * Version:			1.0
 * Date:			2012-8-30
 * History:			
 *************************************/
#include "hmm.h"
#include "mfcc.h"
#include <stdio.h>
#include <math.h>
#include "fix_math_func.h"


//#pragma DATA_SECTION(hmms, "HmmFile0");

Hmm hmms[HMM_SIZE];
long codebook[DIMENTION * CODE_SIZE]; //32位q15
long vector_far[DIMENTION];  //32位Q15
unsigned long codebook_dist_far[CODE_SIZE];  //32位Q10

static int code[200];

static int  isReadHmm = 0;		//对于判断是否已经把HMM模型数据读入结构体hmms中，避免出错
//static long (*a)[STATE_SIZE] = (long (*)[STATE_SIZE])0x140000;
static long a[120][STATE_SIZE];  //曾经因为用了上面的语句，导致晓峰噶机运行不了

static long viterbi(int mfcc[], int length, int index);
static int disteusq_min(long *vector1, long *vector2, int dim, unsigned long *dist_min);
 unsigned long disteusq(long *p1, long *p2, int d);
static void get_code(long *input, int input_len, int *code);

/**************************
 *Name: disteusq_min
 *Function: 求出输入的两个向量的距离，与输入的距离值比较，如果与比其小，返回true, 否则返回false
 *Parameters:
 *  vector1，vector2:输入的两个向量,32位q15
 *  dim:向量的维度
 *  dist_min:用于比较的距离值,并储存最小值,32位q10
 *Return:  距离是否比输入值小，是则返回true;
 **************************/
static int disteusq_min(long *vector1, long *vector2, int dim, unsigned long *dist_min)
{
    int i;
    unsigned long temp_dist = 0;
    long minus;
    for (i = 0; i < dim; i++)
    {
    	minus = vector1[i] - vector2[i];
    	minus >>= 5; //转换成Q10
        temp_dist += mul32_q10(minus, minus);
        if (temp_dist >= *dist_min)
            break;
    }
    if (i < dim)
        return 0;
    else
    {
        *dist_min = temp_dist;
        return 1;
    }
}//end disteusq_min

/*************************
Name: disteusq
Function: 求两个点的欧氏距离
Parameters:
	p1:第一个点, 32位q15
	p2:第二个点, 32位q15
	d: 点的维数
return:
	两个点的距离, 32位Q10
*************************/
 unsigned long disteusq(long *p1, long *p2, int d)
{
	unsigned long returnValue = 0;
	int i;
	long minus;
	if (d < 0)
		return -1;
	for (i = 0; i < d; i++)
	{
		minus = p1[i] - p2[i];
		minus >>= 5; //转换为Q10
		returnValue += mul32_q10(minus, minus);
	}
	return returnValue;
}//end disteusq

/**************************
 *Name:get_code
 *Function:对于输入的向量组，找出所有输入向量的距离最近的码本
 *Parameters:
 *  input:输入的向量组, 32位q15
 *  input_len:输入的向量的个数
 *  codebook:码本集, 32位q15
 *  codebook_len:码本的个数
 *  codebook_dist_far:每个码本与指定一点的距离, 32位q10
 *  vector_far:指定的点的坐标, 32位q15
 *  dim:向量的维数
 *  code:用于保存结果
 *Return:void
 **************************/
static void get_code(long *input, int input_len, int *code)
{
    int i, j;
    int dist_min_index;
    int dim = DIMENTION;
    unsigned long dist_min;  //32位q10
    unsigned long dist_far;  //输入向量与指定点的距离,32位q10
    
    unsigned long a, b;
    unsigned long radius[2];
    
    for (i = 0; i < input_len; i++)
    {
        //dist_far = disteusq(input + i * dim, vector_far, dim);   
        //假设最近的码本与上一个向量相同
        if (i == 0)
        {
            dist_min_index = 0;
        }
        else
        {
            dist_min_index = code[i-1];
        }
        
        dist_min = disteusq(input + i * dim, codebook + dist_min_index * dim, dim);
        //求出边界
        //a = dist_min + dist_far;
        //b = mul32_q10(sqrt32_q10(dist_min), sqrt32_q10(dist_far)) << 1; //左移是为了乘以2
        //radius[0] = a - b;
        //radius[1] = a + b;
        for (j = 0; j < CODE_SIZE; j++)
        {
            //如果超过范围，无可能成为最近码本
            //if ((codebook_dist_far[j] < radius[0]) || (codebook_dist_far[j] > radius[1]) || (j == dist_min_index))
            //{
               //continue;
            //}
            if (disteusq_min(input + i * dim, codebook + j * dim, dim, &dist_min))
            {
                dist_min_index = j;
                
                //a = dist_min + dist_far;
                //b = mul32_q10(sqrt32_q10(dist_min), sqrt32_q10(dist_far)) << 1; //左移是为了乘以2
                //radius[0] = a - b;
                //radius[1] = a + b;
            }//end if
        }//end for j
        code[i] = dist_min_index; //保存最近码本的索引
    }// end for i
}//end get_code

/*********************************************
 * Function:	viterbi	
 * Description:	求出给定的MFCC参数在给定的HMM模型下的概率
 * Calls:		None
 * Input:		mfcc(参数矩阵)量化后的标量数组
 * 				length(MFCC的帧数)
 * 				index(hmm的索引)
 * Output:		none	
 * Return:		概率的对数	
 **********************************************/
static long viterbi(int mfcc[], int length, int index)
{
    int i, j, k;
    long temp;
    //计算初始概率
    for (i = 0; i < STATE_SIZE; i++)
    {
        a[0][i] = hmms[index].init[i] + hmms[index].probs[i][mfcc[0]];
    }
    //迭代
    for (i = 1; i < length; i++)
    {
    	
    	a[i][0] = hmms[index].trans[0][0] + a[i-1][0];
    	a[i][0] += hmms[index].probs[j][mfcc[i]];
    	/*
    	for (j = 1; j < STATE_SIZE; j++)
    	{
    		a[i][j] = hmms[index].trans[j-1][j] + a[i-1][j-1];
    		temp = hmms[index].trans[j][j] + a[i-1][j];
    		if ((a[i][j] > 0) || ((temp > a[i][j]) && temp < 0))
    			a[i][j] = temp;
    		a[i][j] += hmms[index].probs[j][mfcc[i]];
    	}
		*/
        for (j = 0; j < STATE_SIZE; j++)
        {        
        	a[i][j] = hmms[index].trans[0][j] + a[i-1][0];
            for (k = 1; k < STATE_SIZE; k++)
            {
            	temp = hmms[index].trans[k][j] + a[i-1][k];
            	if ((a[i][j] > 0) || ((temp > a[i][j]) && temp < 0))
            		a[i][j] = temp;
            }
            //加上输出概率
            a[i][j] += hmms[index].probs[j][mfcc[i]];
        }

    }
    //求观察序列最后一项的概率的最大值并返回
    temp = a[length - 1][0];
    for (i = 1; i < STATE_SIZE; i++)
    {
        if ((temp > 0) || ((a[length - 1][i] > temp) && (a[length - 1][i] < 0)))
            temp = a[length - 1][i];
    }
    return temp;
}//end viterbi

/*********************************************
 * Function:	hmm	
 * Description:	对于给定的MFCC参数，求出最有可能的相对应的数字（0~9）
 * Calls:		viterbi
 * Input:		mfcc(参数矩阵)
 * 				length(mfcc帧数)		
 * Output:		none	
 * Return:		最有可能的数字
 **********************************************/
int hmm(long mfcc[], int length, long *probability)
{
	int i;
	long maxProb, temp;
	long test[HMM_SIZE];
	int record;
	int returnValue;
	
	//求出mfcc特征值的各个向量对应的码本索引
	get_code(mfcc, length, code);
	//求第一个HMM模型的概率
	maxProb = viterbi(code, length, 0);
	test[0] = maxProb;
	record = 0;
	//求剩余的HMM模型的概率, 并求出概率最大的HMM模型的序号
	for (i = 1; i < HMM_SIZE; i++)
	{
		if (i == 9)
			i = 9;
		temp = viterbi(code, length, i);
		test[i] = temp;
		//如果结果大于0，说明概率已经溢出，非常小
		if ((maxProb > 0) ||((temp > maxProb) && (temp < 0))) 
		{
			maxProb = temp;
			record = i;
		}
	}

	*probability = maxProb;
	returnValue = record;
	if (maxProb < -4000000L)
		returnValue = -1;
	return returnValue;
}//end hmm

	
