
// MicroPCRDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "MicroPCRDlg.h"
#include "ConvertTool.h"
#include "FileManager.h"
#include "PIDManagerDlg.h"

#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMicroPCRDlg ��ȭ ����

CMicroPCRDlg::CMicroPCRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMicroPCRDlg::IDD, pParent)
	, openConstants(true)	// true �� �� ��, �Լ��� ȣ���Ͽ� �ݵ��� �Ѵ�. 
	, openGraphView(false)
	, isConnected(false)
	, m_cMaxActions(1000)
	, m_cTimeOut(1000)
	, m_cArrivalDelta(0.5)
	, m_cTemperature(0)
	, m_sProtocolName(L"")
	, m_totalActionNumber(0)
	, m_currentActionNumber(-1)
	, actions(NULL)
	, m_nLeftSec(0)
	, m_blinkCounter(0)
	, m_timerCounter(0)
	, recordFlag(false)
	, blinkFlag(false)
	, isRecording(false)
	, isStarted(false)
	, isCompletePCR(false)
	, isTargetArrival(false)
	, isFirstDraw(false)
	, m_startTime(0)
	, m_nLeftTotalSec(0)
	, m_prevTargetTemp(25)
	, m_currentTargetTemp(25)
	, m_timeOut(0)
	, m_leftGotoCount(-1)
	, m_recordingCount(0)
	, m_recStartTime(0)
	, m_cGraphYMin(0)
	, m_cGraphYMax(4096)
	, ledControl_wg(1)
	, ledControl_r(1)
	, ledControl_g(1)
	, ledControl_b(1)
	, currentCmd(CMD_READY)
	, m_kp(0.0)
	, m_ki(0.0)
	, m_kd(0.0)
	, isFanOn(false)
	, isLedOn(false)
	, m_cIntegralMax(INTGRALMAX)
	, loadedPID(L"")
	, m_cycleCount2(0)
	, isTempGraphOn(false)
	, targetTempFlag(false)
	, freeRunning(false)
	, freeRunningCounter(0)
	, m_cCompensation(0)
	, emergencyStop(false)
	, m_cCaptureTemper(0)
	, m_cGraphXVal(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMicroPCRDlg::~CMicroPCRDlg()
{
	// ������ ��ü�� �Ҹ��ڿ��� �������ش�.
	if( device != NULL )
		delete device;
	if( actions != NULL )
		delete []actions;
	if( m_Timer != NULL )
		delete m_Timer;
}

void CMicroPCRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_CHAMBER_TEMP, m_cProgressBar);
	DDX_Control(pDX, IDC_CUSTOM_PID_TABLE, m_cPidTable);
	DDX_Text(pDX, IDC_EDIT_MAX_ACTIONS, m_cMaxActions);
	DDV_MinMaxInt(pDX, m_cMaxActions, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_TIME_OUT, m_cTimeOut);
	DDV_MinMaxInt(pDX, m_cTimeOut, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_ARRIVAL_DELTA, m_cArrivalDelta);
	DDV_MinMaxFloat(pDX, m_cArrivalDelta, 0, 10.0);
	DDX_Text(pDX, IDC_EDIT_CHAMBER_TEMP, m_cTemperature);
	DDX_Control(pDX, IDC_LIST_PCR_PROTOCOL, m_cProtocolList);
	DDX_Text(pDX, IDC_EDIT_GRAPH_Y_MIN, m_cGraphYMin);
	DDV_MinMaxInt(pDX, m_cGraphYMin, 0, 4096);
	DDX_Text(pDX, IDC_EDIT_Y_MAX, m_cGraphYMax);
	DDV_MinMaxInt(pDX, m_cGraphYMax, 0, 4096);
	DDX_Text(pDX, IDC_EDIT_INTEGRAL_MAX, m_cIntegralMax);
	DDV_MinMaxFloat(pDX, m_cIntegralMax, 0.0, 10000.0);
	DDX_Text(pDX, IDC_EDIT_COMPENSATION, m_cCompensation);
	DDV_MinMaxByte(pDX, m_cCompensation, 0, 200);
	DDX_Text(pDX, IDC_EDIT_PD_CAPTURE, m_cCaptureTemper);
	DDV_MinMaxInt(pDX, m_cCaptureTemper, 25, 100);
	DDX_Text(pDX, IDC_EDIT_CYCLE_MAX, m_cGraphXVal);
	DDV_MinMaxInt(pDX, m_cGraphXVal, 1, 80);
}

BEGIN_MESSAGE_MAP(CMicroPCRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_SET_SERIAL, SetSerial)
	ON_MESSAGE(WM_MMTIMER, OnmmTimer)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CONSTANTS, &CMicroPCRDlg::OnBnClickedButtonConstants)
	ON_BN_CLICKED(IDC_BUTTON_CONSTANTS_APPLY, &CMicroPCRDlg::OnBnClickedButtonConstantsApply)
	ON_BN_CLICKED(IDC_BUTTON_PCR_START, &CMicroPCRDlg::OnBnClickedButtonPcrStart)
	ON_BN_CLICKED(IDC_BUTTON_PCR_OPEN, &CMicroPCRDlg::OnBnClickedButtonPcrOpen)
	ON_BN_CLICKED(IDC_BUTTON_PCR_RECORD, &CMicroPCRDlg::OnBnClickedButtonPcrRecord)
	ON_BN_CLICKED(IDC_BUTTON_GRAPHVIEW, &CMicroPCRDlg::OnBnClickedButtonGraphview)
	ON_BN_CLICKED(IDC_BUTTON_FAN_CONTROL, &CMicroPCRDlg::OnBnClickedButtonFanControl)
	ON_BN_CLICKED(IDC_BUTTON_ENTER_PID_MANAGER, &CMicroPCRDlg::OnBnClickedButtonEnterPidManager)
	ON_BN_CLICKED(IDC_BUTTON_LED_CONTROL, &CMicroPCRDlg::OnBnClickedButtonLedControl)
	ON_BN_CLICKED(IDC_CHECK_TEMP_GRAPH, &CMicroPCRDlg::OnBnClickedCheckTempGraph)
	ON_WM_MOVING()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMicroPCRDlg �޽��� ó����

