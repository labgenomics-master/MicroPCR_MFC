// ConvertTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ConvertTool.h"

#define __YJDEBUG__
//#undef  __YJDEBUG__		/* ���� �ۼ����� ���Ϸ� ����Ϸ��� �� ���� �ּ��ϼ���. */

// CConvertTool

/******************************************************************
*	Function :	CConvertTool()
*
*	OverView :	�����ڷ�, AfxPrintToPBYTE �Լ��� ����ϱ� ���ؼ��� �����ڸ� ȣ�����־���Ѵ�.
*
*	Note	 :	���� ���� ����� ������ ���� ��쿡 ������ �������ֱ� ���ؼ� ���.
******************************************************************/
CConvertTool::CConvertTool()
{
	CStdioFile filer;
	filer.Open(L"test.txt", CFile::modeCreate);
	filer.Close();
}

CConvertTool::~CConvertTool()
{
}


// CConvertTool ���� �Լ�

/******************************************************************
*	Function :	void AfxPrintToInt(CString format, void* value)
*
*	OverView :	Debug���� ���� ���â�� ����ϱ� ���ؼ� ����ϴ� �Լ�.
*				Int ���� ����ϱ� ���� �Լ��̴�.
*
*	Note	 :	Ex) AfxPrintToInt(L"test : %d", &value);
******************************************************************/
void AfxPrintToInt(CString format, void* value)
{
	CString string;
	string.Format(format, *((unsigned char *)value));
	string = string + L"\n";
	OutputDebugString(string);
}

/******************************************************************
*	Function :	void AfxPrintToDouble(CString format, void* value)
*
*	OverView :	Debug���� ���� ���â�� ����ϱ� ���ؼ� ����ϴ� �Լ�.
*				Double ���� ����ϱ� ���� �Լ��̴�.
*
*	Note	 :	Ex) AfxPrintToDouble(L"test : %f", &value);
******************************************************************/
void AfxPrintToDouble(CString format, void* value)
{
	CString string;
	string.Format(format, *((double *)value));
	string = string + L"\n";
	OutputDebugString(string);
}

/******************************************************************
*	Function :	void AfxPrintToPBYTE(CString footer, BYTE *value, int length)
*
*	OverView :	BYTE�� ���۰� ��� ��µǴ��� ���Ϸ� Ȯ���ϰ� ���� �� ����Ѵ�.
*				ù��° ���ڴ� ����Ϸ��� �ϴ� ���� �տ� ����� �������� ����Ѵ�.
*
*	Note	 :	Ex) AfxPrintToPBYTE(L"test : ", readdata, 65);
******************************************************************/
void AfxPrintToPBYTE(CString footer, BYTE *value, int length)
{
#ifdef __YJDEBUG__
	CStdioFile filer;
	
	CString string1, string2;
	string1 = footer;

	filer.Open(L"test.txt", CFile::modeWrite);
	filer.SeekToEnd();

	for(int i=1; i<length; i++)
	{
		string2.Format(L"%d/", value[i]);
		string1 = string1 + string2;
	}
	string1 = string1 + L"\n";
	filer.WriteString(string1);
#endif
}

/******************************************************************
*	Function :	void AfxCharToString(char *ch)
*
*	OverView :	c++ ������ ���ڿ��� MFC������ ���ڿ��� ������ �� ����Ѵ�.
*				char* -> CString (unicode ��: CStringA)
*
*	Note	 :	None
******************************************************************/
CString AfxCharToString(char *ch)
{
	CString string;
	int	len = MultiByteToWideChar(CP_ACP, 0, ch, strlen(ch), NULL, NULL);
	BSTR buf = SysAllocStringLen(NULL, len);
	MultiByteToWideChar(CP_ACP, 0, ch, strlen(ch), buf, len);
	string.Format(L"%s",buf);
	return string;
}

/******************************************************************
*	Function :	double AfxQuickSort(double d[], int n)
*
*	OverView :	ù��° ���ڷ� �迭�� �޾Ƽ� ���� ũ�⿡ ���� ������������
*				�������ش�. �׸��� ���� ū ���� �����Ѵ�.
*
*	Note	 :	None
******************************************************************/
double AfxQuickSort(double d[], int n)
{
	int left, right;
	double pivot;
	double temp;

	if( n > 1 )			// ��� ȣ�⿡ ���� ���ᰡ �ƴ� ����
	{
		pivot = d[n-1];	// ������ ���� �Ǻ����� ����
		left = -1;		// ���ʿ������� �˻��� ��ġ ����
		right = n-1;	// �����ʿ������� �˻��� ��ġ ����

		while(TRUE)
		{
			while( d[++left] < pivot );		// d[]�� ���� �Ǻ��� ��
			while( d[--right] > pivot );	// d[]�� ���� �Ǻ��� ��

			if( left >= right ) break;		// ������ ���� �������� ������ ũ�ų� ������ Ż��

			temp = d[left];					// ũ�� ���� ��� ���ʰ� ������ ���� ��ȯ
			d[left] = d[right];
		}

		temp = d[left];
		d[left] = d[n-1];
		d[n-1] = temp;
		AfxQuickSort(d, left);				// ���� �ұ����� ���� �� ����
		AfxQuickSort(d + left + 1, n - left - 1);	// ������ �ұ����� ���� �� ����
	}
	return d[2];
}
