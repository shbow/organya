#include "Setting.h"
#include "DefOrg.h"
//#include <string.h>
//#include <stdio.h>
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include "Timer.h"
#include "Sound.h"
#include <string.h>//sprintfテスト用

#include "rxoFunction.h"

extern HWND hDlgTrack;	//A 2008/05/13
extern HINSTANCE hInst;//インスタンスハンドル 2010.09.23 A
extern HWND hDlgPlayer;
extern HWND hDlgTrack;
extern BOOL CALLBACK DialogSetting(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK DialogWave(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);

//char timer_sw = 0;
//簡単なダイアログ関数
short track_name[16] = {
	IDC_TRACK0,
	IDC_TRACK1,
	IDC_TRACK2,
	IDC_TRACK3,
	IDC_TRACK4,
	IDC_TRACK5,
	IDC_TRACK6,
	IDC_TRACK7,

	IDC_TRACK8,
	IDC_TRACK9,
	IDC_TRACK10,
	IDC_TRACK11,
	IDC_TRACK12,
	IDC_TRACK13,
	IDC_TRACK14,
	IDC_TRACK15,
};
extern int mute_name[];

extern char timer_sw;
extern HWND hWnd;
extern int sACrnt;	//範囲選択は常にｶﾚﾝﾄﾄﾗｯｸ
extern int tra, ful ,haba; 
extern void SetEZCWindowMessage(char *Mess);
extern char TrackN[];

void ChangeTrackPlus(HWND hdwnd, int iValue){
	char str[8];

	RECT rect = {64,0,WWidth,WHeight};//更新する領域(トラック変更)


	org_data.track += iValue;
	org_data.track =(org_data.track + 16)% 16;
	setRecentTrack(org_data.track); //A 2010.09.23 

	if(timer_sw == 0) PlayOrganKey(36,org_data.track,1000,80);
	//選択トラック表示
	itoa(org_data.track,str,10);
	
	if(sACrnt){
		if(tra>=0){
			tra = org_data.track;
			char wk[256],wk2[256];

			if(ful==0){
				wsprintf(wk2,MessageString[IDS_STRING104], TrackN[tra]);
				strcpy(wk,wk2);
				wsprintf(wk2,MessageString[IDS_STRING105]);
				strcat(wk,wk2);
				SetEZCWindowMessage(wk);
			}
			else if(ful==1){
				wsprintf(wk2,MessageString[IDS_STRING106]);
				strcpy(wk,wk2);
				SetEZCWindowMessage(wk);
			}
		}
	}

	SetDlgItemText(hDlgTrack,IDE_VIEWTRACK,TrackCode[org_data.track]);
	//SetDlgItemText(hdwnd,IDE_VIEWTRACK,str);
	org_data.PutMusic();
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	SetFocus(hWnd);
}


void ChangeTrack(HWND hdwnd, int iTrack){
	char str[8];
	int i;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域(トラック変更)

	i = iTrack;
	org_data.track = i;
	setRecentTrack(org_data.track); //A 2010.09.23 

	if(timer_sw == 0) PlayOrganKey(36,i,1000,80);
	if(sACrnt){
		if(tra>=0){
			tra = org_data.track;
		}
	}
	//選択トラック表示
	itoa(org_data.track,str,10);
	if(sACrnt){
		if(tra>=0){
			tra = org_data.track;
			char wk[256],wk2[256];

			if(ful==0){
				wsprintf(wk2,MessageString[IDS_STRING104], TrackN[tra]);
				strcpy(wk,wk2);
				wsprintf(wk2,MessageString[IDS_STRING105]);
				strcat(wk,wk2);
				SetEZCWindowMessage(wk);
			}
			else if(ful==1){
				wsprintf(wk2,MessageString[IDS_STRING106]);
				strcpy(wk,wk2);
				SetEZCWindowMessage(wk);
			}
		}
	}
	SetDlgItemText(hDlgTrack,IDE_VIEWTRACK,TrackCode[org_data.track]);
	//SetDlgItemText(hdwnd,IDE_VIEWTRACK,str);
	org_data.PutMusic();
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

	SetFocus(hWnd);
}

BOOL CALLBACK DialogTrack(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect = {64,0,WWidth,WHeight};//更新する領域(トラック変更)
	int i, j;
	char str[8];
	MUSICINFO mi;
	
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		org_data.GetMusicInfo(&mi);
		itoa(mi.wait,str,10);
		SetDlgItemText(hdwnd,IDE_VIEWWAIT,str);
		//itoa(org_data.track,str,10);
		str[0]='1';
		str[1]='\0';
		SetDlgItemText(hdwnd,IDE_VIEWTRACK,str);
		for(i = 0; i < MAXTRACK; i++)org_data.mute[i] = 0;
		HANDLE hBmp;
		hBmp = (HBITMAP)LoadImage( hInst, "B_SOLO", IMAGE_BITMAP, 31, 17, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_BTN_SOLO, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "B_LOUPE_PLUS", IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_BTN_LOUPE1, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "B_LOUPE_MINUS", IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_BTN_LOUPE2, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_1", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK0, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_2", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK1, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_3", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK2, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_4", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK3, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_5", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK4, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_6", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK5, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_7", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK6, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_8", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK7, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_Q", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK8, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_W", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK9, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_E", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK10, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_R", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK11, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_T", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK12, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_Y", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK13, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_U", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK14, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		hBmp = (HBITMAP)LoadImage( hInst, "B_TRACK_I", IMAGE_BITMAP, 14, 21, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_TRACK15, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );

		return 1;
	case WM_MOUSEMOVE:
		if(wParam & MK_LBUTTON){
			ReleaseCapture();
			SendMessage( hdwnd, WM_SYSCOMMAND, SC_MOVE | 2, 0 );

		}
	case WM_COMMAND:
		for(i = 0; i < MAXTRACK; i++){
			if(track_name[i] == LOWORD(wParam)){
				if (HIWORD(wParam) == BN_CLICKED) {
					ChangeTrack(hdwnd , i);
					/*
					org_data.track = i;
					PlayOrganKey(36,i,1000);
					//選択トラック表示
					itoa(org_data.track,str,10);
					SetDlgItemText(hdwnd,IDE_VIEWTRACK,str);
					org_data.PutMusic();
					RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
					*/
				}
				
			}
		}
		if(HIWORD(wParam) == BN_CLICKED){
			for(i = 0; i < MAXTRACK; i++){
			//	PlayDramObject(32,1,2);//test
				//if(SendDlgItemMessage(hdwnd,mute_name[i],BM_GETCHECK,0,0))
				if(IsDlgButtonChecked(hdwnd,mute_name[i]))
					org_data.mute[i] = 1;
				else org_data.mute[i] = 0;
			}
			//SetFocus(hWnd);
		}
		switch(LOWORD(wParam)){
		case IDC_BTN_SOLO:	//ソロ
			j = 1;
			for(i = 0; i < MAXTRACK; i++){
				if(org_data.track == i){
					if(org_data.mute[i] != 0){j = 0; break;}
				}else{
					if(org_data.mute[i] != 1){j = 0; break;}
				}
			}
			//j=1のとき、現在Trでミュート済み

			if(j == 1){ //全ミュート解除
				for(i = 0; i < MAXTRACK; i++){
					if(IsDlgButtonChecked(hdwnd,mute_name[i])){
						SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)0, 0L);
						org_data.mute[i] = 0;
					}
				}	
			}else{ //現在Tr以外をミュート
				for(i = 0; i < MAXTRACK; i++){
					if(org_data.track == i){
						if(IsDlgButtonChecked(hdwnd,mute_name[i])){
							SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)0, 0L);
							org_data.mute[i] = 0;
						}
					}else{
						if(IsDlgButtonChecked(hdwnd,mute_name[i])==0){
							SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)1, 0L);
							org_data.mute[i] = 1;
						}
						
					}
				}
			}
			SetFocus(hWnd);		// 2010.11.30 A
			break;
		case IDC_BTN_LOUPE1: //PLUS 2014.05.27
			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_LOUPE_PLUS, 0L);
			SetFocus(hWnd);		// 
			break;
		case IDC_BTN_LOUPE2: //MINUS 
			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_LOUPE_MINUS, 0L);
			SetFocus(hWnd);		// 2010.11.30 A
			break;
		case IDC_BTN_FULL:	//フル
			{
				int ib = 0;
				for(i = 0; i < MAXTRACK; i++){
					if(IsDlgButtonChecked(hdwnd,mute_name[i])){
						ib = 1;
					}
				}
				if(ib!=0){ //全ミュート解除
					for(i = 0; i < MAXTRACK; i++){
						if(IsDlgButtonChecked(hdwnd,mute_name[i])){
							SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)0, 0L);
							org_data.mute[i] = 0;
						}
					}	
				}else{ //全ミュート
					for(i = 0; i < MAXTRACK; i++){
						if(IsDlgButtonChecked(hdwnd,mute_name[i])==0){
							SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)1, 0L);
							org_data.mute[i] = 1;
						}
					}	
					
				}
			}
			SetFocus(hWnd);		// 2010.11.30 A
			break;
		//A2008/05/13
		case IDC_BTN_MELO:	//メロディのみをON-OFF
			{
				int ib = 0;
				for(i = 0; i < MAXMELODY; i++){
					if(IsDlgButtonChecked(hdwnd,mute_name[i])){
						ib = 1;
					}
				}
				if(ib!=0){ //全ミュート解除
					for(i = 0; i < MAXMELODY; i++){
						if(IsDlgButtonChecked(hdwnd,mute_name[i])){
							SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)0, 0L);
							org_data.mute[i] = 0;
						}
					}	
				}else{ //全ミュート
					for(i = 0; i < MAXMELODY; i++){
						if(IsDlgButtonChecked(hdwnd,mute_name[i])==0){
							SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)1, 0L);
							org_data.mute[i] = 1;
						}
					}	
				}
			}
			SetFocus(hWnd);		// 2010.11.30 A
			break;
		case IDC_BTN_DRUM:	//ドラムのみをON-OFF
			{
				int ib = 0;
				for(i = MAXMELODY; i < MAXTRACK; i++){
					if(IsDlgButtonChecked(hdwnd,mute_name[i])){
						ib = 1;
					}
				}
				if(ib!=0){ //全ミュート解除
					for(i = MAXMELODY; i < MAXTRACK; i++){
						if(IsDlgButtonChecked(hdwnd,mute_name[i])){
							SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)0, 0L);
							org_data.mute[i] = 0;
						}
					}	
				}else{ //全ミュート
					for(i = MAXMELODY; i < MAXTRACK; i++){
						if(IsDlgButtonChecked(hdwnd,mute_name[i])==0){
							SendMessage(GetDlgItem(hdwnd, mute_name[i]), BM_SETCHECK, (WPARAM)1, 0L);
							org_data.mute[i] = 1;
						}
					}	
				}
			}
			SetFocus(hWnd);		// 2010.11.30 A
			break;
		case IDC_TR_WAIT:
			DialogBox(hInst,"DLGSETTING",hdwnd,DialogSetting);
			SetFocus(hWnd);		// 2010.11.30 A
			break;
		case IDC_TR_TRACK: //トラックのTRボタン
			DialogBox(hInst,"DLGWAVE",hdwnd,DialogWave);
			SetFocus(hWnd);		// 2010.11.30 A
			break;

		}
		return 1;
	}
	return 0;
}