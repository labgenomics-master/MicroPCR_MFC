
// MicroPCR.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMicroPCRApp:
// �� Ŭ������ ������ ���ؼ��� MicroPCR.cpp�� �����Ͻʽÿ�.
//

class CMicroPCRApp : public CWinAppEx
{
public:
	CMicroPCRApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMicroPCRApp theApp;