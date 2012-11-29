#include "StdAfx.h"
#include "SpeexEncoder.h"
#include <speex/speex.h>


SpeexEncoder::SpeexEncoder(CString savePath)
	:m_strSavePath(savePath)
{
	SpeexBits bits;
	speex_bits_init(&bits);

	void *enc_state = speex_encoder_init(&speex_nb_mode);
}


SpeexEncoder::~SpeexEncoder(void)
{
}
