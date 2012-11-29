#include "stdafx.h"
#include "SpeexCodec.h"


#include <speex/speex.h>


//#define MAX_PCM_SIZE			1024
#define MAX_PCM_SIZE			2048



static inline int Speex_Bits_Per_Second(int band, int mode)
{
	void *tmp_coder_state;
	spx_int32_t bitrate;

	if(band == SPEEX_ULTRAWIDE)
		tmp_coder_state = speex_encoder_init(&speex_uwb_mode);
	else if(band == SPEEX_WIDE)
		tmp_coder_state = speex_encoder_init(&speex_wb_mode);
	else 
		tmp_coder_state = speex_encoder_init(&speex_nb_mode);

	speex_encoder_ctl(tmp_coder_state, SPEEX_SET_QUALITY, &mode);
	speex_encoder_ctl(tmp_coder_state, SPEEX_GET_BITRATE, &bitrate);
	speex_encoder_destroy(tmp_coder_state);
	return bitrate;
}

static inline int Speex_Bytes_Per_Frame(int band, int mode)
{
	int bits_per_frame = Speex_Bits_Per_Second(band, mode) / 50; // (20ms frame size)
	return ((bits_per_frame+7)/8); // round up
}


CSpeexCodec::CSpeexCodec(int band, int mode)
{
	m_band = band;
	m_mode = mode;
	
	encoder_state = NULL;
	decoder_state = NULL;

	bits = new SpeexBits;
	speex_bits_init(bits);
	m_nFrameSize = 0;
}

CSpeexCodec::~CSpeexCodec()
{
	speex_bits_destroy(bits);
	delete bits;

	if(encoder_state)
		speex_encoder_destroy(encoder_state); 

	if(decoder_state)
		speex_decoder_destroy(decoder_state);
}


BOOL CSpeexCodec::InitEncoder()
{
	spx_int32_t tmp = 0;

	if(m_band == SPEEX_ULTRAWIDE)
	{
		encoder_state = speex_encoder_init(&speex_uwb_mode);
		tmp = 1;
		speex_encoder_ctl(encoder_state, SPEEX_SET_COMPLEXITY, &tmp);
	}
	else if(m_band == SPEEX_WIDE)
	{
		encoder_state = speex_encoder_init(&speex_wb_mode);
		tmp = 3;
		speex_encoder_ctl(encoder_state, SPEEX_SET_COMPLEXITY, &tmp);
	}
	else 
	{
		encoder_state = speex_encoder_init(&speex_nb_mode);
		tmp = 1;
		speex_encoder_ctl(encoder_state, SPEEX_SET_COMPLEXITY, &tmp);
	}

    speex_encoder_ctl(encoder_state, SPEEX_SET_QUALITY, &m_mode);

	tmp = 128000;
	speex_encoder_ctl(encoder_state, SPEEX_SET_VBR_MAX_BITRATE, &tmp);
	speex_encoder_ctl(encoder_state, SPEEX_SET_ABR, &tmp);

	tmp = 1;
	speex_encoder_ctl(encoder_state, SPEEX_SET_VBR, &tmp);

	tmp = 1;
	speex_encoder_ctl(encoder_state, SPEEX_SET_VAD, &tmp);

	tmp = 1;
	speex_encoder_ctl(encoder_state, SPEEX_SET_DTX, &tmp);

	speex_encoder_ctl(encoder_state, SPEEX_GET_FRAME_SIZE, &m_nFrameSize);

	return TRUE;
}

BOOL CSpeexCodec::InitDecoder()
{
//	void * tmp_coder_state;

	if(m_band == SPEEX_ULTRAWIDE)
	{
		decoder_state = speex_decoder_init(&speex_uwb_mode);
//		tmp_coder_state = speex_encoder_init(&speex_uwb_mode);
	}
	else if(m_band == SPEEX_WIDE)
	{
		decoder_state = speex_decoder_init(&speex_wb_mode);
//		tmp_coder_state = speex_encoder_init(&speex_wb_mode);
	}
	else 
	{
		decoder_state = speex_decoder_init(&speex_nb_mode);
//		tmp_coder_state = speex_encoder_init(&speex_nb_mode);
	}

	spx_int32_t tmp = 0;
	speex_decoder_ctl(decoder_state, SPEEX_SET_ENH, &tmp);

	speex_decoder_ctl(decoder_state, SPEEX_GET_FRAME_SIZE, &m_nFrameSize);

//	speex_encoder_destroy(tmp_coder_state);

	return TRUE;
}

int CSpeexCodec::Encode(const LPBYTE pSrcData, const unsigned nSrcSize, LPBYTE pDstBuf)
{
	speex_bits_reset(bits);
	speex_encode_int(encoder_state, (spx_int16_t *)pSrcData, bits);
	speex_bits_insert_terminator(bits);

	return speex_bits_write(bits, (char *)pDstBuf, MAX_PCM_SIZE);
}

int CSpeexCodec::Decode(const LPBYTE pSrcData, const unsigned nSrcSize, LPBYTE pDstBuf)
{
	//speex_bits_read_from(bits, (char *)pSrcData, GetFrameSize());
	speex_bits_read_from(bits, (char *)pSrcData, nSrcSize);

	int nResult = -1;

	if (pSrcData == NULL || nSrcSize == 0)
		nResult = speex_decode_int(decoder_state, NULL, (spx_int16_t *)pDstBuf);
	else
		nResult = speex_decode_int(decoder_state, bits, (spx_int16_t *)pDstBuf);

	if (nResult == -1)
	{
		TRACE(_T("Speex\tDecoding error: nResult == -1\n"));
		return -1;
	}
	else if (nResult == -2)
	{
		TRACE(_T("Speex\tDecoding error: corrupted stream?\n"));
		return -1;
	}
	else if (speex_bits_remaining(bits) < 0)
	{
		TRACE(_T("Speex\tDecoding overflow: corrupted stream?\n"));
		return -1;
	}

	return nResult;
}