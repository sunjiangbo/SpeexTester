
// SpeexTesterDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "PCMFile.h"

// CSpeexTesterDlg 대화 상자
class CSpeexTesterDlg : public CDialogEx, IPCMRead
{
// 생성입니다.
public:
	CSpeexTesterDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SPEEXTESTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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