#pragma once

#include <speex/speex.h>

#define SPEEX_NARROW							1
#define SPEEX_WIDE								2
#define SPEEX_ULTRAWIDE							3

class SHSpeexCodec
{
private:
	INT			m_nBand;
	INT			m_nQuality;
	INT			m_nFrameSize;
	void*		encoder_state;
	void*		decoder_state;

	float*		m_pBuffer;
	SpeexBits*	bits;
	char		m_cbits[200];		

	

public:
	SHSpeexCodec(INT band = SPEEX_WIDE, INT quality = 10);
	~SHSpeexCodec(void);

	void InitEncoder();
	int Encode(SHORT* pSrcData, UINT uCount, BYTE** pOutput);

	void InitDecoder();
	int Decode(BYTE* pSrcData, UINT uCount, SHORT* output);

	inline int GetFrameSize() const { return m_nFrameSize; }
};

