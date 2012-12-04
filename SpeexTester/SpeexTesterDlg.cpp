CreateSpeexCodec
// SpeexTesterDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "SpeexTester.h"
#include "SpeexTesterDlg.h"
#include "afxdialogex.h"

#include <speex/speex.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CSpeexTesterDlg::CSpeexTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpeexTesterDlg::IDD, pParent)
	, m_pPCMFile (NULL)
	, m_pEncSaveFile (NULL)
	, m_pDecSaveFile (NULL)
	, m_pSpeexCodec ( NULL)
	, m_ProgPos (0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpeexTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LBL_INPUT, m_STCInputPcm);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_STC_STATE, m_STCState);
	DDX_Control(pDX, IDC_BTN_PCM_OPEN, m_BtnOpen);
	DDX_Control(pDX, IDC_BTN_ENCODE, m_BtnTest);
	DDX_Control(pDX, IDC_COMB_TYPE, m_CombFileType);
	DDX_Control(pDX, IDC_COMB_SAMPLERATE, m_CombSampleRate);
	DDX_Control(pDX, IDC_COMB_QUALITY, m_CombQuality);
	DDX_Control(pDX, IDC_STC_ENCBANDWIDTH, m_StcEncBandWidth);
}

BEGIN_MESSAGE_MAP(CSpeexTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PCM_OPEN, &CSpeexTesterDlg::OnBnClickedBtnPcmOpen)
	ON_BN_CLICKED(IDC_BTN_ENCODE, &CSpeexTesterDlg::OnBnClickedBtnEncodeDecode)
END_MESSAGE_MAP()


// CSpeexTesterDlg 메시지 처리기

BOOL CSpeexTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);


	InitControls();

	return TRUE;  
}

void CSpeexTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CSpeexTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CSpeexTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CSpeexTesterDlg::PreTranslateMessage(MSG* pMsg)
{
	
	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CSpeexTesterDlg::ConvertUniCode(char* str, int len, TCHAR* OutputStr)
{
	wchar_t strUnicode[256] = {0,};
	char    strMultibyte[256] = {0,};
	strcpy_s(strMultibyte,256, str);
	int nLen = MultiByteToWideChar(CP_ACP, 0, strMultibyte, len, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, strMultibyte, len, strUnicode, nLen);

	wsprintf(OutputStr, _T("%s\0"), strUnicode);
}

void CSpeexTesterDlg::CreateSpeexCodec()
{
	if (m_pSpeexCodec)
	{
		delete m_pSpeexCodec;
		m_pSpeexCodec = NULL;
	}

	INT bandSel = m_CombSampleRate.GetCurSel();
	if (bandSel == 0)
		bandSel = SPEEX_NARROW;
	else if (bandSel == 1)
		bandSel = SPEEX_WIDE;
	else if (bandSel == 2)
		bandSel = SPEEX_ULTRAWIDE;

	INT quality = m_CombQuality.GetCurSel();
	m_pSpeexCodec = new SHSpeexCodec(bandSel, quality + 1);
}

void CSpeexTesterDlg::InitControls()
{
	m_Progress.SetRange(0, 100);
	m_Progress.SetPos(0);
	m_STCState.SetWindowTextW(_T("대기중."));
	m_BtnTest.EnableWindow(FALSE);


	m_CombFileType.AddString(_T("WAVE"));
	m_CombFileType.AddString(_T("RAW"));
	m_CombFileType.SetCurSel(0);

	m_CombSampleRate.AddString(_T("8K (Narrow Band)"));
	m_CombSampleRate.AddString(_T("16K (Wide Band)"));
	m_CombSampleRate.AddString(_T("32K (Ultra Wide Band)"));
	m_CombSampleRate.SetCurSel(1);

	m_CombQuality.AddString(_T("1 (High)"));
		m_CombQuality.AddString(_T("2"));
	m_CombQuality.AddString(_T("3"));
	m_CombQuality.AddString(_T("4"));
	m_CombQuality.AddString(_T("5"));
	m_CombQuality.AddString(_T("6"));
	m_CombQuality.AddString(_T("7"));
	m_CombQuality.AddString(_T("8"));
	m_CombQuality.AddString(_T("9"));
	m_CombQuality.AddString(_T("10 (Low)"));
	m_CombQuality.SetCurSel(9);
}


void CSpeexTesterDlg::InitProgress()
{
	UINT nFilesize = m_pPCMFile->GetFileSize();
	m_nPCMDuration = nFilesize / 100 / 320;

	m_ProgPos = 0;
	int total = m_pPCMFile->GetFileSize() / m_pSpeexCodec->GetFrameSize();
	m_Progress.SetRange32(0, total);
	m_Progress.SetPos(0);
}



void CSpeexTesterDlg::OnBnClickedBtnPcmOpen()
{
	TCHAR szFilter[] = _T("ALL (*.*) | *.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

	if(IDOK == dlg.DoModal()) {

		m_PCMInputPath.SetString(dlg.GetPathName());
		m_STCInputPcm.SetWindowText(m_PCMInputPath);

		if (m_pPCMFile) 
		{
			delete(m_pPCMFile);
			m_pPCMFile = NULL;
		}

		INT band = m_CombSampleRate.GetCurSel();
		if (band == 0)
			band = 160;
		else if (band == 1)
			band = 320;
		else if (band == 2)
			band = 640;

		m_nPCMDuration = 0;
		m_pPCMFile = new PCMFile(m_PCMInputPath, band, FALSE);
		if (m_pPCMFile->OpenPCMFile() == FALSE) 
		{
			AfxMessageBox(_T("File Open Failed."));
		}
		else 
		{
			CreateSpeexCodec();
 			m_BtnTest.EnableWindow(TRUE);
		}
	} 	
}


void CSpeexTesterDlg::OnBnClickedBtnEncodeDecode()
{
	m_nEncSum = m_nEncBandWidth = 0;
	m_BtnOpen.EnableWindow(FALSE);
	m_BtnTest.EnableWindow(FALSE);

	if (m_pEncSaveFile) 
	{
		m_pEncSaveFile->Close();
		delete(m_pEncSaveFile);
		m_pEncSaveFile = NULL;
	}

	CString savePath = m_PCMInputPath;
	savePath = savePath.Left(savePath.GetLength() - 3);
	savePath.Append(_T("spxenc"));
	m_pEncSaveFile = new CFile(savePath, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary);

	m_pSpeexCodec->InitEncoder();

	
	InitProgress();
	m_pPCMFile->ReadData(this);
}


void CSpeexTesterDlg::ReceiveChunkData(BYTE* buffer, UINT count, BOOL isFinish)
{
	if (isFinish) 
	{
		m_nEncBandWidth = m_nEncSum / 100 / m_nPCMDuration;
		CString band;
		band.Format(_T("Enc BandWidth : %d bps"), m_nEncBandWidth);
		m_StcEncBandWidth.SetWindowTextW(band);

		m_pEncSaveFile->Close();
		delete m_pEncSaveFile;
		m_pEncSaveFile = NULL;

		m_STCState.SetWindowTextW(_T("Encoding Finish."));

		if (m_pDecSaveFile) 
		{
			m_pDecSaveFile->Close();
			delete(m_pDecSaveFile);
			m_pDecSaveFile = NULL;
		}
		::AfxBeginThread(DecodeFileWriteThread, (LPVOID)this);
		return;
	}

	LPBYTE output = NULL;
	int len = m_pSpeexCodec->Encode((short *)buffer, count, &output);
	m_nEncSum += len;

	m_pEncSaveFile->Write(&len , sizeof(int));
	m_pEncSaveFile->Write(output, len);

	m_Progress.SetPos(m_ProgPos++);

	TCHAR msg[256];
	wsprintf(msg, _T("Encoding : %d byte\0"), len);
	m_STCState.SetWindowTextW(msg);
}

void CSpeexTesterDlg::ReceiveHeaders(PPCM_HEADERS pHeaderData) 
{
}


UINT CSpeexTesterDlg::DecodeFileWriteThread(LPVOID pParma)
{
	
	CSpeexTesterDlg* pThis = (CSpeexTesterDlg *)pParma;

	CString encFilePath = pThis->m_PCMInputPath;
	encFilePath= encFilePath.Left(encFilePath.GetLength() - 3);
	encFilePath.Append(_T("spxenc"));

	CString savePath = pThis->m_PCMInputPath;
	savePath = savePath.Left(savePath.GetLength() - 3);
	savePath.Append(_T("spxdec"));

	pThis->m_pSpeexCodec->InitDecoder();
	SHSpeexCodec* codec = pThis->m_pSpeexCodec;

	SHORT* pOutputBuffer = new SHORT[codec->GetFrameSize()];
	PBYTE pReadBuffer = new BYTE[codec->GetFrameSize()];
	try
	{
		CFile encFile(encFilePath, CFile::modeRead);
		pThis->m_pDecSaveFile = new CFile(savePath, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary);

		int readByte = 0;
		BOOL isFinish = FALSE;
		TCHAR msg[256];
		while (TRUE) 
		{
			int encLen = 0;
			readByte = encFile.Read(&encLen, sizeof(int));
			readByte = encFile.Read(pReadBuffer , encLen);

			wsprintf(msg, _T("Decoding : %d : %d"), readByte, encLen);
			pThis->m_STCState.SetWindowText(msg);

			isFinish = (readByte < encLen);
			if (readByte == 0)
				break;

			int len = codec->Decode(pReadBuffer, encLen, pOutputBuffer );
			pThis->m_pDecSaveFile->Write(pOutputBuffer, sizeof(SHORT) * codec->GetFrameSize());

			pThis->m_Progress.SetPos(pThis->m_ProgPos++);

			if (isFinish) 
			{
				break;
			}
		}
		encFile.Close();
		pThis->m_pDecSaveFile->Close();
		delete pThis->m_pDecSaveFile;
		pThis->m_pDecSaveFile = NULL;

		delete [] pReadBuffer;
		delete [] pOutputBuffer;

		pThis->m_STCState.SetWindowText(_T("Decode Finish."));
		pThis->m_BtnOpen.EnableWindow(TRUE);

		delete pThis->m_pPCMFile;
		pThis->m_pPCMFile = NULL;

		delete pThis->m_pSpeexCodec;
		pThis->m_pSpeexCodec = NULL;
		
	}
	catch (CFileException* e)
	{
		TRACE(_T("DEC ERROR\n"));
	} 
	
	return 0x78;
}

/*
#define FRAME_SIZE 160
void CSpeexTesterDlg::foo(char* file) 
{
	const SpeexMode *pSpeex_uwb_mode = speex_lib_get_mode (SPEEX_MODEID_WB);
	const SpeexMode *pSpeex_wb_mode = speex_lib_get_mode (SPEEX_MODEID_WB);
	const SpeexMode *pSpeex_nb_mode = speex_lib_get_mode (SPEEX_MODEID_NB);

	char *inFile;
	FILE *fin, *fout;
	short in[FRAME_SIZE];
	float input[FRAME_SIZE];
	char cbits[200];
	int nbBytes;
	
	void *state;
	SpeexBits bits;
	int i, tmp;
	
	state = speex_encoder_init(pSpeex_wb_mode);
	tmp=10;
	speex_encoder_ctl(state, SPEEX_SET_QUALITY, &tmp);


	inFile = "./wb_male.wav";
	fin = fopen(inFile, "rb");
	fseek(fin, 0, SEEK_SET);
	fseek(fin, 44, SEEK_SET);
	fout = fopen("./out.spx", "wb");
		
	
	speex_bits_init(&bits);
	while (1)
	{
		
		int read = fread(in, sizeof(short), FRAME_SIZE, fin);
		if (feof(fin))
			break;
		
		for (i=0;i<FRAME_SIZE;i++)
			input[i]=in[i];
		
		speex_bits_reset(&bits);
		speex_encode(state, input, &bits);
		nbBytes = speex_bits_write(&bits, cbits, 200);
		printf("ENC : %d", nbBytes);
		fwrite(&nbBytes, sizeof(int), 1, fout);
		fwrite(cbits, 1, nbBytes, fout);
		
	}	
	speex_encoder_destroy(state);
	speex_bits_destroy(&bits);
	fclose(fin);
	fclose(fout);
	


	short out[FRAME_SIZE];
	float output[FRAME_SIZE];
	
	state = speex_decoder_init(pSpeex_wb_mode);
	tmp=1;
	speex_decoder_ctl(state, SPEEX_SET_ENH, &tmp);

	
	fout = fopen("./111.dex", "wb");
	fin = fopen("./wb_male.spxenc", "rb");
  
   speex_bits_init(&bits);
   while (1)
   {
      fread(&nbBytes, sizeof(int), 1, fin);
      fprintf (stderr, "nbBytes: %d\n", nbBytes);
      if (feof(fin))
         break;

	  TRACE(_T("______________READ :%d"), nbBytes);
      
  
      fread(cbits, 1, nbBytes, fin);
  
      speex_bits_read_from(&bits, cbits, nbBytes); 
      speex_decode(state, &bits, output);

     
      for (i=0;i<FRAME_SIZE;i++)
         out[i]=output[i];

   
      fwrite(out, sizeof(short), FRAME_SIZE, fout);
   }
  
   speex_decoder_destroy(state);
   speex_bits_destroy(&bits);
   fclose(fout);
   fclose(fin);
}
*/