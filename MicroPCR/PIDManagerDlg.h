#pragma once
#include "afxwin.h"
#include ".\gridctrl_src\gridctrl.h"
#include "UserDefs.h"


// CPIDManagerDlg ��ȭ �����Դϴ�.

class CPIDManagerDlg : public CDialog
{
private:
	vector< CString > pidList;
	vector< PID > pids;

	void initPidList();
	void initPidTable();
	void loadPidTable();

	int selectedIndex;

	bool hasPidList;

public:
	CString selectedPID;

	bool isHasPidList();

	DECLARE_DYNAMIC(CPIDManagerDlg)

public:
	CPIDManagerDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPIDManagerDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PID_MANAGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_cPidList;
	CGridCtrl m_cPidTable;
	afx_msg void OnBnClickedButtonPidNew();
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonDelete();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeListPid();
	virtual void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
};