BOOL CMicroPCRDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	/* UI Settings */
	bmpProgress.LoadBitmapW(IDB_BITMAP_SCALE);
	bmpRecNotWork.LoadBitmapW(IDB_BITMAP_REC_NOT_WORKING); 
	bmpRecWork.LoadBitmapW(IDB_BITMAP_REC_WORKING);

	((CButton*)GetDlgItem(IDC_BUTTON_PCR_RECORD))->SetBitmap((HBITMAP)bmpRecNotWork);

	m_cProgressBar.SetRange32(-10, 110);
	m_cProgressBar.SendMessage(PBM_SETBARCOLOR,0,RGB(200, 0, 50));

	SetDlgItemText(IDC_EDIT_ELAPSED_TIME, L"0m 0s");

	CFont font;
	CRect rect;
	CString labels[3] = { L"No", L"Temp.", L"Time" };

	font.CreatePointFont(100, L"Arial", NULL);

	m_cProtocolList.SetFont(&font);
	m_cProtocolList.GetClientRect(&rect);

	for(int i=0; i<3; ++i)
		m_cProtocolList.InsertColumn(i, labels[i], LVCFMT_CENTER, (rect.Width()/3));

	actions = new Action[m_cMaxActions];

	OnBnClickedButtonConstants();

	// 150725 PID Check
	// ������ �ҷ��� PID ���� �ִ��� Ȯ���Ѵ�.
	CString recentPath;
	vector< CString > pidList;
	FileManager::loadRecentPath(FileManager::PID_PATH, recentPath);
	FileManager::enumFiles( L"./PID/", pidList );

	do
	{
		if( recentPath.IsEmpty() ){
			if( pidList.empty() )
				AfxMessageBox(L"����� PID Parameter �� �������� �ʽ��ϴ�.\n�����ϴ� â���� �̵��˴ϴ�.");
			else
				AfxMessageBox(L"�ֱ� ����� PID Parameter �� �������� �ʽ��ϴ�.\n�����ϴ� â���� �̵��˴ϴ�.");
	
			OnBnClickedButtonEnterPidManager();
			break;
		}
		else{
			// recentPath �� �ҷ����鼭 ������ ���� ���, recentPath ���� 
			// �������ν� ���� if ���� �����ϵ��� �Ѵ�.
			if( !FileManager::loadPID( recentPath, pids ) ){
				DeleteFile(RECENT_PID_PATH);
				recentPath.Empty();
			}
			else{
				loadedPID = recentPath;
				SetDlgItemText(IDC_EDIT_LOADED_PID, loadedPID);
				break;
			}
		}
	}while(true);

	initPidTable();
	loadPidTable();

	loadConstants();

	// Chart Settings
	CAxis *axis;
	axis = m_Chart.AddAxis( kLocationBottom );
	axis->SetTitle(L"PCR Cycles");
	axis->SetRange(0, m_cGraphXVal);

	axis = m_Chart.AddAxis( kLocationLeft );
	axis->SetTitle(L"Sensor Value");
	axis->SetRange(m_cGraphYMin, m_cGraphYMax);

	sensorValues.push_back( 1.0 );

	device = new CDeviceConnect( GetSafeHwnd() );
	m_Timer = new CMMTimers(1, GetSafeHwnd());

	// ���� �õ�
	BOOL status = device->CheckDevice();

	if( status )
	{
		SetDlgItemText(IDC_EDIT_DEVICE_STATUS, L"Connected");
		isConnected = true;

		CString recentPath;
		FileManager::loadRecentPath(FileManager::PROTOCOL_PATH, recentPath);

		if( !recentPath.IsEmpty() )
			readProtocol(recentPath);
		else
			AfxMessageBox(L"No Recent Protocol File! Please Read Protocol!");

		m_Timer->startTimer(TIMER_DURATION, FALSE);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_DEVICE_STATUS, L"Disconnected");
		isConnected = false;
	}

	enableWindows();

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMicroPCRDlg::OnPaint()
{
	CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect graphRect;

		int oldMode = dc.SetMapMode( MM_LOMETRIC );

		graphRect.SetRect( 15, 350, 570, 760 );
		
		dc.DPtoLP( (LPPOINT)&graphRect, 2 );

		CDC *dc2 = CDC::FromHandle(dc.m_hDC);
		m_Chart.OnDraw( dc2, graphRect, graphRect );

		dc.SetMapMode( oldMode );

		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMicroPCRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMicroPCRDlg::SetSerial(WPARAM wParam, LPARAM lParam)
{
	char *Serial = (char *)lParam;
	return TRUE;
}

BOOL CMicroPCRDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	// ���� �õ�
	BOOL status = device->CheckDevice();

	if( status )
	{
		// �ߺ� connection ����
		if( isConnected ) 
			return TRUE;

		SetDlgItemText(IDC_EDIT_DEVICE_STATUS, L"Connected");

		isConnected = true;

		CString recentPath;
		FileManager::loadRecentPath(FileManager::PROTOCOL_PATH, recentPath);

		if( !recentPath.IsEmpty() )
			readProtocol(recentPath);
		else
			AfxMessageBox(L"No Recent Protocol File! Please Read Protocol!");

		m_Timer->startTimer(TIMER_DURATION, FALSE);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_DEVICE_STATUS, L"Disconnected");
		isConnected = false;

		m_Timer->stopTimer();
	}

	enableWindows();

	return TRUE;
}

BOOL CMicroPCRDlg::PreTranslateMessage(MSG* pMsg)
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

void CMicroPCRDlg::Serialize(CArchive& ar)
{
	// Constants ���� ������ �� �����.
	if (ar.IsStoring())
	{
		ar << m_cMaxActions << m_cTimeOut << m_cArrivalDelta << m_cGraphYMin << m_cGraphYMax << m_cIntegralMax << m_cCompensation << m_cCaptureTemper << m_cGraphXVal;
	}
	else	// Constants ���� ���Ϸκ��� �ҷ��� �� ����Ѵ�.
	{
		ar >> m_cMaxActions >> m_cTimeOut >> m_cArrivalDelta >> m_cGraphYMin >> m_cGraphYMax >> m_cIntegralMax >> m_cCompensation >> m_cCaptureTemper >> m_cGraphXVal;

		UpdateData(FALSE);
	}
}

