#pragma once

// CConvertTool ��� ����Դϴ�.

class CConvertTool : public CObject
{
public:
	CConvertTool();
	virtual ~CConvertTool();
};

/***** �� Ŭ������ ���� �Լ��� ����ϱ� ���� Ŭ�����Դϴ�. *****/

void AfxPrintToInt(CString format, void* value);
void AfxPrintToDouble(CString format, void* value);
void AfxPrintToPBYTE(CString footer, BYTE *value, int length = 65);
CString AfxCharToString(char *ch);		// for use getting Serial number
double AfxQuickSort(double d[], int n);	// for filtering temperature