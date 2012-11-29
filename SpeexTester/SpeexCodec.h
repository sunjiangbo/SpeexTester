#pragma once

// speex codec type
#define SPEEX_NARROW							1
#define SPEEX_WIDE								2
#define SPEEX_ULTRAWIDE							3



struct SpeexBits;
struct SpeexPreprocessState;



class CSpeexCodec
{
public:
	CSpeexCodec(int band = SPEEX_WIDE, int mode = 10);
	~CSpeexCodec();

	BOOL InitEncoder();
	BOOL InitDecoder();

	int Encode(const LPBYTE pSrcData, const unsigned nSrcSize, LPBYTE pDstBuf);
	int Decode(const LPBYTE pSrcData, const unsigned nSrcSize, LPBYTE pDstBuf);

	inline int GetFrameSize() const { return m_nFrameSize; }

private:
	int m_band;
	int m_mode;
	int m_nFrameSize;
	
	void * encoder_state;
	void * decoder_state;

	SpeexBits * bits;
};