
// SpeexTesterDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "SpeexTester.h"
#include "SpeexTesterDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpeexTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LBL_INPUT, m_STCInputPcm);
	DDX_Control(pDX, IDC_LIST_PCMINFO, m_ListPCMInfo);
}

BEGIN_MESSAGE_MAP(CSpeexTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PCM_OPEN, &CSpeexTesterDlg::OnBnClickedBtnPcmOpen)
END_MESSAGE_MAP()


// CSpeexTesterDlg �޽��� ó����

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

	
	//Init
	m_STCInputPcm.SetWindowTextW(_T("���õ� ���� ����."));
	m_ListPCMInfo.SetExtendedStyle(LVS_EX_GRIDLINES);
	m_ListPCMInfo.InsertColumn(0, _T("�ʵ�"), LVCFMT_LEFT, 100, -1);
	m_ListPCMInfo.InsertColumn(1, _T("��"), LVCFMT_CENTER, 200, -1);

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
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
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

void CSpeexTesterDlg::OnBnClickedBtnPcmOpen()
{
	TCHAR szFilter[] = _T("PCM (*.wav)|*.wav||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	
	if(IDOK == dlg.DoModal()) {

		m_StrInputPcm.SetString(dlg.GetPathName());
		m_STCInputPcm.SetWindowText(m_StrInputPcm);

		if (m_pPCMFile) 
		{
			delete(m_pPCMFile);
			m_pPCMFile = NULL;
		}
		m_pPCMFile = new PCMFile(m_StrInputPcm);
		if (m_pPCMFile->OpenPCMFile() == FALSE) 
		{
			AfxMessageBox(_T("PCM ���� ���� ����."));
		}
		else 
		{
			m_pPCMFile->ReadHeader(this);
			m_pPCMFile->ReadData(this);
		}
	} 	
}


static int ccc = 0;
void CSpeexTesterDlg::ReceiveChunkData(BYTE* buffer, UINT count, BOOL isFinish)
{
	TRACE(_T("RECEIVE DATA : %d : %d\n"), count, ccc++);
}

void CSpeexTesterDlg::ReceiveHeaders(PPCM_HEADERS pHeaderData) 
{
	m_ListPCMInfo.DeleteAllItems();


	TCHAR unicodeConvert[256] = {0,};
	TCHAR infoBuffer[256] = {0,};
	int row = 0;
	CString valueStr;

	
	ConvertUniCode((char *)pHeaderData->riff.chunkID, 4, unicodeConvert);
	m_ListPCMInfo.InsertItem(row, _T("Chunk ID"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, unicodeConvert, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->riff.chunckSize);
	m_ListPCMInfo.InsertItem(row, _T("Chunk Size"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );
	
	ConvertUniCode((char *)pHeaderData->riff.format, 4, unicodeConvert);
	m_ListPCMInfo.InsertItem(row, _T("Format"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, unicodeConvert, 0, 0, 0, NULL );

	ConvertUniCode((char *)pHeaderData->fmt.subChunk1ID, 4, unicodeConvert);
	m_ListPCMInfo.InsertItem(row, _T("SubChunk1 ID"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, unicodeConvert, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->fmt.subChunk1Size);
	m_ListPCMInfo.InsertItem(row, _T("SubChunk1 Size"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->fmt.audioFormat);
	m_ListPCMInfo.InsertItem(row, _T("Audio Format"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->fmt.numOfChannels);
	m_ListPCMInfo.InsertItem(row, _T("Channels"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->fmt.sampleRate);
	m_ListPCMInfo.InsertItem(row, _T("SmapleRate"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->fmt.ByteRate);
	m_ListPCMInfo.InsertItem(row, _T("ByteRate"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->fmt.BlockAlign);
	m_ListPCMInfo.InsertItem(row, _T("Block Align"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->fmt.BitsPerSmaple);
	m_ListPCMInfo.InsertItem(row, _T("Bit Per Sample"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );

	ConvertUniCode((char *)pHeaderData->data.subChunk2ID, 4, unicodeConvert);
	m_ListPCMInfo.InsertItem(row, _T("SubChunk2 ID"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, unicodeConvert, 0, 0, 0, NULL );

	valueStr.Format(_T("%d"), pHeaderData->data.subChunk2Size);
	m_ListPCMInfo.InsertItem(row, _T("SubChunk2 Size"));
	m_ListPCMInfo.SetItem(row++, 1, LVIF_TEXT, valueStr, 0, 0, 0, NULL );
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