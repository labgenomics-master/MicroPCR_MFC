// TempGraphDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "TempGraphDlg.h"


// CTempGraphDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTempGraphDlg, CDialog)

CTempGraphDlg::CTempGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTempGraphDlg::IDD, pParent)
{

}

CTempGraphDlg::~CTempGraphDlg()
{
}

void CTempGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTempGraphDlg, CDialog)
END_MESSAGE_MAP()


// CTempGraphDlg �޽��� ó�����Դϴ�.

BOOL CTempGraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CRect rect;
	GetClientRect(&rect);

	//  �׷��� �ʱ�ȭ
	m_Graph.Create(WS_VISIBLE | WS_CHILD, rect, this);
	m_Graph.InitGraph();
	m_Graph.BackgroundColor(RGB(0,0,0));
	m_Graph.GridColor(RGB(200,200,200));
	
	m_Graph.InitData(1, L"�µ�", 120, 0, RGB(255,0,0), true);
	m_Graph.m_SettingFlg = true;
	m_Graph.DrawRect();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CTempGraphDlg::addData(float data)
{
	m_Graph.LineAdd(1, data);
	m_Graph.DrawRect();
}
BOOL CTempGraphDlg::DestroyWindow()
{
	m_Graph.AllClear();
	m_Graph.DestroyWindow();

	return CDialog::DestroyWindow();
}
