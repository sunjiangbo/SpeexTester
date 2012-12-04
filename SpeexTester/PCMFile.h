#pragma once
#include "PCMDefine.h"

class IPCMRead
{
public:
	virtual void ReceiveHeaders(PPCM_HEADERS pHeaderData) = 0;
	virtual void ReceiveChunkData(BYTE* buffer, UINT count, BOOL isFinish) = 0;
};

class PCMFile
{
public:
	CString			m_strFilePath;
	PPCM_HEADERS	m_pRIFFHeader;
	BOOL			m_bOpen;
	CFile*			m_pFile;
	CWinThread*		m_pThread;

	BOOL			m_bRawData;
	UINT			m_uReadSize;

public:
	PCMFile(CString filepath, UINT readSize = 160, BOOL isRawData = FALSE);
	~PCMFile(void);

	BOOL OpenPCMFile();
	void ReadHeader(IPCMRead* pRecvier);
	BOOL ReadData(IPCMRead* pRecvier);
	UINT GetFileSize();
	
	static UINT PCMDataReadThread(LPVOID pParam);

private:
	BOOL ValidExtension(CString& path);
	void FileClose();
};