static const int PID_TABLE_COLUMN_WIDTHS[6] = { 88, 120, 120, 75, 75, 75 };

// pid table �� grid control �� �׸��� ���� ����.
void CMicroPCRDlg::initPidTable()
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

// ���Ϸκ��� pid ���� �ҷ��ͼ� table �� �׷��ش�.
void CMicroPCRDlg::loadPidTable()
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

void CMicroPCRDlg::loadConstants()
{
	CFile file;

	if( file.Open(CONSTANTS_PATH, CFile::modeRead) ){
		CArchive ar(&file, CArchive::load);
		Serialize(ar);
		ar.Close();
		file.Close();
	}
	else{
		AfxMessageBox(L"Constants ������ ���� ���� �ʱ�ȭ�Ǿ����ϴ�.\n�ٽ� ���� �������ּ���.");
		saveConstants();
		OnBnClickedButtonConstants();
	}
}

void CMicroPCRDlg::saveConstants()
{
	CFile file;

	file.Open(CONSTANTS_PATH, CFile::modeCreate|CFile::modeWrite);
	CArchive ar(&file, CArchive::store);
	Serialize(ar);
	ar.Close();
	file.Close();
}

void CMicroPCRDlg::enableWindows()
{
	GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(isConnected&&!loadedPID.IsEmpty());
	GetDlgItem(IDC_BUTTON_PCR_OPEN)->EnableWindow(isConnected);
	GetDlgItem(IDC_BUTTON_PCR_RECORD)->EnableWindow(isConnected);
}

void CMicroPCRDlg::readProtocol(CString path)
{
	CFile file;
	if( path.IsEmpty() || !file.Open(path, CFile::modeRead) )
	{
		AfxMessageBox(L"No Recent Protocol File! Please Read Protocol!");
		return;
	}

	int fileSize = (int)file.GetLength() + 1;
	char *inTemp = new char[fileSize * sizeof(char)];
	file.Read(inTemp, fileSize);

	CString inString = AfxCharToString(inTemp);
	
	m_sProtocolName = getProtocolName(path);

	if( m_sProtocolName.GetLength() > MAX_PROTOCOL_LENGTH )
	{
		AfxMessageBox(L"Protocol Name is Too Long !");
		return;
	}

	if( inTemp ) delete[] inTemp;

	int markPos = inString.Find(L"%PCR%");
	if( markPos < 0 )
	{
		AfxMessageBox(L"This is not PCR File !!");
		return;
	}
	markPos = inString.Find(L"%END");
	if( markPos < 0 )
	{
		AfxMessageBox(L"This is not PCR File !!");
		return;
	}

	int line = 0;
	for(int i=0; i<markPos; i++)
	{
		if( inString.GetAt(i) == '\n' )
			line++;
	}

	m_totalActionNumber = 0;
	for(int i=0; i<line; i++)
	{
		int linePos = inString.FindOneOf(L"\n\r");
		CString oneLine = (CString)inString.Left(linePos);
		inString.Delete(0, linePos + 2);
		if( i > 0 )
		{
			int spPos = oneLine.FindOneOf(L" \t");
			// Label Extraction
			actions[m_totalActionNumber].Label = oneLine.Left(spPos);
			oneLine.Delete(0, spPos);
			oneLine.TrimLeft();
			// Temperature Extraction
			spPos = oneLine.FindOneOf(L" \t");
			CString tmpStr = oneLine.Left(spPos);
			oneLine.Delete(0, spPos);
			oneLine.TrimLeft();

			actions[m_totalActionNumber].Temp = (double)_wtof(tmpStr);

			bool timeflag = false;
			wchar_t tempChar = NULL;

			for(int j=0; j<oneLine.GetLength(); j++)
			{
				tempChar = oneLine.GetAt(j);
				if( tempChar == (wchar_t)'m' )
					timeflag = true;
				else if( tempChar == (wchar_t)'M' )
					timeflag = true;
				else
					timeflag = false;
			}

			// Duration Extraction
			double time = (double)_wtof(oneLine);

			if(timeflag)
				actions[m_totalActionNumber].Time = time * 60;
			else
				actions[m_totalActionNumber].Time = time;

			timeflag = false;
			
			if( actions[m_totalActionNumber].Label != "GOTO" )
			{
				m_nLeftTotalSec += (int)(actions[m_totalActionNumber].Time);
			}

			int label = 0;
			CString temp;
			if( actions[m_totalActionNumber].Label == "GOTO" )
			{
				while(true && actions[m_totalActionNumber].Temp != 0 && actions[m_totalActionNumber].Temp < 101 )
				{
					temp.Format(L"%.0f", actions[m_totalActionNumber].Temp);
					if( actions[label++].Label == temp) break;
				}

				for(int j=0; j<actions[m_totalActionNumber].Time; j++)
				{
					for(int k=label-1; k<m_totalActionNumber; k++)
						m_nLeftTotalSec += (int)(actions[k].Time);
				}
			}
			m_totalActionNumber++;
		}
	}

	int labelNo = 1;

	for(int i=0; i<m_totalActionNumber; i++)
	{
		if( actions[i].Label != "GOTO" )
		{
			if( _ttoi(actions[i].Label) != labelNo )
			{
				AfxMessageBox(L"Label numbering error");
				return;
			}
			else
				labelNo++;

			if( (actions[i].Temp > 100) || (actions[i].Temp < 0) )
			{
				AfxMessageBox(L"Target Temperature error!!");
				return;
			}

			if( (actions[i].Time > 3600) || (actions[i].Time < 0) )
			{
				AfxMessageBox(L"Target Duration error!!");
				return;
			}
		}
		else
		{
			if( (actions[i].Temp > (double)_wtof(actions[i-1].Label) ) ||
				(actions[i].Temp < 1) )
			{
				AfxMessageBox(L"GOTO Label error !!");
				return;
			}

			if( (actions[i].Time > 100) || (actions[i].Time < 1) )
			{
				AfxMessageBox(L"GOTO repeat count error !!");
				return;
			}
		}
	}

	m_nLeftTotalSecBackup = m_nLeftTotalSec;

	// ����Ʈ�� ǥ�����ش�.
	displayList();
}

