#ifndef __PCM_DEFINE__
#define __PCM_DEFINE__

typedef struct _tagPCMRIFF 
{
	BYTE	chunkID[4];
	UINT	chunckSize;
	BYTE	format[4];

}PCM_RIFF, *PPCM_RIFF;


typedef struct _tagPCMFMT
{
	BYTE	subChunk1ID[4];
	UINT	subChunk1Size;
	SHORT	audioFormat;
	SHORT	numOfChannels;
	UINT	sampleRate;
	UINT	ByteRate;
	SHORT	BlockAlign;
	SHORT	BitsPerSmaple;

}PCM_FMT, *PPCM_FMT;


typedef struct _tagPCMData
{
	BYTE subChunk2ID[4];
	UINT subChunk2Size;

}PCM_DATA, *PPCM_DATA;


typedef struct _tagPCMHeaders
{
	PCM_RIFF	riff;
	PCM_FMT		fmt;
	PCM_DATA	data;

}PCM_HEADERS, *PPCM_HEADERS;

#endif