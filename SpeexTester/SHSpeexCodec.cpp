#include "StdAfx.h"
#include "SHSpeexCodec.h"



const SpeexMode *pSpeex_uwb_mode = speex_lib_get_mode (SPEEX_MODEID_WB);
const SpeexMode *pSpeex_wb_mode = speex_lib_get_mode (SPEEX_MODEID_WB);
const SpeexMode *pSpeex_nb_mode = speex_lib_get_mode (SPEEX_MODEID_NB);

SHSpeexCodec::SHSpeexCodec(INT band, INT quality)
{
 	encoder_state = decoder_state = NULL;

	m_nBand = band;
	m_nQuality = quality;

	m_pBuffer = NULL;
	bits = new SpeexBits;
}


SHSpeexCodec::~SHSpeexCodec(void)
{
	if (bits)
	{
		//speex_bits_destroy(bits);
		delete bits;
		bits = NULL;
	}
	

	if(encoder_state)
		speex_encoder_destroy(encoder_state); 

	if(decoder_state)
		speex_decoder_destroy(decoder_state);

	if (m_pBuffer) 
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}


void SHSpeexCodec::InitEncoder()
{
	spx_int32_t tmp = 0;

	if(m_nBand == SPEEX_ULTRAWIDE)
	{
		encoder_state = speex_encoder_init(pSpeex_uwb_mode);
		tmp = 1;
		speex_encoder_ctl(encoder_state, SPEEX_SET_COMPLEXITY, &tmp);
	}
	else if(m_nBand == SPEEX_WIDE)
	{
		encoder_state = speex_encoder_init(pSpeex_wb_mode);
		tmp = 3;
		speex_encoder_ctl(encoder_state, SPEEX_SET_COMPLEXITY, &tmp);
	}
	else 
	{
		encoder_state = speex_encoder_init(pSpeex_nb_mode);
		tmp = 1;
		speex_encoder_ctl(encoder_state, SPEEX_SET_COMPLEXITY, &tmp);
	}
	tmp = 1;
	speex_encoder_ctl(encoder_state, SPEEX_SET_VBR, &tmp);
	speex_encoder_ctl(encoder_state, SPEEX_SET_QUALITY, &m_nQuality);
	speex_encoder_ctl(encoder_state, SPEEX_GET_FRAME_SIZE, &m_nFrameSize);
	speex_bits_init(bits);

	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
	m_pBuffer = new float[m_nFrameSize];
}

int SHSpeexCodec::Encode(SHORT* pSrcData, UINT uCount, BYTE** pOutput)
{
	for (int i = 0; i < m_nFrameSize; i++)
		m_pBuffer[i] = pSrcData[i];
		

	speex_bits_reset(bits);
	speex_encode(encoder_state, m_pBuffer, bits);
	int nbBytes = speex_bits_write(bits, m_cbits, 200);
	*pOutput = (BYTE *)m_cbits;

	return nbBytes;
}


void SHSpeexCodec::InitDecoder()
{
	if(m_nBand == SPEEX_ULTRAWIDE)
	{
		decoder_state = speex_decoder_init(pSpeex_uwb_mode);
	}
	else if(m_nBand == SPEEX_WIDE)
	{
		decoder_state = speex_decoder_init(pSpeex_wb_mode);
	}
	else 
	{
		decoder_state = speex_decoder_init(pSpeex_nb_mode);
	}

	spx_int32_t tmp = 0;
	speex_decoder_ctl(decoder_state, SPEEX_SET_ENH, &tmp);
	speex_decoder_ctl(decoder_state, SPEEX_GET_FRAME_SIZE, &m_nFrameSize);
	speex_bits_init(bits);

	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
	m_pBuffer = new float[m_nFrameSize];
}

int SHSpeexCodec::Decode(BYTE* pSrcData, UINT uCount, SHORT* output)
{
	int ret = 0;
	
	speex_bits_read_from(bits, (char *)pSrcData, uCount); 
	ret = speex_decode(decoder_state, bits, m_pBuffer);

	for (int i = 0; i < m_nFrameSize; i++)
		output[i] = m_pBuffer[i];

	return ret;
}