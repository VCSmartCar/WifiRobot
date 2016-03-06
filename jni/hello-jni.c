/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <android/log.h>


#include "hmm.h"
#include "mfcc.h"
#include "dtw.h"
#define DHMM_RECOG 0
#define DTW_RECOG 1

#define LOGI(TAG, MES) __android_log_print(ANDROID_LOG_INFO  , TAG, MES)
/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
char global_var[50];

extern Hmm hmms[HMM_SIZE];
extern long codebook[DIMENTION * CODE_SIZE]; //32位q15
extern long vector_far[DIMENTION];  //32位Q15
extern unsigned long codebook_dist_far[CODE_SIZE];  //32位Q10
long mfccMatrix[3000];

extern long models[HMM_SIZE][3000];
extern int model_len[HMM_SIZE];
jint Java_elechen_android_wifirobot_recognize_MainActivity_recognize(JNIEnv* env, jobject thiz, jintArray arr, jint size, jshort recog_type)
{
	jint result;
	long value;
	jint *source = (*env)->GetIntArrayElements(env, arr, NULL);
	mfcc((long *)source, mfccMatrix, 256, size);
	if (recog_type == DHMM_RECOG)
	{
		result = hmm(mfccMatrix + 2 * DIMENTION, size - 4, &value);
		sprintf(global_var, "概率为%d", value);
		LOGI("test", global_var);
	}
	else
	{
		result = dtw(mfccMatrix + 2 * DIMENTION, size - 4, &value);
		sprintf(global_var, "距离为%d", value);
		LOGI("test", global_var);
	}
	(*env)->ReleaseIntArrayElements(env, arr, source, 0);

	return result;
}//end recognize

//检查是否读取正确
void Java_elechen_android_wifirobot_recognize_MainActivity_checkDhmm(JNIEnv* env, jobject thiz)
{
	int i;
	char output[50] = "hello";
	sprintf(output, "%d %d %d", codebook[0], vector_far[0], codebook_dist_far[0]);
	LOGI("test", output);
}//end


/*********************************************
 * Function:	readHmm
 * Description:	从文件中将HMM模型读出并放到HMM数组中
 * Calls:		fopen,
 * 				fscanf,
 * 				fclose
 * Input:		path(文件的路径)
 * Output:	    hmms(储存HMM模型)
 * 				isReadHmm
 * Return:		如果读取成功，返回1，否则返回0
 **********************************************/
jboolean Java_elechen_android_wifirobot_recognize_MainActivity_readHmm(JNIEnv* env, jobject thiz, jstring jpath)
{
	FILE *fp;
	const char *cpath;
	jboolean iscopy;
	long tmp;
	int i, j, k;
	cpath = (*env)->GetStringUTFChars(env, jpath, &iscopy);
	fp = fopen(cpath, "rb");
	(*env)->ReleaseStringUTFChars(env, jpath, cpath);
	if (fp == NULL)
	{
		printf("fault to read the Hmm file");
		return JNI_FALSE;
	}
	else
	{
		//按结构体的格式把数据读入结构体中
		for (i = 0; i < HMM_SIZE; i++)
        {
            for (j = 0; j < STATE_SIZE; j++)
            {
                fscanf(fp, "%ld", &tmp);
                hmms[i].init[j] = tmp;
            }//end for j
            for (j = 0; j < STATE_SIZE; j++)
            {
                for (k = 0; k < STATE_SIZE; k++)
                {
                    fscanf(fp, "%ld", &tmp);
                    hmms[i].trans[j][k] = tmp;
                }
            }//end for j
            for (j = 0; j < STATE_SIZE; j++)
            {
                for (k = 0; k < CODE_SIZE; k++)
                {
                	fscanf(fp, "%ld", &tmp);
                	hmms[i].probs[j][k] = tmp;
                }//end for k
            }//end for j
        }//end for i;
		fclose(fp);
		return JNI_TRUE;
	}//end if
}//end readHmm

/*********************************************
 * Function:	readCodebook
 * Description:	从文件中将码本，判定点，码本与判定点的距离按循序读出来
 * Calls:		fopen,
 * 				fscanf,
 * 				fclose
 * Input:		path(文件的路径)
 * Output:	    codebook(储存HMM模型)
 * 				vector_far
 * 				codebook_dist_far
 * Return:		如果读取成功，返回1，否则返回0
 **********************************************/
jboolean Java_elechen_android_wifirobot_recognize_MainActivity_readCodebook(JNIEnv* env, jobject thiz, jstring jpath)
 {
 	FILE *f;
 	int i, j;
 	long tmp;
 	jboolean iscopy;
 	const char *cpath = (*env)->GetStringUTFChars(env, jpath, &iscopy);
 	f = fopen(cpath, "rb");
 	(*env)->ReleaseStringUTFChars(env, jpath, cpath);

 	if (f == NULL)
 	{
 		printf("读取失败！！\n");
 		return JNI_FALSE;
 	}
 	for (i = 0; i < CODE_SIZE; i++)
 	{
 		for (j = 0; j < DIMENTION; j++)
 		{
 			fscanf(f, "%ld", &tmp);
 			codebook[i * DIMENTION + j] = tmp;
 		}
 	}//end for i
 	for (i = 0; i < DIMENTION; i++)
 	{
 		fscanf(f, "%ld", &tmp);
 		vector_far[i] = tmp;
 	}//end for i
 	for (i = 0; i < CODE_SIZE; i++)
 	{
 		fscanf(f, "%ld", &tmp);
 		codebook_dist_far[i] = tmp;
 	}//end for i
 	fclose(f);
 	return JNI_TRUE;
 }//end readCodebook

void Java_elechen_android_wifirobot_recognize_MainActivity_readDtw(JNIEnv* env, jobject thiz, jintArray model, jint len, jint index)
{
	int i;
	jint *model_p;

	model_len[index] = len;
	model_p = (*env)->GetIntArrayElements(env, model, NULL);

	mfcc((long *)model_p, (long *)models[index], 256, len);
	(*env)->ReleaseIntArrayElements(env, model, model_p, JNI_ABORT);
}//end readDtw

 void Java_elechen_android_wifirobot_recognize_MainActivity_readMatrix(JNIEnv* env, jobject thiz, jstring jpath, jintArray input_m, jint m, jint n)
 {
 	FILE *f;
 	const char *cpath;
 	jint *matrix;

 	jboolean iscopy;
 	int i, j;
 	long tmp;

 	cpath = (*env)->GetStringUTFChars(env, jpath, &iscopy);
 	f = fopen(cpath, "rb");
 	(*env)->ReleaseStringUTFChars(env, jpath, cpath);

 	matrix = (*env)->GetIntArrayElements(env, input_m, NULL);
 	if (f == NULL)
 	{
 		printf("读取文件失败");
 		return;
 	}
 	for (i = 0; i < m; i++)
 	{
 		for (j = 0; j < n; j++)
 		{
 			fscanf(f, "%ld", &tmp);
 			matrix[i * n + j] = tmp;
 		}
 	}//end for i

 	(*env)->ReleaseIntArrayElements(env, input_m, matrix, 0);
 	fclose(f);
 }//end readMfcc
