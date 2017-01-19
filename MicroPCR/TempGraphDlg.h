#pragma once

#include ".\GraphControl\GraphCtrl.h"
// CTempGraphDlg ��ȭ �����Դϴ�.

class CTempGraphDlg : public CDialog
{
	DECLARE_DYNAMIC(CTempGraphDlg)

private:
	GraphCtrl m_Graph;

public:
	CTempGraphDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTempGraphDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TEMP_GRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void addData(float data);
	virtual BOOL DestroyWindow();
};