void CMicroPCRDlg::displayList()
{
	int j=0;

	m_cProtocolList.DeleteAllItems();

	for(int i=0; i<m_totalActionNumber; i++)
	{
		m_cProtocolList.InsertItem(i, actions[i].Label);		// �� ���� ����Ʈ�� ǥ���Ѵ�.
		CString tempString;

		if( actions[i].Label == "GOTO" )		// GOTO �� ���
		{
			tempString.Format(L"%d", (int)actions[i].Temp);
			m_cProtocolList.SetItemText(i, 1, tempString);

			tempString.Format(L"%d",(int)actions[i].Time);
			m_cProtocolList.SetItemText(i, 2, tempString);
		}
		else					// ���� �� ���
		{
			tempString.Format(L"%d", (int)actions[i].Temp);
			m_cProtocolList.SetItemText(i, 1, tempString);			// �µ��� ����Ʈ�� ǥ���Ѵ�.

			// �ð� �ޱ�(�д���)
			int durs = (int)actions[i].Time;
			// �Ҽ��� ���� �ʷ� ȯ��
			int durm = durs/60;
			durs = durs%60;

			if( durs == 0 )
			{
				if( durm == 0 ) tempString.Format(L"��");
				else tempString.Format(L"%dm", durm);
			}
			else
			{
				if( durm == 0 ) tempString.Format(L"%ds", durs);
				else tempString.Format(L"%dm %ds", durm, durs);
			}

			m_cProtocolList.SetItemText(i, 2, tempString);		// �ð��� ����Ʈ�� ǥ���Ѵ�.
		}
	}

	// �������� �̸�
	// InvalidateRect(&CRect(10, 5, 10 * m_sProtocolName.GetLength() + MAX_PROTOCOL_LENGTH, 34));		// Protocol Name ����

	// ��ü �ð��� ��, ��, �� ������ ���
	int second = m_nLeftTotalSec % 60;
	int minute = m_nLeftTotalSec / 60;
	int hour   = minute / 60;
	minute = minute - hour * 60;

	// ��ü �ð� ǥ��
	CString leftTime;
	leftTime.Format(L"%02d:%02d:%02d", hour, minute, second);
	SetDlgItemText(IDC_EDIT_LEFT_PROTOCOL_TIME, leftTime);
}

CString CMicroPCRDlg::getProtocolName(CString path)
{
	int pos = 0;
	CString protocol = path;

	for(int i=0; i<protocol.GetLength(); i++)
	{
		if( protocol.GetAt(i) == '\\' )
			pos = i;
	}

	return protocol.Mid(pos + 1, protocol.GetLength());
}

void CMicroPCRDlg::OnBnClickedButtonConstants()
{
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	if( openConstants )
	{
		SetWindowPos(NULL, w/3, h/3, 585, 375, SWP_NOZORDER);
		SetDlgItemText(IDC_BUTTON_CONSTANTS, L"PID ����");
	}
	else
	{
		if( openGraphView )
			OnBnClickedButtonGraphview();
		
		// openConstants
		if( isTempGraphOn )
			SetWindowPos(NULL, 100, h/3, 1175, 375, SWP_NOZORDER);
		else
			SetWindowPos(NULL, w/5, h/3, 1175, 375, SWP_NOZORDER);
		SetDlgItemText(IDC_BUTTON_CONSTANTS, L"PID �ݱ�");
	}

	m_cPidTable.SetEditable(!openConstants);
	openConstants = !openConstants;
}

void CMicroPCRDlg::OnBnClickedButtonGraphview()
{
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	if( openGraphView )
	{
		SetWindowPos(NULL, w/3, h/3, 585, 375, SWP_NOZORDER);
		SetDlgItemText(IDC_BUTTON_GRAPHVIEW, L"�׷��� ����");
	}
	else
	{
		if( openConstants )
			OnBnClickedButtonConstants();
		SetWindowPos(NULL, w/3, h/7, 585, 800, SWP_NOZORDER);
		SetDlgItemText(IDC_BUTTON_GRAPHVIEW, L"�׷��� �ݱ�");
	}

	openGraphView = !openGraphView;
}

void CMicroPCRDlg::OnBnClickedButtonConstantsApply()
{
	if( !UpdateData() )
		return;

	if( !pids.empty() ){
		pids.clear();
	
		for (int row = 1; row < m_cPidTable.GetRowCount(); row++) {
			CString startTemp = m_cPidTable.GetItemText(row, 1);
			CString targetTemp = m_cPidTable.GetItemText(row, 2);
			CString kp = m_cPidTable.GetItemText(row, 3);
			CString kd = m_cPidTable.GetItemText(row, 4);
			CString ki = m_cPidTable.GetItemText(row, 5);
	
			pids.push_back( PID( _wtof(startTemp), _wtof(targetTemp), _wtof(kp), _wtof(ki), _wtof(kd) ) );
		}

		// ����� ���� ���� �������ش�.
		FileManager::savePID( loadedPID, pids );
	}

	// PID ���� ������ ������ Constants ���� �����Ѵ�.
	saveConstants();

	CAxis *axis = m_Chart.AddAxis( kLocationBottom );
	axis->SetRange(0, m_cGraphXVal);
	axis = m_Chart.GetAxisByLocation( kLocationLeft );

	axis->SetRange(m_cGraphYMin, m_cGraphYMax);

	// ���� ���� ���, ���� ����� ������ pid �� �����Ѵ�.
	if( isStarted )
		findPID();
}

