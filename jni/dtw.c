#include "hmm.h"
#include "mfcc.h"
#include "fix_math_func.h"
#include "stdlib.h"
#include "stdio.h"

#define INT_32_MAX 0x7fffffff
long models[HMM_SIZE][3000];
int model_len[HMM_SIZE];
#include <android/log.h>
#define LOGI(TAG, MES) __android_log_print(ANDROID_LOG_INFO  , TAG, MES)

char disp[100];

long cur_array[MAX_FRAMES];
long pre_array[MAX_FRAMES];
long pre_pre_array[MAX_FRAMES];

static unsigned long disteusq(long *p1, long *p2, int dim, int q_in, int q_out)
{
	unsigned long returnValue = 0;
	int i;
	long minus, tmp;
	if (dim < 0)
		return -1;
	for (i = 0; i < dim; i++)
	{
		minus = p1[i] - p2[i];
		tmp = mul32_utility(minus, minus, q_in, q_in, q_out);
		returnValue += tmp;
		//sprintf(disp, "%f ^2 = %f", (double)minus / ((long)1<<q_in), (double)tmp / ((long)1<<q_out));
		//LOGI("disteusq", disp );
	}
	return returnValue;
}//end disteusq

unsigned long fast_dtw(long *input, int length, int index, int dim, int q_in, int q_out)
{
	int m = length - 1;
	int n = model_len[index] - 1;
	int high_x = ((n<<1) - m) / 3;
	int high_y = high_x << 1;
	int low_y = ((m << 1) - n) / 3;
	int low_x = low_y << 1;
	int x_min, x_max;
	int x, y;
	long min_dist;
	long *cur_dist = cur_array;
	long *pre_dist = pre_array;
	long *pre_pre_dist = pre_pre_array;
	long *tmp;
	for (y = 0; y <= n; y++)
	{
		if (y <= high_y)
		{	x_min = y >> 1;}   //x_min = y / 2
		else
		{	x_min = ((y - n)<<1) + m;}   //x_min = 2(y - high_y) + high_x

		if (y <= low_y)
		{	x_max = y<<1;}		//x_min = 2y
		else
		{	x_max = ((y - n)>>1) + m;}		//x_max = (y - low_y) / 2 + low_x

		//把没赋值的赋最大值

		for (x = 0; x < x_min; x++)
			cur_dist[x] = INT_32_MAX;
		if (x_max < m)
		{
			for (x = x_max + 1; x <= m; x++)
				cur_dist[x] = INT_32_MAX;
		}

		for (x = x_min; x <= x_max; x++)
		{
			cur_dist[x] = disteusq(input + dim * x, (long *)models[index] + dim * y, dim, q_in, q_out);
			min_dist = INT_32_MAX;
			//动态规划，求最小值
			if ((x >= 1) && (y >= 1))
			{
				if (min_dist > pre_dist[x - 1])
					min_dist = pre_dist[x - 1];
				//if (min_dist > pre_dist[x])
				//	min_dist = pre_dist[x];
				if (x >= 2)
				{
					if (min_dist > pre_dist[x - 2])
					{	min_dist = pre_dist[x - 2];}
				}
				if (y >= 2)
				{
					if (min_dist > pre_pre_dist[x - 1])
					{	min_dist = pre_pre_dist[x - 1];}
				}
			}//end if

			if (min_dist < INT_32_MAX)
				cur_dist[x] += min_dist;

			//sprintf(disp, "y = %d, x = %d, min = %f, dist = %f", y, x, (double)min_dist / ((long)1<<q_out), (double)cur_dist[x] / ((long)1<<q_out));
			//LOGI("fastdtw", disp);
		}//end for x

		tmp = pre_pre_dist;
		pre_pre_dist = pre_dist;
		pre_dist = cur_dist;
		cur_dist = tmp;
	}//end for y

	min_dist = pre_dist[m];
	return min_dist;
}//end for fast_dtw

int dtw(long *input, int len, long *distance)
{
	int i = 0;
	long cur_dist;
	long min_dist;
	int record = 0;

	min_dist = fast_dtw(input, len, 0, DIMENTION, 15, 15);
	record = 0;
	for (i = 1; i < HMM_SIZE; i++)
	{
		cur_dist = fast_dtw(input, len, i, DIMENTION, 15, 15);
		if (min_dist > cur_dist)
		{
			min_dist = cur_dist;
			record = i;
		}
	}//end for
	*distance = min_dist;

	sprintf(disp, "the value of max_32_int is %ld", INT_32_MAX);
	LOGI("test", disp);

	return record;
}//end dtw
