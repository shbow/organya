#include "Setting.h"
#include "DefOrg.h"
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include "Timer.h"
#include "Sound.h"
#include "Scroll.h"
char timer_sw = 0;
extern HWND hDlgTrack;
extern HWND hWnd;

int mute_name[MAXTRACK] = {
	IDC_MUTE0,
	IDC_MUTE1,
	IDC_MUTE2,
	IDC_MUTE3,
	IDC_MUTE4,
	IDC_MUTE5,
	IDC_MUTE6,
	IDC_MUTE7,
	IDC_MUTE8,
	IDC_MUTE9,
	IDC_MUTE10,
	IDC_MUTE11,
	IDC_MUTE12,
	IDC_MUTE13,
	IDC_MUTE14,
	IDC_MUTE15,
};
//簡単なダイアログ関数
BOOL CALLBACK DialogPlayer(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam){
	int i;
	HMENU hmenu,hsubmenu;
	MUSICINFO mi;
	HFONT hFont1;
	HWND  hCtrlWnd;
	long hp,vp;
	char str[32];
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		HANDLE hBmp;
		org_data.GetMusicInfo(&mi);
		itoa(mi.wait,str,10);
		SetDlgItemText(hdwnd,IDE_VIEWWAIT,str);
		org_data.SetPlayPointer(0);//頭出し
		//プレイヤーに表示
		//hFont1 = CreateFont(0, 0, 0, 0, 900, 0, 0, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック");
		hFont1 = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial Black");
		hCtrlWnd = GetDlgItem(hdwnd, IDE_VIEWMEAS);
		SendMessage(hCtrlWnd, WM_SETFONT, (WPARAM)hFont1, MAKELPARAM(FALSE, 0)); /* フォントの変更 */
		SetDlgItemText(hdwnd,IDE_VIEWMEAS,"0");

		hCtrlWnd = GetDlgItem(hdwnd, IDE_VIEWXPOS);
		SendMessage(hCtrlWnd, WM_SETFONT, (WPARAM)hFont1, MAKELPARAM(FALSE, 0)); /* フォントの変更 */
		SetDlgItemText(hdwnd,IDE_VIEWXPOS,"0");
		DeleteObject(hFont1); //フォントオブジェクトの削除

		//画像張りつけ
/*
		hBmp = (HBITMAP)LoadImage( hInst, "HEAD", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_START, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "PLAY", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_PLAY, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "STOP", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_STOP, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "LEFT", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_LEFT, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "RIGHT", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_RIGHT, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
*/
		hBmp = (HBITMAP)LoadImage( hInst, "B_HEAD", IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_START, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "B_PLAY", IMAGE_BITMAP, 40, 24, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_PLAY, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "B_STOP", IMAGE_BITMAP, 40, 24, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_STOP, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "B_LEFT", IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_LEFT, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "B_RIGHT", IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_RIGHT, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		return 1;
	case WM_MOUSEMOVE:
		if(wParam & MK_LBUTTON){
			ReleaseCapture();
			SendMessage( hdwnd, WM_SYSCOMMAND, SC_MOVE | 2, 0 );

		}
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_PLAY:
			if(timer_sw == 0){
				
				//メニューを無効にする。
				hmenu = GetMenu(hWnd);
				hsubmenu = GetSubMenu(hmenu,0);
				EnableMenuItem(hmenu,0,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,1,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,2,MF_BYPOSITION|MF_DISABLED);
				//EnableMenuItem(hmenu,5,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,4,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,3,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,6,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,7,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,8,MF_BYPOSITION|MF_DISABLED);
				DragAcceptFiles(hWnd,FALSE);//ドラッグ禁止
				//トラック ミュート のチェック
				for(i = 0; i < MAXTRACK; i++){
					if(SendDlgItemMessage(hDlgTrack,mute_name[i],BM_GETCHECK,0,0)){
						org_data.mute[i] = 1;
					}else{
						org_data.mute[i] = 0;
					}
				}
				//プレイポインターの設定
				scr_data.GetScrollPosition(&hp,&vp);
				org_data.GetMusicInfo(&mi);
				org_data.SetPlayPointer(hp*mi.dot*mi.line);
				timer_sw = 1;
				InitMMTimer();
				//テンポを取得
				MUSICINFO mi;
				org_data.GetMusicInfo(&mi);
				StartTimer(mi.wait);
				SetFocus(hWnd);
			}
			return 1;
		case IDC_STOP:
		case IDC_START:
			if(timer_sw){
				//メニューを有効にする。
				hmenu = GetMenu(hWnd);
				hsubmenu = GetSubMenu(hmenu,0);
				EnableMenuItem(hmenu,0,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,1,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,2,MF_BYPOSITION|MF_ENABLED);
				//EnableMenuItem(hmenu,5,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,4,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,3,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,6,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,7,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,8,MF_BYPOSITION|MF_ENABLED);
				DragAcceptFiles(hWnd,TRUE);//ドラッグ許可
				org_data.GetMusicInfo(&mi);
				Rxo_StopAllSoundNow();	// 2010.11.30 C
				/*
				for(int i = 0; i < MAXMELODY; i++)
					PlayOrganObject(NULL,2 ,i,NULL);
					*/
				QuitMMTimer();
				timer_sw = 0;
			}
			SetFocus(hWnd);
//			return 1;
//			if(timer_sw){//ストップの効果も
//				//メニューを有効にする。
//				hmenu = GetMenu(hWnd);
//				hsubmenu = GetSubMenu(hmenu,0);
//				EnableMenuItem(hmenu,0,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,1,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,2,MF_BYPOSITION|MF_ENABLED);
//				//EnableMenuItem(hmenu,5,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,4,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,3,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,6,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,7,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,8,MF_BYPOSITION|MF_ENABLED);
//				DragAcceptFiles(hWnd,TRUE);//ドラッグ許可
//				org_data.GetMusicInfo(&mi);
//				Rxo_StopAllSoundNow();	// 2010.11.30 C
//				/*
//				for(int i = 0; i < MAXMELODY; i++)
//					PlayOrganObject(NULL,2 ,i,NULL);
//					*/
//				QuitMMTimer();
//				timer_sw = 0;
//				SetFocus(hWnd);
//			}
			if(LOWORD(wParam) == IDC_START){
//				PlayOrganKey(36,9,1000);
				SetDlgItemText(hdwnd,IDE_VIEWMEAS,"0");
				SetDlgItemText(hdwnd,IDE_VIEWXPOS,"0");
				scr_data.SetHorzScroll(0);
				org_data.SetPlayPointer(0);//頭出し
				SetFocus(hWnd);
			}
			return 1;
		case IDC_LEFT:
			scr_data.HorzScrollProc( SB_LINELEFT );
			SetFocus(hWnd);
			return 1;
		case IDC_RIGHT:
			scr_data.HorzScrollProc( SB_LINERIGHT );
			SetFocus(hWnd);
			return 1;
		}
		//case WM_PAINT:
		//	break;
			/*
			HDC hdc;
			PAINTSTRUCT ps;
			RECT rc1={0,0,200,36},rc2={0,0,1,36},rc3={0,0,200,1};
			HBRUSH brh1,brh2;
			hdc = BeginPaint(hdwnd,&ps);
			brh1 = CreateSolidBrush(0x323232); //0x00bbggrr 
			brh2 = CreateSolidBrush(0x808080); //0x00bbggrr 
			FillRect(hdc, &rc1, brh1);
			FillRect(hdc, &rc2, brh2);
			FillRect(hdc, &rc3, brh2);

			DeleteObject(brh1);
			EndPaint(hWnd,&ps);
			*/
		//	break;
	case WM_CLOSE:
		//DeleteObject(hFont1); //フォントオブジェクトの削除
		return 1;
	}
	return 0;
}