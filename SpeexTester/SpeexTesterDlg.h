
// SpeexTesterDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "PCMFile.h"
#include "SHSpeexCodec.h"

#define SPEEX_BUFFER_SIZE		2048

class CSpeexTesterDlg : public CDialogEx, IPCMRead
{
// �����Դϴ�.
public:
	CSpeexTesterDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SPEEXTESTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

private:
	CString			m_PCMInputPath;
	UINT			m_ProgPos;
	
	PCMFile*		m_pPCMFile;
	SHSpeexCodec*	m_pSpeexCodec;
	CFile*			m_pEncSaveFile;
	CFile*			m_pDecSaveFile;

	CButton			m_BtnOpen;
	CButton			m_BtnTest;
	CComboBox		m_CombFileType;
	CComboBox		m_CombSampleRate;
	CComboBox		m_CombQuality;
	CStatic			m_STCInputPcm;
	CProgressCtrl	m_Progress;
	CStatic			m_STCState;
	CStatic			m_StcEncBandWidth;

	UINT			m_nEncSum;
	UINT			m_nPCMDuration;
	UINT			m_nEncBandWidth;

public:
	virtual void ReceiveChunkData(BYTE* buffer, UINT count, BOOL isFinish);
	virtual void ReceiveHeaders(PPCM_HEADERS headers);
	static UINT DecodeFileWriteThread(LPVOID pParma);

private:
	void ConvertUniCode(char* str, int len, TCHAR* OutputStr);
	void CreateSpeexCodec();
	void InitControls();
	void InitProgress();

	//Test
	void foo(char* file);



public:
	afx_msg void OnBnClickedBtnPcmOpen();
	afx_msg void OnBnClickedBtnEncodeDecode();
	
};