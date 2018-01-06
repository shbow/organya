//Module   : WinTimer.cpp
//include  : mmsystem.h
//Import   : WinMM.lib
//
//Contents : 「マルチメディアタイマーの使用方法」
//

#include <windows.h>  //Win32n API's
#include <windowsx.h> //mmsystem.h
#include "DefOrg.h"
#include "OrgData.h"

//エラーチェックマクロ
//#define MMInspect(ret)  if((ret) != TIMERR_NOERROR) return FALSE;

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//プロトタイプ宣言
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

BOOL InitMMTimer();
BOOL StartTimer(DWORD dwTimer);
VOID CALLBACK TimerProc(UINT uTID,UINT uMsg,DWORD dwUser,DWORD dwParam1,DWORD dwParam2);
BOOL QuitMMTimer();

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//グローバル変数
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
static UINT ExactTime   = 13;//最小精度
static UINT TimerID     = NULL;

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//タイマー精度を設定する。
//この関数はアプリケーション初期化時に一度呼び出す。
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
BOOL InitMMTimer()
{
	TIMECAPS tc;
	MMRESULT ret;

	//タイマーの精度情報を取得する
	ret = timeGetDevCaps(&tc,sizeof(TIMECAPS));
	if(ret != TIMERR_NOERROR) return FALSE;
	if(ExactTime < tc.wPeriodMin)ExactTime = tc.wPeriodMin;
	//この精度で初期化する
	ret = timeBeginPeriod(ExactTime);
	if(ret != TIMERR_NOERROR) return FALSE;
	return TRUE;
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//タイマーを起動する。
//dwTimer   設定するタイマー間隔
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
BOOL StartTimer(DWORD dwTimer)
{
	MMRESULT ret = NULL;
	ExactTime = dwTimer;
	//タイマーを生成する
	TimerID = timeSetEvent
	(
		dwTimer,       //タイマー時間
		10,             //許容できるタイマー精度
		(LPTIMECALLBACK)TimerProc, //コールバックプロシージャ
		NULL,          //ユーザーがコールバック関数のdwUserに送る情報値
		TIME_PERIODIC //タイマー時間毎にイベントを発生させる
	);
	if(ret != TIMERR_NOERROR) return FALSE;
	return TRUE;
}
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//タイマーのコールバック関数
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
VOID CALLBACK TimerProc(UINT uTID,UINT uMsg,DWORD dwUser,DWORD dwParam1,DWORD dwParam2)
{
	DWORD dwNowTime;
	dwNowTime = timeGetTime();
	//===================================================================================
	//ここにユーザー定義のソースを書く。
	//基本的に関数を呼び出すだけで処理は他の関数でするべきだろう。
	//===================================================================================
	org_data.PlayData();
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//タイマーリソースを開放する。
//アプリケーション終了時に一度呼び出す。
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
BOOL QuitMMTimer()
{
	MMRESULT ret;

	if(TimerID != TIMERR_NOERROR)
	{
		//タイマーを使用中なら終了させる
		ret = timeKillEvent(TimerID);
		if((ret) != TIMERR_NOERROR) return FALSE;
	}
	//タイマーリソースを開放する
	ret = timeEndPeriod(ExactTime);
	if((ret) != TIMERR_NOERROR) return FALSE;
	return TRUE;
}