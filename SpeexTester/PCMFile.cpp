#include "StdAfx.h"
#include "PCMFile.h"

static PCMFile* pgInstance = NULL;

PCMFile::PCMFile(CString filepath)
	:m_pFile(NULL)
	,m_pRIFFHeader(NULL)
	,m_strFilePath (filepath)
	,m_bOpen (FALSE)
	,m_pThread (NULL)
{
	pgInstance = this;
}

PCMFile::~PCMFile(void)
{
	FileClose();
	TRACE(_T("PCMFILE Destroy\n"));
}

BOOL PCMFile::OpenPCMFile() 
{
	if (ValidExtension(m_strFilePath) == FALSE) 
		return FALSE;

	try
	{
		m_pFile = new CFile(m_strFilePath, CFile::modeRead);
		m_bOpen = TRUE;
	}
	catch (CFileException* e)
	{
		AfxMessageBox(_T("File Open failed\n"));
		return FALSE;
	}
	return TRUE;
}

void PCMFile::ReadHeader(IPCMRead* pRecvier) 
{
	if (m_pRIFFHeader)
	{
		pRecvier->ReceiveHeaders(m_pRIFFHeader);
		return;
	}
	
	if (ValidExtension(m_strFilePath) == FALSE) 
		return;

	if (m_pRIFFHeader == NULL)
	{
		m_pRIFFHeader = new PCM_HEADERS;
		m_pFile->Read(m_pRIFFHeader, sizeof(PCM_HEADERS));

		if (pRecvier) 
			pRecvier->ReceiveHeaders(m_pRIFFHeader);
	}
}

BOOL PCMFile::ReadData(IPCMRead* pRecvier)
{
	if (ValidExtension(m_strFilePath) == FALSE) 
	{
		pRecvier->ReceiveChunkData(NULL, 0, TRUE);
		return FALSE;
	}
	m_pThread= ::AfxBeginThread(PCMDataReadThread, (LPVOID)pRecvier);
	return TRUE;
}

BOOL PCMFile::ValidExtension(CString& path)
{
	CString ext = path.Right(3);
	ext = ext.MakeLower();

	if (ext.Compare(_T("wav")) == 0) 
		return TRUE;
	return FALSE;
}

void PCMFile::FileClose() 
{
	if (m_pFile) 
	{
		m_pFile->Close();
		delete(m_pFile);
		m_pFile = NULL;
	}

	if (m_pRIFFHeader) {
		delete(m_pRIFFHeader);
		m_pRIFFHeader = NULL;
	}
}



UINT PCMFile::PCMDataReadThread(LPVOID pParam)
{
	IPCMRead* pReceiver = (IPCMRead *)pParam;

	try
	{
		UINT readByte = 0;
		const UINT CHUNK_SIZE = 160;//m_pRIFFHeader->fmt.ByteRate;
		BYTE* readBuffer = new BYTE[CHUNK_SIZE];

		BOOL isFinish = FALSE;
		while (TRUE) 
		{
			readByte = pgInstance->m_pFile->Read(readBuffer, CHUNK_SIZE);

			isFinish = (readByte < CHUNK_SIZE);
			
			if (pReceiver) {
				pReceiver->ReceiveChunkData(readBuffer, readByte, isFinish);
			}
			
			if (isFinish)
			{
				TRACE(_T("Read End.\n"));
				break;
			}
		}
		free(readBuffer);
		pgInstance->FileClose();
	}
	catch (CFileException* e)
	{
		AfxMessageBox(_T("File Open failed\n"));
		pReceiver->ReceiveChunkData(NULL, 0, TRUE);
	}
	return 0x77;
}