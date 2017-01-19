// PIDCreateDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "PIDCreateDlg.h"
#include "FileManager.h"


// CPIDCreateDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPIDCreateDlg, CDialog)

CPIDCreateDlg::CPIDCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPIDCreateDlg::IDD, pParent)
	, m_cStartTemp(25)
	, m_cTargetTemp(25)
	, m_cKp(0)
	, m_cKi(0)
	, m_cKd(0)
	, saveLabel(L"")
{
	
}

CPIDCreateDlg::~CPIDCreateDlg()
{
}

void CPIDCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PID_START_TEMP, m_cStartTemp);
	DDV_MinMaxFloat(pDX, m_cStartTemp, 4, 104);
	DDX_Text(pDX, IDC_EDIT_PID_TARGET_TEMP2, m_cTargetTemp);
	DDV_MinMaxFloat(pDX, m_cTargetTemp, 4, 104);
	DDX_Text(pDX, IDC_EDIT_PID_P, m_cKp);
	//DDV_MinMaxFloat(pDX, m_cKp, 0, 200);
	DDX_Text(pDX, IDC_EDIT_PID_D, m_cKd);
	//DDV_MinMaxFloat(pDX, m_cKd, 0, 100);
	DDX_Text(pDX, IDC_EDIT_PID_I, m_cKi);
	//DDV_MinMaxFloat(pDX, m_cKi, 0, 1);
	DDX_Control(pDX, IDC_COMBO_PID_REMOVAL, m_cRemovalList);
	DDX_Control(pDX, IDC_CUSTOM_PID_TABLE3, m_cPidTable);
}


BEGIN_MESSAGE_MAP(CPIDCreateDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PID_CREATE_ADD, &CPIDCreateDlg::OnBnClickedButtonPidCreateAdd)
	ON_BN_CLICKED(IDC_BUTTON_PID_CREATE_COMPLETE, &CPIDCreateDlg::OnBnClickedButtonPidCreateComplete)
	ON_BN_CLICKED(IDC_BUTTON_PID_CREATE_REMOVAL, &CPIDCreateDlg::OnBnClickedButtonPidCreateRemoval)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_CUSTOM_PID_TABLE3, OnGridEndEdit)
END_MESSAGE_MAP()


BOOL CPIDCreateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initPidTable();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


// Enter, Esc ��ư ����
BOOL CPIDCreateDlg::PreTranslateMessage(MSG* pMsg)
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

void CPIDCreateDlg::OnBnClickedButtonPidCreateAdd()
{
	if( !UpdateData() )
		return;

	if( m_cStartTemp == m_cTargetTemp )
	{
		AfxMessageBox(L"���ۿµ��� Ÿ�ٿµ��� ���� ���� �� �� �����ϴ�.");
		return;
	}

	int pidSize = pids.size();
	
	for(int i=0; i<pidSize; ++i){
		PID pid = pids[i];
		
		if( pid.startTemp == m_cStartTemp &&
			pid.targetTemp == m_cTargetTemp ){
				AfxMessageBox(L"�̹� �����ϴ� pid parameter �Դϴ�.");
				return;
		}
	}

	// Row size �� �����Ѵ�
	m_cPidTable.SetRowCount(pidSize+2);

	// ���� ���� �����Ѵ�. 
	// ������ �� �ִ� �޺� �ڽ����� �־��ش�.
	pids.push_back( PID(m_cStartTemp, m_cTargetTemp, m_cKp, m_cKi, m_cKd) );

	CString label;
	label.Format(L"PID #%d", pidSize+1);
	m_cRemovalList.AddString(label);

	initPidTable();
	loadPidTable();

	m_cStartTemp = m_cTargetTemp = 25;
	m_cKp = m_cKd = m_cKi = 0;

	// �ʱ�ȭ�� ���� ���� control �� �Ѱ��ش�.
	UpdateData(false);
}

// Save �� �̸��� ����� Dialog
#include "SaveDlg.h"

