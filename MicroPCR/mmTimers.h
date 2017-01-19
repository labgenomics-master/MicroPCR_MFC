#ifndef	___multimedia_timers___
#define	___multimedia_timers___
#pragma comment(lib, "winmm.lib")

#include	<mmsystem.h>

#define WM_MMTIMER WM_USER + 2

class CMMTimers
{
public:
	//�ڵ�
	HWND handle_win;

	//������
	CMMTimers(UINT resolution, HWND hwnd);

	//�Ҹ���
	virtual ~CMMTimers();

	//Ÿ�̸� ����
	UINT	getTimerRes() { return timerRes; };

	//Ÿ�̸� ����
	bool	startTimer(UINT period,bool oneShot);

	//Ÿ�̸� ����
	bool	stopTimer();

	//Ÿ�̸� ���ν���
	virtual void timerProc() {};

protected:
	UINT	timerRes;
	UINT	timerId;
};


#endif