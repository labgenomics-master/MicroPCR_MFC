#include	"StdAfx.h"
#include	"mmTimers.h"

//������
CMMTimers::CMMTimers(UINT resolution,HWND hwnd) : timerRes(0), timerId(0)
{
	TIMECAPS	tc;

	handle_win=hwnd;

	if (TIMERR_NOERROR == timeGetDevCaps(&tc,sizeof(TIMECAPS)))
	{
		timerRes = min(max(tc.wPeriodMin,resolution),tc.wPeriodMax);
		timeBeginPeriod(timerRes);
	}
}

//�Ҹ���
CMMTimers::~CMMTimers()
{
	stopTimer();
	if (0 != timerRes)
	{
		timeEndPeriod(timerRes);
		timerRes = 0;
	}
}


extern "C"

// Ÿ�̸� ���ν���
void CALLBACK internalTimerProc(UINT id,UINT msg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	CMMTimers *	timer = (CMMTimers *)dwUser;
    
	//timer->timerProc();
	PostMessage(timer->handle_win,WM_MMTIMER, dw1, dw2); 
}

// Ÿ�̸� ����
bool CMMTimers::startTimer(UINT period,bool oneShot)
{
	bool		res = false;
	MMRESULT	result;

	result = timeSetEvent(period,timerRes,internalTimerProc,(DWORD)this,oneShot ? TIME_ONESHOT : TIME_PERIODIC);
	if (NULL != result)
	{
		timerId = (UINT)result;
		res = true;
	}

	return res;
}

//Ÿ�̸� ����
bool CMMTimers::stopTimer()
{
	MMRESULT	result;
	
	result = timeKillEvent(timerId);
	if (TIMERR_NOERROR == result)
		timerId = 0;

	return TIMERR_NOERROR == result;
}
