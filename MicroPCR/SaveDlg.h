#pragma once


// CSaveDlg ��ȭ �����Դϴ�.

class CSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CSaveDlg)

public:
	CSaveDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSaveDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_cLabel;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_cNoticeLabel;
};
