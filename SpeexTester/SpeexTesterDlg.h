
// SpeexTesterDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "PCMFile.h"

// CSpeexTesterDlg ��ȭ ����
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
	CStatic		m_STCInputPcm;
	CListCtrl	m_ListPCMInfo;
	CString		m_StrInputPcm;
	PCMFile*	m_pPCMFile;

public:
	virtual void ReceiveChunkData(BYTE* buffer, UINT count, BOOL isFinish);
	virtual void ReceiveHeaders(PPCM_HEADERS headers);

private:
	void ConvertUniCode(char* str, int len, TCHAR* OutputStr);

public:
	afx_msg void OnBnClickedBtnPcmOpen();
	
	
};