void CMicroPCRDlg::OnBnClickedButtonPcrStart()
{
	if( m_totalActionNumber < 1 )
	{
		AfxMessageBox(L"There is no action list. Please load task file!!");
		return;
	}

	if( !isStarted )
	{
		CString message = L"";

		for (int row = 1; row < m_cPidTable.GetRowCount(); row++) {
			CString startTemp = m_cPidTable.GetItemText(row, 1);
			CString targetTemp = m_cPidTable.GetItemText(row, 2);
			CString kp = m_cPidTable.GetItemText(row, 3);
			CString kd = m_cPidTable.GetItemText(row, 4);
			CString ki = m_cPidTable.GetItemText(row, 5);

			message += L"temp(" + startTemp + L"->" + targetTemp + L"), pid = " + kp + L"," + ki + L"," + kd + L"\n";
		}

		message.Format(L"%s\nCompensation : %d ���� PCR�� �����ұ��?", message, m_cCompensation);
		
		if( MessageBox(message, L"PCR Parameter Check", MB_YESNO ) == IDYES ){

			initValues();

			isStarted = true;

			GetDlgItem(IDC_BUTTON_FAN_CONTROL)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_PCR_OPEN)->EnableWindow(FALSE);
			SetDlgItemText(IDC_BUTTON_PCR_START, L"PCR Stop");

			currentCmd = CMD_PCR_RUN;

			m_prevTargetTemp = 25;
			m_currentTargetTemp = (BYTE)actions[0].Temp;
			findPID();

			if( !isRecording )
				OnBnClickedButtonPcrRecord();

			m_startTime = timeGetTime();

			isFirstDraw = false;
			clearSensorValue();

			// Log ������ ����ϱ� ���� ���� ����
			CreateDirectory(L"./Log/", NULL);
		}
	}
	else
	{
		PCREndTask();
	}
}

void CMicroPCRDlg::OnBnClickedButtonPcrOpen()
{
	if( !isConnected )
		return;

	m_nLeftTotalSec = 0;

	CFileDialog fdlg(TRUE, NULL, NULL, NULL, L"*.txt |*.txt|");
	if( fdlg.DoModal() == IDOK )
	{
		FileManager::saveRecentPath(FileManager::PROTOCOL_PATH, fdlg.GetPathName());

		m_sProtocolName = getProtocolName(fdlg.GetPathName());

		readProtocol(fdlg.GetPathName());
	}
}

void CMicroPCRDlg::OnBnClickedButtonPcrRecord()
{
	if( !isRecording )
	{
		CreateDirectory(L"./Record/", NULL);

		CString fileName, fileName2, fileName3;
		CTime time = CTime::GetCurrentTime();
		fileName = time.Format(L"./Record/%Y%m%d-%H%M-%S.txt");
		fileName2 = time.Format(L"./Record/pd%Y%m%d-%H%M-%S.txt");
		fileName3 = time.Format(L"./Record/pdRaw%Y%m%d-%H%M-%S.txt");
		
		m_recFile.Open(fileName, CStdioFile::modeCreate|CStdioFile::modeWrite);
		m_recFile.WriteString(L"Number	Time	Temperature\n");

		m_recPDFile.Open(fileName2, CStdioFile::modeCreate|CStdioFile::modeWrite);
		m_recPDFile.WriteString(L"Cycle	Time	Value\n");

		m_recPDFile2.Open(fileName3, CStdioFile::modeCreate|CStdioFile::modeWrite);
		m_recPDFile2.WriteString(L"Cycle	Time	Value\n");

		m_recordingCount = 0;
		m_cycleCount = 0;
		m_cycleCount2 = 0;
		m_recStartTime = timeGetTime();
	}
	else
	{
		m_recFile.Close();
		m_recPDFile.Close();
		m_recPDFile2.Close();
	}

	isRecording = !isRecording;
}

void CMicroPCRDlg::OnBnClickedButtonFanControl()
{
	isFanOn = !isFanOn;

	if( isFanOn )
	{
		currentCmd = CMD_FAN_ON;
		SetDlgItemText(IDC_BUTTON_FAN_CONTROL, L"FAN OFF");
		GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(FALSE);
	}
	else
	{
		currentCmd = CMD_FAN_OFF;
		SetDlgItemText(IDC_BUTTON_FAN_CONTROL, L"FAN ON");
		GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(TRUE);
	}
}

// PID �� ������ �� �ִ� Manager Dialog �� �����Ѵ�.
void CMicroPCRDlg::OnBnClickedButtonEnterPidManager()
{
	static CPIDManagerDlg dlg;

	if( dlg.DoModal() == IDOK )
	{
		loadedPID = dlg.selectedPID;
		SetDlgItemText(IDC_EDIT_LOADED_PID, loadedPID);
		FileManager::saveRecentPath(FileManager::PID_PATH, loadedPID);
		if( !FileManager::loadPID(loadedPID, pids) ){
			AfxMessageBox(L"PID �� �����ϴµ� ������ �߻��Ͽ����ϴ�.\n�����ڿ��� �����ϼ���.");
		}
	}

	if( loadedPID.IsEmpty() )
		AfxMessageBox(L"PID �� ���õ��� ������ PCR �� ������ �� �����ϴ�.");

	if( !dlg.isHasPidList() ){
		loadedPID.Empty();
		SetDlgItemText(IDC_EDIT_LOADED_PID, loadedPID);
		pids.clear();
	}

	// ���������� ��������� Start ��ư ��Ȱ��ȭ
	GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(!loadedPID.IsEmpty());

	initPidTable();
	loadPidTable();
}

void CMicroPCRDlg::OnBnClickedButtonLedControl()
{
	isLedOn = !isLedOn;

	if( isLedOn )
	{
		ledControl_b = 0;
		SetDlgItemText(IDC_BUTTON_LED_CONTROL, L"LED OFF");
	}
	else
	{
		ledControl_b = 1;
		SetDlgItemText(IDC_BUTTON_LED_CONTROL, L"LED ON");
	}
}



void CMicroPCRDlg::blinkTask()
{
	m_blinkCounter++;
	if( m_blinkCounter > (500/TIMER_DURATION) )
	{
		m_blinkCounter = 0;
		if( isRecording )
		{
			recordFlag = !recordFlag;
			if( recordFlag )
				SET_BUTTON_IMAGE(IDC_BUTTON_PCR_RECORD, bmpRecWork);
			else
				SET_BUTTON_IMAGE(IDC_BUTTON_PCR_RECORD, bmpRecNotWork);
		}
		else
			SET_BUTTON_IMAGE(IDC_BUTTON_PCR_RECORD, bmpRecNotWork);

		if( isStarted )
		{
			blinkFlag = !blinkFlag;
			if( blinkFlag )
				m_cProtocolList.SetTextBkColor(RGB(240,200,250));
			else
				m_cProtocolList.SetTextBkColor(RGB(255,255,255));

			m_cProtocolList.RedrawItems(m_currentActionNumber, m_currentActionNumber);
		}
		else
		{
			m_cProtocolList.SetTextBkColor(RGB(255,255,255));
			m_cProtocolList.RedrawItems(0, m_totalActionNumber);
		}
	}
}

