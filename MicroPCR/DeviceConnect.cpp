// DeviceConnect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DeviceConnect.h"


// CDeviceConnect

CDeviceConnect::CDeviceConnect()
{
	// None
}

/******************************************************************
*	Function :	CDeviceConnect(HWND hwnd)
*
*	OverView :	�����ڷ�, �⺻ �����ڸ��� �� �����ڸ� ȣ���ؾߵȴ�.
*
*	Note	 :	��ġ�� ������� ��, �ش� ���̾�α׷� �޽����� ������� �ϱ� ������
*				�� ���̾�α��� ������ �ڵ鰪�� �Ű������� �޴´�.
******************************************************************/
CDeviceConnect::CDeviceConnect(HWND hwnd)
:	m_hDlg(hwnd)
{
}

CDeviceConnect::~CDeviceConnect()
{
}


// CDeviceConnect ��� �Լ�

/******************************************************************
*	Function :	int GetDevices(int Vid, int Pid, int Max)
*
*	OverView :	Ư�� Vid, Pid ���� ������ ��ġ���� ��� ����Ǿ� �ִ��� Ȯ���ϴ� �κ��̴�.
*				��ġ�� ������ ���� �� ������ �������ش�.
*
*	Note	 :	Vid, Pid ���� ����Ʈ�� MyPCR�� ������ �ִ�.
******************************************************************/
int CDeviceConnect::GetDevices(int Vid, int Pid, int Max)
{
	mdeviceList *pList = m_DeviceList;

	int nEntries = m_cDevices.GetList(Vid, Pid, NULL, NULL, NULL, pList, Max);

	// Reset the device_list structure pointer
	pList = m_DeviceList;

	return nEntries;
}

/******************************************************************
*	Function :	BOOL OpenDevice(int Vid, int Pid, char* Serial, BOOL IsBlock)
*
*	OverView :	Ư�� Vid, Pid ���� ������ ��ġ���� ����õ��� �Ѵ�.
*				���ῡ ���� ���ο� ���� BOOL ���� �Ѱ��ش�.
*
*	Note	 :	�ٸ� Ư�� �ø����� ������ ��⿡ �����ϱ� ���ؼ��� �ش� �ϴ� �ø���
*				�� �Ű������� �Ѱ�����Ѵ�. 
******************************************************************/
BOOL CDeviceConnect::OpenDevice(int Vid, int Pid, char* Serial, BOOL IsBlock)
{
	int status = m_cDevices.Open(Vid, Pid, NULL, Serial, NULL, IsBlock);

	if( status )
	{
		char *Serial = m_cDevices.m_SerialNumber;
		PostMessage(m_hDlg, WM_SET_SERIAL, 0x00, (LPARAM)Serial);
		return TRUE;
	}

	return FALSE;
}

/******************************************************************
*	Function :	BOOL CheckDevice(void)
*
*	OverView :	��ġ�� ������¸� Ȯ���ϴ� �ݹ��Լ����� �� �Լ��� ȣ���Ͽ�
*				����� ��ġ�� ������ �� ��ġ�� ������ �õ��ϴ� �ൿ�� ���ش�.
*				����� 1:1 �ҽ��̱⶧����, ���� ������ ���ؼ��� �ҽ��� �����ؾ��Ѵ�.
*				
*	Note	 :	None
******************************************************************/
BOOL CDeviceConnect::CheckDevice(void)
{
	if( GetDevices() )
	{
		return OpenDevice();
	}

	return FALSE;
}

/******************************************************************
*	Function :	int Read(BYTE *Buffer)
*
*	OverView :	OpenDevice �� ������ ��ġ�κ��� Buffer ���� �����͸� �Է¹޴´�.
*				���� ������ ũ�⸦ �������ش�.
*				
*	Note	 :	None
******************************************************************/
int CDeviceConnect::Read(void *Buffer)
{
	return m_cDevices.Read(Buffer);
}


/******************************************************************
*	Function :	int Write(BYTE *Buffer)
*
*	OverView :	OpenDevice �� ������ ��ġ���� Buffer�� �����Ѵ�.
*				���ۿ� ������ ������ ũ�⸦ �������ش�.
*				
*	Note	 :	None
******************************************************************/
int CDeviceConnect::Write(void *Buffer)
{
	return m_cDevices.Write(Buffer);
}
