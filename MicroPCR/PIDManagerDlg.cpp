// PIDManagerDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "PIDManagerDlg.h"
#include "FileManager.h"


// CPIDManagerDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPIDManagerDlg, CDialog)

CPIDManagerDlg::CPIDManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPIDManagerDlg::IDD, pParent)
	, selectedPID(L"")
	, selectedIndex(-1)
	, hasPidList(false)
{

}

CPIDManagerDlg::~CPIDManagerDlg()
{
}

void CPIDManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PID, m_cPidList);
	DDX_Control(pDX, IDC_CUSTOM_PID_TABLE2, m_cPidTable);
}


BEGIN_MESSAGE_MAP(CPIDManagerDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PID_NEW, &CPIDManagerDlg::OnBnClickedButtonPidNew)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CPIDManagerDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CPIDManagerDlg::OnBnClickedButtonDelete)
	ON_LBN_SELCHANGE(IDC_LIST_PID, &CPIDManagerDlg::OnLbnSelchangeListPid)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_CUSTOM_PID_TABLE2, OnGridEndEdit)
END_MESSAGE_MAP()


// CPIDManagerDlg �޽��� ó�����Դϴ�.

// Enter, Esc ��ư ����
BOOL CPIDManagerDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CPIDManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initPidList();
	initPidTable();

	if( pidList.size() == 0 ){
		GetDlgItem(IDC_BUTTON_PID_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PID_SELECT)->EnableWindow(FALSE);
	}

	selectedIndex = -1;

	if( m_cPidList.GetCount() != 0 )
		hasPidList = true;
	else
		OnBnClickedButtonPidNew();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

#include "PIDCreateDlg.h"
// ���ο� PID �� ������ �� �ִ� Dialog �� �����Ѵ�.
void CPIDManagerDlg::OnBnClickedButtonPidNew()
{
	CPIDCreateDlg dlg;

	if( dlg.DoModal() == IDOK )
	{
		initPidList();
		AfxMessageBox(L"���ο� PID �� �����Ǿ����ϴ�.");
		
		GetDlgItem(IDC_BUTTON_PID_DELETE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PID_SELECT)->EnableWindow(TRUE);

		hasPidList = true;
	}
}

void CPIDManagerDlg::OnBnClickedButtonSelect()
{
	int idx = m_cPidList.GetCurSel();

	if( idx == -1 ){
		AfxMessageBox(L"����� PID �� �������ּ���.");
		return;
	}

	selectedPID = pidList[idx];

 	OnOK();
}

void CPIDManagerDlg::OnBnClickedButtonDelete()
{
	int idx = m_cPidList.GetCurSel();

	if( idx == -1 ){
		AfxMessageBox(L"������ PID �� �������ּ���.");
		return;
	}

	selectedPID = pidList[idx];

	// �ش� ������ �����ϰ�, ���� pid List �� �ҷ��´�.
	DeleteFile( L"./PID/" + selectedPID );
	initPidList();
	initPidTable();

	// ���� ��������� üũ
	if( m_cPidList.GetCount() != 0 )
		hasPidList = true;
	else
		hasPidList = false;

	selectedPID = L"";
}


void CPIDManagerDlg::initPidList()
{
	// ������ �ִ� pid list �� �����Ѵ�. 
	pidList.clear();
	m_cPidList.ResetContent();

	// PID ������ ���� �� �����Ƿ�, �������ش�.
	CreateDirectory(L"./PID/", NULL);
	FileManager::enumFiles( L"./PID/", pidList );

	for(int i=0; i<pidList.size(); ++i)
		m_cPidList.AddString( pidList[i] );

	if( pidList.size() == 0 ){
		GetDlgItem(IDC_BUTTON_PID_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PID_SELECT)->EnableWindow(FALSE);
	}
}

static const int PID_TABLE_COLUMN_WIDTHS[6] = { 41, 100, 100, 50, 50, 50 };

void CPIDManagerDlg::initPidTable()
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

void CPIDManagerDlg::loadPidTable()
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

void CPIDManagerDlg::OnLbnSelchangeListPid()
{
	selectedIndex = m_cPidList.GetCurSel();

	if( selectedIndex != -1 ){
		selectedPID = pidList[selectedIndex];

		// PID ������ �ƴ� ��� �����ϰ� ����Ʈ�� �ٽ� �������.
		if( !FileManager::loadPID( selectedPID, pids ) ){
			AfxMessageBox( selectedPID + L"�� �ùٸ� PID ������ �ƴմϴ�. ���ŵ˴ϴ�." );
			DeleteFile( L"./PID/" + selectedPID );
			initPidList();
			return;
		}

		// ����� pids ���� ���� pid table �� �����ش�.
		initPidTable();
		loadPidTable();
	}
}

void CPIDManagerDlg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	pids.clear();

	for (int row = 1; row < m_cPidTable.GetRowCount(); ++row) {
		CString startTemp = m_cPidTable.GetItemText(row, 1);
		CString targetTemp = m_cPidTable.GetItemText(row, 2);
		CString kp = m_cPidTable.GetItemText(row, 3);
		CString kd = m_cPidTable.GetItemText(row, 4);
		CString ki = m_cPidTable.GetItemText(row, 5);

		pids.push_back( PID( _wtof(startTemp), _wtof(targetTemp), _wtof(kp), _wtof(ki), _wtof(kd) ) );
	}

	if( !FileManager::savePID( selectedPID, pids ) ){
		AfxMessageBox(L"PID �� �����ϴ� �߿� ������ �߻��Ͽ����ϴ�.\n�����ڿ��� �����ϼ���.");
		return;
	}

	m_cPidList.SetCurSel(selectedIndex);
	OnLbnSelchangeListPid();

	*pResult = 0;
}

bool CPIDManagerDlg::isHasPidList()
{
	return hasPidList;
}