CString dslr_title;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char title[128];
	GetWindowTextA(hwnd,title,sizeof(title));

	if( strstr(title, "DSLR Remote Pro for Windows - Connected") != NULL )
		dslr_title = title;

	return TRUE;
}

void CMicroPCRDlg::findPID()
{
	if ( fabs(m_prevTargetTemp - m_currentTargetTemp) < .5 ) 
		return; // if target temp is not change then do nothing 1->.5 correct

	// enable the constant changing.
	GetDlgItem(IDC_BUTTON_CONSTANTS_APPLY)->EnableWindow(FALSE);
	
	double dist	 = 10000;
	int paramIdx = 0;
	
	for ( int i = 0; i < pids.size(); i++ )
	{
		double tmp = fabs(m_prevTargetTemp - pids[i].startTemp) + fabs(m_currentTargetTemp - pids[i].targetTemp);

		if ( tmp < dist )
		{
			dist = tmp;
			paramIdx = i;
		}
	}

	m_kp = pids[paramIdx].kp;
	m_ki = pids[paramIdx].ki;
	m_kd = pids[paramIdx].kd;

	GetDlgItem(IDC_BUTTON_CONSTANTS_APPLY)->EnableWindow(TRUE);
}

void CMicroPCRDlg::timeTask()
{
	// elapse time 
	int elapsed_time = (int)((double)(timeGetTime()-m_startTime)/1000.);
	int min = elapsed_time/60;
	int sec = elapsed_time%60;
	CString temp;
	temp.Format(L"%dm %ds", min, sec);
	SetDlgItemText(IDC_EDIT_ELAPSED_TIME, temp);

	// timer counter increased
	m_timerCounter++;

	// 1s ���� ����ǵ��� ����
	if( m_timerCounter == (1000/TIMER_DURATION) )
	{
		m_timerCounter = 0;

		if( m_nLeftSec == 0 )
		{
			m_currentActionNumber++;

			// clear previous blink
			m_cProtocolList.SetTextBkColor(RGB(255,255,255));
			m_cProtocolList.RedrawItems(0, m_totalActionNumber);

			if( (m_currentActionNumber) >= m_totalActionNumber )
			{
				isCompletePCR = true;
				PCREndTask();
				return;
			}

			if( actions[m_currentActionNumber].Label.Compare(L"GOTO") != 0 )
			{
				m_prevTargetTemp = m_currentTargetTemp;
				m_currentTargetTemp = (int)actions[m_currentActionNumber].Temp;

				// 150828 YJ added for target temperature comparison by case
				targetTempFlag = m_prevTargetTemp > m_currentTargetTemp;

				isTargetArrival = false;
				m_nLeftSec = (int)(actions[m_currentActionNumber].Time);
				m_timeOut = m_cTimeOut*10;

				int min = m_nLeftSec/60;
				int sec = m_nLeftSec%60;

				// current left protocol time
				CString leftTime;
				if( min == 0 )
					leftTime.Format(L"%ds", sec);
				else
					leftTime.Format(L"%dm %ds", min, sec);
				m_cProtocolList.SetItemText(m_currentActionNumber, 2, leftTime);

				// find the proper pid values.
				findPID();
			}
			else	// is GOTO
			{
				if( m_leftGotoCount < 0 )
					m_leftGotoCount = (int)actions[m_currentActionNumber].Time;

				if( m_leftGotoCount == 0 )
					m_leftGotoCount = -1;
				else
				{
					m_leftGotoCount--;
					CString leftGoto;
					leftGoto.Format(L"%d", m_leftGotoCount);

					m_cProtocolList.SetItemText(m_currentActionNumber, 2, leftGoto);
					
					// GOTO ���� target label ���� �־���
					CString tmpStr;
					tmpStr.Format(L"%d",(int) actions[m_currentActionNumber].Temp);

					int pos = 0;
					for (pos = 0; pos < m_totalActionNumber; ++pos)
						if( tmpStr.Compare(actions[pos].Label) == 0 )
							break;

					m_currentActionNumber = pos-1;
				}
			}
		}
		else	// action �� �������� ���
		{
			if( !isTargetArrival )
			{
				m_timeOut--;

				if( m_timeOut == 0 )
				{
					AfxMessageBox(L"The target temperature cannot be reached!!");
					PCREndTask();
				}
			}
			else
			{
				m_nLeftSec--;
				m_nLeftTotalSec--;

				int min = m_nLeftSec/60;
				int sec = m_nLeftSec%60;

				// current left protocol time
				CString leftTime;
				if( min == 0 )
					leftTime.Format(L"%ds", sec);
				else
					leftTime.Format(L"%dm %ds", min, sec);
				m_cProtocolList.SetItemText(m_currentActionNumber, 2, leftTime);
				
				// total left protocol time
				min = m_nLeftTotalSec/60;
				sec = m_nLeftTotalSec%60;

				if( min == 0 )
					leftTime.Format(L"%ds", sec);
				else
					leftTime.Format(L"%dm %ds", min, sec);
				SetDlgItemText(IDC_EDIT_LEFT_PROTOCOL_TIME, leftTime);
			}
		}

		// 150108 YJ for camera shoot
		if( m_currentActionNumber != 0 && 
			( m_nLeftSec == 1 && ((int)(actions[m_currentActionNumber].Temp) == m_cCaptureTemper) ))
		{
			dslr_title = "";
			EnumWindows(EnumWindowsProc, NULL);

			if( dslr_title != "" )
			{
				HWND checker = ::FindWindow(NULL, dslr_title);

				if( checker )
					::PostMessage(checker, WM_COMMAND, MAKELONG(1003, BN_CLICKED), (LPARAM)GetSafeHwnd());
			}
		}

		if( m_currentActionNumber != 0 && 
			m_nLeftSec == 2 && ((int)(actions[m_currentActionNumber].Temp) == m_cCaptureTemper) )
		{
			ledControl_b = 0;
		}
		else if( m_currentActionNumber != 0 && 
			m_nLeftSec == 0 && ((int)(actions[m_currentActionNumber].Temp) == m_cCaptureTemper) )
		{
			ledControl_b = 1;
		}

		// for graph drawing
		if( ((int)(actions[m_currentActionNumber].Temp) == m_cCaptureTemper) && 
			m_nLeftSec == 1 )
		{
			double lights = (double)(photodiode_h & 0x0f)*255. + (double)(photodiode_l);
			addSensorValue( lights );

			if( isRecording )
			{
				m_cycleCount++;
				CString out;
				out.Format(L"%6d	%8.0f	%3.1f\n", m_cycleCount, (double)(timeGetTime()-m_recStartTime), lights);
				m_recPDFile.WriteString(out);
			}
		}
	}
}

