/*************************************
 * File name:		hmm.c
 * Description:		��HMMģ��ʶ������
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
long codebook[DIMENTION * CODE_SIZE]; //32λq15
long vector_far[DIMENTION];  //32λQ15
unsigned long codebook_dist_far[CODE_SIZE];  //32λQ10

static int code[200];

static int  isReadHmm = 0;		//�����ж��Ƿ��Ѿ���HMMģ�����ݶ���ṹ��hmms�У��������
//static long (*a)[STATE_SIZE] = (long (*)[STATE_SIZE])0x140000;
static long a[120][STATE_SIZE];  //������Ϊ�����������䣬��������������в���

static long viterbi(int mfcc[], int length, int index);
static int disteusq_min(long *vector1, long *vector2, int dim, unsigned long *dist_min);
 unsigned long disteusq(long *p1, long *p2, int d);
static void get_code(long *input, int input_len, int *code);

/**************************
 *Name: disteusq_min
 *Function: �����������������ľ��룬������ľ���ֵ�Ƚϣ���������С������true, ���򷵻�false
 *Parameters:
 *  vector1��vector2:�������������,32λq15
 *  dim:������ά��
 *  dist_min:���ڱȽϵľ���ֵ,��������Сֵ,32λq10
 *Return:  �����Ƿ������ֵС�����򷵻�true;
 **************************/
static int disteusq_min(long *vector1, long *vector2, int dim, unsigned long *dist_min)
{
    int i;
    unsigned long temp_dist = 0;
    long minus;
    for (i = 0; i < dim; i++)
    {
    	minus = vector1[i] - vector2[i];
    	minus >>= 5; //ת����Q10
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
Function: ���������ŷ�Ͼ���
Parameters:
	p1:��һ����, 32λq15
	p2:�ڶ�����, 32λq15
	d: ���ά��
return:
	������ľ���, 32λQ10
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
		minus >>= 5; //ת��ΪQ10
		returnValue += mul32_q10(minus, minus);
	}
	return returnValue;
}//end disteusq

/**************************
 *Name:get_code
 *Function:��������������飬�ҳ��������������ľ���������뱾
 *Parameters:
 *  input:�����������, 32λq15
 *  input_len:����������ĸ���
 *  codebook:�뱾��, 32λq15
 *  codebook_len:�뱾�ĸ���
 *  codebook_dist_far:ÿ���뱾��ָ��һ��ľ���, 32λq10
 *  vector_far:ָ���ĵ������, 32λq15
 *  dim:������ά��
 *  code:���ڱ�����
 *Return:void
 **************************/
static void get_code(long *input, int input_len, int *code)
{
    int i, j;
    int dist_min_index;
    int dim = DIMENTION;
    unsigned long dist_min;  //32λq10
    unsigned long dist_far;  //����������ָ����ľ���,32λq10
    
    unsigned long a, b;
    unsigned long radius[2];
    
    for (i = 0; i < input_len; i++)
    {
        //dist_far = disteusq(input + i * dim, vector_far, dim);   
        //����������뱾����һ��������ͬ
        if (i == 0)
        {
            dist_min_index = 0;
        }
        else
        {
            dist_min_index = code[i-1];
        }
        
        dist_min = disteusq(input + i * dim, codebook + dist_min_index * dim, dim);
        //����߽�
        //a = dist_min + dist_far;
        //b = mul32_q10(sqrt32_q10(dist_min), sqrt32_q10(dist_far)) << 1; //������Ϊ�˳���2
        //radius[0] = a - b;
        //radius[1] = a + b;
        for (j = 0; j < CODE_SIZE; j++)
        {
            //���������Χ���޿��ܳ�Ϊ����뱾
            //if ((codebook_dist_far[j] < radius[0]) || (codebook_dist_far[j] > radius[1]) || (j == dist_min_index))
            //{
               //continue;
            //}
            if (disteusq_min(input + i * dim, codebook + j * dim, dim, &dist_min))
            {
                dist_min_index = j;
                
                //a = dist_min + dist_far;
                //b = mul32_q10(sqrt32_q10(dist_min), sqrt32_q10(dist_far)) << 1; //������Ϊ�˳���2
                //radius[0] = a - b;
                //radius[1] = a + b;
            }//end if
        }//end for j
        code[i] = dist_min_index; //��������뱾������
    }// end for i
}//end get_code

/*********************************************
 * Function:	viterbi	
 * Description:	���������MFCC�����ڸ�����HMMģ���µĸ���
 * Calls:		None
 * Input:		mfcc(��������)������ı�������
 * 				length(MFCC��֡��)
 * 				index(hmm������)
 * Output:		none	
 * Return:		���ʵĶ���	
 **********************************************/
static long viterbi(int mfcc[], int length, int index)
{
    int i, j, k;
    long temp;
    //�����ʼ����
    for (i = 0; i < STATE_SIZE; i++)
    {
        a[0][i] = hmms[index].init[i] + hmms[index].probs[i][mfcc[0]];
    }
    //����
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
            //�����������
            a[i][j] += hmms[index].probs[j][mfcc[i]];
        }

    }
    //��۲��������һ��ĸ��ʵ����ֵ������
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
 * Description:	���ڸ�����MFCC������������п��ܵ����Ӧ�����֣�0~9��
 * Calls:		viterbi
 * Input:		mfcc(��������)
 * 				length(mfcc֡��)		
 * Output:		none	
 * Return:		���п��ܵ�����
 **********************************************/
int hmm(long mfcc[], int length, long *probability)
{
	int i;
	long maxProb, temp;
	long test[HMM_SIZE];
	int record;
	int returnValue;
	
	//���mfcc����ֵ�ĸ���������Ӧ���뱾����
	get_code(mfcc, length, code);
	//���һ��HMMģ�͵ĸ���
	maxProb = viterbi(code, length, 0);
	test[0] = maxProb;
	record = 0;
	//��ʣ���HMMģ�͵ĸ���, �������������HMMģ�͵����
	for (i = 1; i < HMM_SIZE; i++)
	{
		if (i == 9)
			i = 9;
		temp = viterbi(code, length, i);
		test[i] = temp;
		//����������0��˵�������Ѿ�������ǳ�С
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

	
