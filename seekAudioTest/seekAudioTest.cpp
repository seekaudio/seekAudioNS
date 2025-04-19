// seekAudioTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "time.h"
#if WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "webrtc/modules/audio_processing/ns/include/noise_suppression.h"
#include "seekaudio.h"

typedef NoiseHandle*(*SeekAudioNsCreate)();
typedef void (*SeekAudioNsFree)(NoiseHandle*);
typedef int(*SeekAudioNsInit)(NoiseHandle*,int, MODEL_TYPE);
typedef int(*SeekAudioNsProcess)(NoiseHandle*, const float*, float*);

static double seekAudioNoiseTest(const char* noiseFilePath, const char* denoiseFilePath,int mode)
{
	FILE *fin = NULL;
	FILE *fout = NULL;
	long startTime = 0;
	long endTime = 0;
	long costTime = 0;
	int audioLenghMS = 0;
	int i = 0;
	int count = 0;
	NoiseHandle*denoise = NULL;
	short buffer16k[160];
	float *buffer = NULL;
	float *out = NULL;

	fin = fopen(noiseFilePath, "rb");
	
	if (!fin)
	{
		printf("seekAudioNoiseTest open file %s failed\n ", noiseFilePath);
		return -1;
	}

	fout = fopen(denoiseFilePath, "wb");
	if (!fout)
	{
		printf("seekAudioNoiseTest open file %s failed\n ", denoiseFilePath);
		return -1;
	}

	const char* utf8String = "seekaudio.dll";
	int length = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[length];
	MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, wstr, length);
	HMODULE mNoiseModule = LoadLibrary(wstr);

	SeekAudioNsCreate m_pSeekAudioNsCreate = (SeekAudioNsCreate)::GetProcAddress(mNoiseModule, "SeekAudioNs_Create");
	SeekAudioNsFree   m_pSeekAudioNsFree = (SeekAudioNsFree)::GetProcAddress(mNoiseModule, "SeekAudioNs_Free");
	SeekAudioNsInit   m_pSeekAudioNsInit = (SeekAudioNsInit)::GetProcAddress(mNoiseModule, "SeekAudioNs_Init");
	SeekAudioNsProcess   m_pSeekAudioNsProcess = (SeekAudioNsProcess)::GetProcAddress(mNoiseModule, "SeekAudioNs_Process");

	buffer = (float*)malloc(160 * sizeof(float));
	out = (float*)malloc(160 * sizeof(float));

	denoise = m_pSeekAudioNsCreate();

	m_pSeekAudioNsInit(denoise, 16000, (MODEL_TYPE)mode);

	startTime = GetTickCount();

	while (1) {
		int outLen = 0;
		if (fin)
			fread(buffer16k, sizeof(short), 160, fin);
		if (feof(fin)) break;

		count++;

		for (i = 0; i < 160; i++)
			buffer[i] = (float)buffer16k[i];

		m_pSeekAudioNsProcess(denoise, buffer, out);

		for (i = 0; i < 160; i++)
			buffer16k[i] = (short)out[i];

		if (fout)
			fwrite(buffer16k, sizeof(short), 160, fout);
	}

	endTime = GetTickCount();

	m_pSeekAudioNsFree(denoise);

	free(out);
	free(buffer);
	costTime = endTime - startTime;
	audioLenghMS = 1000 * count * 1 / 100;

	if (fin)
		fclose(fin);
	if (fout)
		fclose(fout);

	printf("this audio file lengh is %d ms. the noise reduction take %ld ms, processing 1 ms of audio requires %f ms of time\n", audioLenghMS, costTime, costTime / (float)audioLenghMS);

	if (!FreeLibrary(mNoiseModule))
		return 0;

	return 0;

}


static double webrtcNoiseTest(const char* noiseFilePath, const char* denoiseFilePath, int mode)
{
	FILE *fin = NULL;
	FILE *fout = NULL;
	long startTime = 0;
	long endTime = 0;
	long costTime = 0;
	int audioLenghMS = 0;
	int i = 0;
	int count = 0;
	NsHandle*denoise = NULL;
	short buffer16k[160];
	float *buffer = NULL;
	float *out = NULL;
	
	fin = fopen(noiseFilePath, "rb");
	
	if (!fin)
	{
		printf("webrtcNoiseTest open file %s failed\n ", noiseFilePath);
		return -1;
	}

	fout = fopen(denoiseFilePath, "wb");
	if (!fout)
	{
		printf("webrtcNoiseTest open file %s failed\n ", denoiseFilePath);
		return -1;
	}

	buffer = (float*)malloc(160 * sizeof(float));
	out = (float*)malloc(160 * sizeof(float));

	denoise = WebRtcNs_Create();
	WebRtcNs_Init(denoise, 16000);

	WebRtcNs_set_policy(denoise, 3);
	startTime = GetTickCount();

	while (1) {
		int outLen = 0;
		if (fin)
			fread(buffer16k, sizeof(short), 160, fin);
		if (feof(fin)) break;

		count++;

		for (i = 0; i < 160; i++)
			buffer[i] = (float)buffer16k[i];

		WebRtcNs_Analyze(denoise, buffer);
		WebRtcNs_Process(denoise, (float* const*)&buffer, 1, (float* const*)&out);

		for (i = 0; i < 160; i++)
			buffer16k[i] = (short)out[i];

		if (fout)
			fwrite(buffer16k, sizeof(short), 160, fout);
	}

	endTime = GetTickCount();
	
	WebRtcNs_Free(denoise);
	free(out);
	free(buffer);
	costTime = endTime - startTime;
	audioLenghMS = 1000 * count * 1 / 100;

	if (fin)
		fclose(fin);
	if (fout)
		fclose(fout);

	printf("this audio file lengh is %d ms. the noise reduction take %ld ms, processing 1 ms of audio requires %f ms of time\n", audioLenghMS, costTime, costTime / (float)audioLenghMS);

	return 0;
}

int main(int argc, char **argv)
{	
	int mode = 0;

	if (argc != 4) {
		fprintf(stderr, "usage: %s <noisy speech filepath> <output denoised filepath> <mode>\n", argv[0]);
		return 1;
	}

	mode = atoi(argv[3]);

	if (mode == 0)
	{
		fprintf(stderr, "use seekauido lite model to denoise the audio\n");
		seekAudioNoiseTest(argv[1], argv[2], mode);
	}
	else if (mode == 1)
	{
		fprintf(stderr, "use seekauido main model to denoise the audio\n");
		seekAudioNoiseTest(argv[1], argv[2], mode);
	}
	else if (mode == 2)
	{
		fprintf(stderr, "use webrtc ns to denoise the audio\n");
		webrtcNoiseTest(argv[1], argv[2], mode);
	}
	else
	{
		fprintf(stderr, "the mode %d should be <= 2\n", mode);
		return 0;
	}

	return 0;
}