void CMicroPCRDlg::PCREndTask()
{
	if( isRecording )
		OnBnClickedButtonPcrRecord();

	initValues();

	// 151020 YJ
	m_Timer->stopTimer();
	GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(FALSE);
	
	while( true ){
		RxBuffer rx;
		TxBuffer tx;

		memset(&rx, 0, sizeof(RxBuffer));
		memset(&tx, 0, sizeof(TxBuffer));

		tx.cmd = CMD_PCR_STOP;

		BYTE senddata[65] = { 0, };
		BYTE readdata[65] = { 0, };
		memcpy(senddata, &tx, sizeof(TxBuffer));

		device->Write(senddata);

		device->Read(&rx);

		memcpy(readdata, &rx, sizeof(RxBuffer));

		if( rx.state == STATE_READY ){
			break;
		}

		Sleep(TIMER_DURATION);
	}

	m_Timer->startTimer(TIMER_DURATION, FALSE);
	GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(TRUE);
	isStarted = false;

	SetDlgItemText(IDC_BUTTON_PCR_START, L"PCR Start");

	if( !emergencyStop ){
		if( isCompletePCR )
			AfxMessageBox(L"PCR ended!!");
		else
			AfxMessageBox(L"PCR incomplete!!");
	}
	else
		AfxMessageBox(L"Emergency stop!(overheating)");

	GetDlgItem(IDC_BUTTON_PCR_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_FAN_CONTROL)->EnableWindow(TRUE);

	emergencyStop = false;
	isCompletePCR = false;
}

void CMicroPCRDlg::initValues()
{
	currentCmd = CMD_READY;
	m_currentActionNumber = -1;
	m_nLeftSec = 0;
	m_nLeftTotalSec = 0;
	m_timerCounter = 0;
	m_startTime = 0;
	recordFlag = false;

	m_nLeftTotalSec = m_nLeftTotalSecBackup;
	m_leftGotoCount = -1;
	m_recordingCount = 0;
	m_recStartTime = 0;
	blinkFlag = false;
	m_timeOut = 0;
	m_blinkCounter = 0;
	ledControl_wg = 1;
	ledControl_r = 1;
	ledControl_g = 1;
	ledControl_b = 1;

	m_kp = 0;
	m_ki = 0;
	m_kd = 0;

	isTargetArrival = false;

	m_prevTargetTemp = m_currentTargetTemp = 25;
}

