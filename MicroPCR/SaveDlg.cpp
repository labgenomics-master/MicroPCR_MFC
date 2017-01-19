// SaveDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "SaveDlg.h"


// CSaveDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSaveDlg, CDialog)

CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDlg::IDD, pParent)
	, m_cLabel(_T(""))
	, m_cNoticeLabel(_T(""))
{

}

CSaveDlg::~CSaveDlg()
{
}

void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SAVE_LABEL, m_cLabel);
	DDX_Text(pDX, IDC_STATIC_SAVE_LABEL, m_cNoticeLabel);
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSaveDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CSaveDlg �޽��� ó�����Դϴ�.

void CSaveDlg::OnBnClickedOk()
{
	UpdateData();

	if( m_cLabel.IsEmpty() ){
		AfxMessageBox(L"������ �̸��� �Է��ϼ���.");
		return;
	}

	OnOK();
}

BOOL CSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Ÿ��Ʋ�� label ���� ���� �������ش�.
	// Ÿ��Ʋ ������ InitDialog �� ����� ������ �����ϱ⿡ ���⿡ ó����.
	CString titleCheck;
	GetDlgItemText(IDC_STATIC_SAVE_LABEL, titleCheck);
	if( titleCheck.Find(L"PID") != -1 )
		SetWindowText(L"Save PID");
	else
		SetWindowText(L"Save Protocol");

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CSaveDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg -> message == WM_KEYDOWN )
	{
		if( pMsg -> wParam == VK_RETURN )
			return TRUE;

		if( pMsg -> wParam == VK_ESCAPE )
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
