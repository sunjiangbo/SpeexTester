
// SpeexTester.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CSpeexTesterApp:
// �� Ŭ������ ������ ���ؼ��� SpeexTester.cpp�� �����Ͻʽÿ�.
//

class CSpeexTesterApp : public CWinApp
{
public:
	CSpeexTesterApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CSpeexTesterApp theApp;