LRESULT CMicroPCRDlg::OnmmTimer(WPARAM wParam, LPARAM lParam)
{
	blinkTask();

	if( isStarted )
	{
		int errorPrevent = 0;
		timeTask();
	}

	RxBuffer rx;
	TxBuffer tx;
	float currentTemp = 0.0;

	memset(&rx, 0, sizeof(RxBuffer));
	memset(&tx, 0, sizeof(TxBuffer));

	tx.cmd = currentCmd;
	tx.currentTargetTemp = (BYTE)m_currentTargetTemp;
	tx.ledControl = 1;
	tx.led_wg = ledControl_wg;
	tx.led_r = ledControl_r;
	tx.led_g = ledControl_g;
	tx.led_b = ledControl_b;
	tx.compensation = m_cCompensation;

	// pid ���� buffer �� �����Ѵ�.
	
	memcpy(&(tx.pid_p1), &(m_kp), sizeof(float));
	memcpy(&(tx.pid_i1), &(m_ki), sizeof(float));
	memcpy(&(tx.pid_d1), &(m_kd), sizeof(float));

	// integral max ���� buffer �� �����Ѵ�.
	memcpy(&(tx.integralMax_1), &(m_cIntegralMax), sizeof(float));

	BYTE senddata[65] = { 0, };
	BYTE readdata[65] = { 0, };
	memcpy(senddata, &tx, sizeof(TxBuffer));

	device->Write(senddata);

	if( device->Read(&rx) == 0 )
		return FALSE;

	memcpy(readdata, &rx, sizeof(RxBuffer));

	// ���κ��� ���� �µ� ���� �޾ƿͼ� ������.
	// convert BYTE pointer to float type for reading temperature value.
	memcpy(&currentTemp, &(rx.chamber_temp_1), sizeof(float));

	// ���κ��� ���� Photodiode ���� �޾ƿͼ� ������.
	photodiode_h = rx.photodiode_h;
	photodiode_l = rx.photodiode_l;

	if( currentCmd == CMD_FAN_OFF ){
		currentCmd = CMD_READY;
	}

	if( currentTemp < 0.0 )
		return FALSE;

	// 150828 YJ added
	// 150904 YJ changed, for waiting device flag
	// 150918 YJ deleted, bug by usb communication
	/*
	if( targetTempFlag && rx.targetArrival )
		targetTempFlag = false;
	*/

	// 150918 YJ added, For falling stage routine
	if( targetTempFlag && !freeRunning ){
		// target temp ���ϰ� �Ǵ� �������� freeRunning ���� 
		if( currentTemp <= m_currentTargetTemp ){
			freeRunning = true;
			freeRunningCounter = 0;
		}
	}

	if( freeRunning ){
		freeRunningCounter++;
		// ���� �ٸ��� 3�� �ĺ��� arrival �� �ν��ϵ��� ����
		if( freeRunningCounter >= (3000/TIMER_DURATION) ){
			targetTempFlag = false;
			freeRunning = false;
			freeRunningCounter = 0;
			isTargetArrival = true;
		}
	}

	if( fabs(currentTemp-m_currentTargetTemp) < m_cArrivalDelta && !targetTempFlag )
		isTargetArrival = true;

	CString tempStr;
	tempStr.Format(L"%3.1f", currentTemp);
	SetDlgItemText(IDC_EDIT_CHAMBER_TEMP, tempStr);
	m_cProgressBar.SetPos((int)currentTemp);

	// ���� ������� PID ���� �����ش�.
	CString pidstr;
	pidstr.Format(L"%.1f", m_kp);
	SetDlgItemText(IDC_EDIT_CURRENT_P, pidstr);
	pidstr.Format(L"%.4f", m_ki);
	SetDlgItemText(IDC_EDIT_CURRENT_I, pidstr);
	pidstr.Format(L"%.1f", m_kd);
	SetDlgItemText(IDC_EDIT_CURRENT_D, pidstr);

	if( isTempGraphOn && isStarted )
		tempGraphDlg.addData(currentTemp);

	// Check the error from device
	static bool onceShow = true;
	if( rx.currentError == ERROR_ASSERT && onceShow ){
		onceShow = false;
		AfxMessageBox(L"Software error occured!\nPlease contact to developer");
	}
	else if( rx.currentError == ERROR_OVERHEAT && onceShow ){
		onceShow = false;
		emergencyStop = true;
		PCREndTask();
	}

	CString out;
	double lights = (double)(photodiode_h & 0x0f)*255. + (double)(photodiode_l);
	
	out.Format(L"%3.1f %d %d", lights, currentCmd, rx.state);
	SetDlgItemText(IDC_EDIT_PHOTODIODE, out);

	// Save the recording data.
	if( isRecording )
	{
		m_recordingCount++;
		out.Format(L"%6d	%8.0f	%3.1f\n", m_recordingCount, (double)(timeGetTime()-m_recStartTime), currentTemp);
		m_recFile.WriteString(out);

		m_cycleCount2++;
		out.Format(L"%6d	%8.0f	%3.1f\n", m_cycleCount2, (double)(timeGetTime()-m_recStartTime), lights);
		m_recPDFile2.WriteString(out);

		// for log message per 1 sec
		if( m_recordingCount % 20 == 0 ){
			int elapsed_time = (int)((double)(timeGetTime()-m_startTime)/1000.);
			int min = elapsed_time/60;
			int sec = elapsed_time%60;
			CString elapseTime, lineTime, totalTime;
			elapseTime.Format(L"%dm %ds", min, sec);

			min = m_nLeftSec/60;
			sec = m_nLeftSec%60;

			// current left protocol time
			if( min == 0 )
				lineTime.Format(L"%ds", sec);
			else
				lineTime.Format(L"%dm %ds", min, sec);
			
			// total left protocol time
			min = m_nLeftTotalSec/60;
			sec = m_nLeftTotalSec%60;

			if( min == 0 )
				totalTime.Format(L"%ds", sec);
			else
				totalTime.Format(L"%dm %ds", min, sec);

			CString log;
			log.Format(L"cmd: %d, targetTemp: %3.1f, temp: %s, elapsed time: %s, line Time: %s, protocol Time: %s, device TargetArr: %d, mfc TargetArr: %d, free Running: %d, free Running Counter: %d, ArrivalDelta: %3.1f, tempFlag: %d, photodiode: %3.1f\n", 
				currentCmd, m_currentTargetTemp, tempStr, elapseTime, lineTime, totalTime, (int)rx.targetArrival, (int)isTargetArrival, (int)freeRunning, (int)freeRunningCounter, m_cArrivalDelta, (int)targetTempFlag, lights);
			FileManager::log(log);
		}
	}

	return FALSE;
}

void CMicroPCRDlg::addSensorValue(double val)
{
	// ������ ����� ��Ʈ�� ���� ��, 
	// ���� ������ double �� vector �� �����Ͽ�
	// �� ���� ������� �ٽ� �׸�.
	sensorValues.push_back(val);
	m_Chart.DeleteAllData();

	int size = sensorValues.size();

	double *data = new double[size*2];
	int	nDims = 2, dims[2] = { 2, size };

	for(int i=0; i<size; ++i)
	{
		data[i] = i;
		data[i+size] = sensorValues[i];
	}

	m_Chart.AddData( data, nDims, dims );

	InvalidateRect(&CRect(15, 350, 1155, 760));
}

void CMicroPCRDlg::clearSensorValue()
{
	sensorValues.clear();
	sensorValues.push_back( 1.0 );

	m_Chart.DeleteAllData();
	InvalidateRect(&CRect(15, 350, 1155, 760));
}

void CMicroPCRDlg::OnBnClickedCheckTempGraph()
{
	CButton* check = (CButton*)GetDlgItem(IDC_CHECK_TEMP_GRAPH);
	isTempGraphOn = check->GetCheck();

	if( isTempGraphOn ){
		if( openConstants )
			SetWindowPos(NULL, 100, GetSystemMetrics(SM_CYSCREEN)/3, 1175, 375, SWP_NOZORDER);

		tempGraphDlg.Create(IDD_DIALOG_TEMP_GRAPH, this);
		
		CRect parent_rect, rect;
		tempGraphDlg.GetClientRect(&rect);
		GetWindowRect(&parent_rect);
		
		tempGraphDlg.SetWindowPos(this, parent_rect.right+10, parent_rect.top, 
			rect.Width(), rect.Height(),  SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
	}
	else
		tempGraphDlg.DestroyWindow();
}

void CMicroPCRDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	CDialog::OnMoving(fwSide, pRect);

	if( isTempGraphOn ){
		CRect parent_rect, rect;
		tempGraphDlg.GetClientRect(&rect);
		GetWindowRect(&parent_rect);
		
		tempGraphDlg.SetWindowPos(this, parent_rect.right+10, parent_rect.top, 
			rect.Width(), rect.Height(),  SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
	}
}

void CMicroPCRDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( isTempGraphOn ){
		CRect parent_rect, rect;
		tempGraphDlg.GetClientRect(&rect);
		GetWindowRect(&parent_rect);
		
		tempGraphDlg.SetWindowPos(this, parent_rect.right+10, parent_rect.top, 
			rect.Width(), rect.Height(),  SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
	}
}