void CPIDCreateDlg::OnBnClickedButtonPidCreateComplete()
{
	int size = pids.size();

	if( size == 0 ){
		int res = MessageBox(L"������ PID �� ���� ������� �ʽ��ϴ�.\n�����ϰڽ��ϱ�?", L"�˸�", MB_OKCANCEL);

		if( res == IDOK )
			OnCancel();
		else
			return;
	}

	static CSaveDlg dlg;
	dlg.m_cNoticeLabel = L"������ PID �̸��� �Է��ϼ���.";
	
	if( dlg.DoModal() == IDOK ){
		saveLabel = dlg.m_cLabel;
		dlg.m_cLabel = L"";

		CreateDirectory(L"./PID/", NULL);

		if( FileManager::findFile( L"./PID/", saveLabel ) ){
			AfxMessageBox(L"�ش� PID ���� �̹� �����մϴ�.\n�ٽ� �õ����ּ���.");
			return;
		}

		FileManager::savePID( saveLabel, pids );
		OnOK();
	}
}

void CPIDCreateDlg::OnBnClickedButtonPidCreateRemoval()
{
	int selectIndex = m_cRemovalList.GetCurSel();

	if( selectIndex == -1 ){
		AfxMessageBox(L"������ PID Parameter �� �����ϼ���.");
		return;
	}

	pids.erase( pids.begin() + selectIndex );
	
	m_cRemovalList.ResetContent();
	for(int i=0; i<pids.size(); ++i){
		CString item;
		item.Format(L"PID #%d", i+1);
		m_cRemovalList.AddString(item);
	}

	initPidTable();
	loadPidTable();
}

static const int PID_TABLE_COLUMN_WIDTHS[6] = { 50, 100, 100, 50, 50, 50 };

void CPIDCreateDlg::initPidTable()
{
	// grid control �� ���� ������ �����Ѵ�.
	m_cPidTable.SetListMode(true);

	m_cPidTable.DeleteAllItems();

	m_cPidTable.SetSingleRowSelection();
	m_cPidTable.SetSingleColSelection();
	m_cPidTable.SetRowCount(1);
	m_cPidTable.SetColumnCount(PID_CONSTANTS_MAX+1);
    m_cPidTable.SetFixedRowCount(1);
    m_cPidTable.SetFixedColumnCount(1);
	m_cPidTable.SetEditable(true);

	// �ʱ� gridControl �� table ������ �������ش�.
	DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE;

    for (int col = 0; col < m_cPidTable.GetColumnCount(); col++) { 
		GV_ITEM Item;
        Item.mask = GVIF_TEXT|GVIF_FORMAT;
        Item.row = 0;
        Item.col = col;

        if (col > 0) {
                Item.nFormat = DT_LEFT|DT_WORDBREAK;
                Item.strText = PID_TABLE_COLUMNS[col-1];
        }

        m_cPidTable.SetItem(&Item);
		m_cPidTable.SetColumnWidth(col, PID_TABLE_COLUMN_WIDTHS[col]);
    }
}

void CPIDCreateDlg::loadPidTable()
{
	m_cPidTable.SetRowCount(pids.size()+1);

	for(int i=0; i<pids.size(); ++i){
		float *temp[5] = { &(pids[i].startTemp), &(pids[i].targetTemp), 
			&(pids[i].kp), &(pids[i].kd), &(pids[i].ki)};
		for(int j=0; j<PID_CONSTANTS_MAX+1; ++j){
			GV_ITEM item;
			item.mask = GVIF_TEXT|GVIF_FORMAT;
			item.row = i+1;
			item.col = j;
			item.nFormat = DT_LEFT|DT_WORDBREAK;

			// ù��° column �� PID 1 ���� ǥ��
			if( j == 0 )
				item.strText.Format(L"PID #%d", i+1);
			else
				item.strText.Format(L"%.4f", *temp[j-1]);
			
			m_cPidTable.SetItem(&item);
		}
	}
}

void CPIDCreateDlg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	pids.clear();

	for (int row = 1; row < m_cPidTable.GetRowCount(); ++row) {
		CString startTemp = m_cPidTable.GetItemText(row, 1);
		CString targetTemp = m_cPidTable.GetItemText(row, 2);
		CString kp = m_cPidTable.GetItemText(row, 3);
		CString kd = m_cPidTable.GetItemText(row, 4);
		CString ki = m_cPidTable.GetItemText(row, 5);

		pids.push_back( PID( _wtof(startTemp), _wtof(targetTemp), _wtof(kp), _wtof(kd), _wtof(ki) ) );
	}

	*pResult = 0;
}