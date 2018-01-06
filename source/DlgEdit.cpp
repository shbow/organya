#include "Setting.h"
#include "DefOrg.h"
#include <string.h>
#include <stdio.h>
#include "Sound.h"
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include "rxoFunction.h"

extern HWND hDlgPlayer;
extern HWND hDlgTrack;
extern NOTECOPY nc_Select; //選択範囲
extern int tra, ful ,haba; 
extern char TrackN[];

BOOL CALLBACK DialogDelete(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[128] = {NULL};
	int v;
	long a,b,c;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	MUSICINFO mi;
	PARCHANGE pc;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		EnableWindow(hDlgPlayer,FALSE);

		//itoa(org_data.track,str,10);
		//SetDlgItemText(hdwnd,IDE_DELTRACK,str);
		SetDlgItemText(hdwnd,IDE_DELTRACK,TrackCode[org_data.track]);
		if(tra<0){
			SetDlgItemText(hdwnd,IDE_DELFROM,"");
			SetDlgItemText(hdwnd,IDE_DELTO,"");
		}else{
			v = GetSelectMeasBeat(GET_MEAS1);
			SetDlgItemInt(hdwnd,IDE_DELFROM,v,FALSE);
			v = GetSelectMeasBeat(GET_MEAS2);
			SetDlgItemInt(hdwnd,IDE_DELTO,v,FALSE);
		}
		
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableWindow(hDlgPlayer,TRUE);
			return 1;
		case IDOK:
			GetDlgItemText(hdwnd,IDE_DELTRACK,str,4);
			//a = atol(str);
			a = ReverseTrackCode(str);
			if(a >= MAXTRACK){
				//MessageBox(hdwnd,"トラックが異常です","Error(track)",MB_OK);

				return 1;
			}
			//パラメータ生成
			pc.track = (char)a;
			org_data.GetMusicInfo(&mi);
			GetDlgItemText(hdwnd,IDE_DELFROM,str,4);
			b = atol(str);
			pc.x1 = b * mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_DELTO,str,4);
			c = atol(str);
			pc.x2 = c * mi.dot*mi.line-1;
			if(b >= c){
				//MessageBox(hdwnd,"範囲が異常です","Error(track)",MB_OK);
				msgbox(hdwnd, IDS_WARNING_HANI, IDS_ERROR_TRACK, MB_OK);	// 2014.10.18 
				return 1;
			}
			SetUndo();
			//消去
			org_data.DelateNoteData(&pc);
			//表示
			org_data.PutMusic();
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			//MessageBox(hdwnd,"消去しました","通知",MB_OK);	// 2014.10.18 D
			msgbox(hdwnd,IDS_INFO_CLEAR ,IDS_NOTIFY_TITLE ,MB_OK);	// 2014.10.18 A
			return 1;
		}
	}
	return 0;
}
//コピーダイアログ
BOOL CALLBACK DialogCopy(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[128] = {NULL};
	long a,b,c;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	MUSICINFO mi;
//	PARCHANGE pc;
	NOTECOPY nc;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		//itoa(org_data.track,str,10);
		//SetDlgItemText(hdwnd,IDE_TRACK1,str);
		//SetDlgItemText(hdwnd,IDE_TRACK2,str);
		if(tra<0){
			SetDlgItemText(hdwnd,IDE_TRACK1,TrackCode[org_data.track]);
			SetDlgItemText(hdwnd,IDE_TRACK2,TrackCode[org_data.track]);
			SetDlgItemText(hdwnd,IDE_MEAS1_1,"");
			{
				char c[32];
				GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS, c , 32);
				SetDlgItemText(hdwnd,IDE_MEAS1_1,c);
			}
			SetDlgItemText(hdwnd,IDE_MEAS1_2,"");
			SetDlgItemText(hdwnd,IDE_BEAT1_1,"0");
			SetDlgItemText(hdwnd,IDE_BEAT1_2,"0");
			SetDlgItemText(hdwnd,IDE_MEAS2,"");
			SetDlgItemText(hdwnd,IDE_BEAT2,"0");
		}else{
			SetDlgItemText(hdwnd,IDE_TRACK1,TrackCode[tra]);
			SetDlgItemText(hdwnd,IDE_TRACK2,TrackCode[org_data.track]);
			{
				char c[32];
				GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS, c , 32);
				SetDlgItemText(hdwnd,IDE_MEAS1_1,c);
			}
			SetDlgItemInt(hdwnd,IDE_MEAS1_1,GetSelectMeasBeat(GET_MEAS1),FALSE);
			SetDlgItemInt(hdwnd,IDE_MEAS1_2,GetSelectMeasBeat(GET_MEAS2),FALSE);
			SetDlgItemInt(hdwnd,IDE_BEAT1_1,GetSelectMeasBeat(GET_BEAT1),FALSE);
			SetDlgItemInt(hdwnd,IDE_BEAT1_2,GetSelectMeasBeat(GET_BEAT2),FALSE);
			SetDlgItemText(hdwnd,IDE_MEAS2,"");
			SetDlgItemText(hdwnd,IDE_BEAT2,"0");
			
		}
		SetDlgItemText(hdwnd,IDE_COPYNUM,"1");
		EnableWindow(hDlgPlayer,FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableWindow(hDlgPlayer,TRUE);
			return 1;
		case IDOK:
			//コピー回数のチェック
			GetDlgItemText(hdwnd,IDE_COPYNUM,str,4);
			if(atol(str) < 1){
				//MessageBox(hdwnd,"コピー回数が異常です","Error(Copy)",MB_OK);	// 2014.10.18 D
				msgbox(hdwnd,IDS_WARNING_COPY_KAISUU ,IDS_ERROR_COPY, MB_OK);	// 2014.10.18 A
				return 1;
			}
			//トラックのチェック
			GetDlgItemText(hdwnd,IDE_TRACK1,str,4);
			if(ReverseTrackCode(str) >= MAXTRACK){
				//MessageBox(hdwnd,"From Trackが異常です","Error(Copy)",MB_OK);	// 2014.10.18 D
				msgbox(hdwnd,IDS_WARNING_FROM_TRACK, IDS_ERROR_COPY, MB_OK);	// 2014.10.18 A
				return 1;
			}else nc.track1 = (char)ReverseTrackCode(str);
			GetDlgItemText(hdwnd,IDE_TRACK2,str,4);
			if(ReverseTrackCode(str) >= MAXTRACK){
				//MessageBox(hdwnd,"To Trackが異常です","Error(Copy)",MB_OK);	// 2014.10.18 D
				msgbox(hdwnd,IDS_WARNING_TO_TRACK, IDS_ERROR_COPY, MB_OK);	// 2014.10.18 A
				return 1;
			}else nc.track2= (char)ReverseTrackCode(str);
			//コピー範囲のチェック
			org_data.GetMusicInfo(&mi);
			GetDlgItemText(hdwnd,IDE_MEAS1_1,str,4);//範囲from
			a = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_BEAT1_1,str,4);
			if(atol(str) >= mi.dot*mi.line){
				//MessageBox(hdwnd,"From Beatが異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_FROM_BEAT,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			a += atol(str);
			GetDlgItemText(hdwnd,IDE_MEAS1_2,str,4);//範囲to
			b = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_BEAT1_2,str,4);
			if(atol(str) >= mi.dot*mi.line){
				//MessageBox(hdwnd,"From Beatが異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_FROM_BEAT,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			b += atol(str)-1;
			GetDlgItemText(hdwnd,IDE_MEAS2,str,4);//コピー先to
			c = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_BEAT2,str,4);
			if(atol(str) >= mi.dot*mi.line){
				//MessageBox(hdwnd,"To Beatが異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_TO_BEAT,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			if(a >= b){
				//MessageBox(hdwnd,"コピー範囲が異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_COPY_HANI,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			c += atol(str);
			nc.x1_1 = a;
			nc.x1_2 = b;
			nc.x2 = c;
			GetDlgItemText(hdwnd,IDE_COPYNUM,str,4);//コピー先to
			nc.num = atol(str);

			SetUndo();
			org_data.CopyNoteData(&nc);
			org_data.CheckNoteTail(nc.track2);
			//表示
			org_data.PutMusic();
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			//試しに表示
//			sprintf(str,"%dトラックの%dから%dまでを、%dトラックの%dに%d回コピーしました",
//			nc.track1,nc.x1_1,nc.x1_2,nc.track2,nc.x2,nc.num);
			//MessageBox(hdwnd,"コピーしました","通知",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_COPY,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A
			return 1;
		}
	}
	return 0;
}

//パンチェンジダイアログ
BOOL CALLBACK DialogPan(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[128] = {NULL};
//	long a,b,c,d;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	PARCHANGE pc;
	MUSICINFO mi;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		SendDlgItemMessage(hdwnd,IDR_ADD,BM_SETCHECK,1,0);
		//itoa(org_data.track,str,10);
		//SetDlgItemText(hdwnd,IDE_TRACK,str);
		SetDlgItemText(hdwnd,IDE_TRACK,TrackCode[org_data.track]);
		if(tra<0){
			SetDlgItemText(hdwnd,IDE_MEAS1,"");
			SetDlgItemText(hdwnd,IDE_MEAS2,"");
		}else{
			SetDlgItemInt(hdwnd,IDE_MEAS1,GetSelectMeasBeat(GET_MEAS1),FALSE);
			SetDlgItemInt(hdwnd,IDE_MEAS2,GetSelectMeasBeat(GET_MEAS2),FALSE);
		}
		SetDlgItemText(hdwnd,IDE_PAR,"1");
		EnableWindow(hDlgPlayer,FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableWindow(hDlgPlayer,TRUE);
			return 1;
		case IDOK:
			if(SendDlgItemMessage(hdwnd,IDR_ADD,BM_GETCHECK,0,0))
				pc.mode = MODEPARADD;
			else if(SendDlgItemMessage(hdwnd,IDR_SUB,BM_GETCHECK,0,0))
				pc.mode = MODEPARSUB;
			//トラックのチェック
			GetDlgItemText(hdwnd,IDE_TRACK,str,4);
			if(ReverseTrackCode(str) >= MAXTRACK){
				//MessageBox(hdwnd,"Trackが異常です","Error(Pan)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_TRACK,IDS_ERROR_PAN,MB_OK);	// 2014.10.19 A
				return 1;
			}else pc.track = (char)ReverseTrackCode(str);
			//範囲のチェック
			org_data.GetMusicInfo(&mi);
			GetDlgItemText(hdwnd,IDE_MEAS1,str,4);//範囲from
			pc.x1 = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_MEAS2,str,4);//範囲to
			pc.x2 = atol(str)*mi.dot*mi.line - 1;
			GetDlgItemText(hdwnd,IDE_PAR,str,4);//値
			pc.a = (unsigned char)atol(str);
			if(pc.x1 >= pc.x2){
				//MessageBox(hdwnd,"範囲が異常です","Error(Pan)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_HANI,IDS_ERROR_PAN,MB_OK);	// 2014.10.19 A
				return 1;
			}
			SetUndo();
			org_data.ChangePanData(&pc);
			//表示
			org_data.PutMusic();
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			//MessageBox(hdwnd,"指定範囲のパンを変更しました","通知",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_CHANGE_PAN,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A
			return 1;
		}
	}
	return 0;
}
//トランスポーズダイアログ
BOOL CALLBACK DialogTrans(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[128] = {NULL};
//	long a,b,c,d;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	PARCHANGE pc;
	MUSICINFO mi;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		SendDlgItemMessage(hdwnd,IDR_ADD,BM_SETCHECK,1,0);
		//itoa(org_data.track,str,10);
		//SetDlgItemText(hdwnd,IDE_TRACK,str);
		SetDlgItemText(hdwnd,IDE_TRACK,TrackCode[org_data.track]);
		if(tra<0){
			SetDlgItemText(hdwnd,IDE_MEAS1,"");
			SetDlgItemText(hdwnd,IDE_MEAS2,"");
		}else{
			SetDlgItemInt(hdwnd,IDE_MEAS1,GetSelectMeasBeat(GET_MEAS1),FALSE);
			SetDlgItemInt(hdwnd,IDE_MEAS2,GetSelectMeasBeat(GET_MEAS2),FALSE);
		}
		SetDlgItemText(hdwnd,IDE_PAR,"1");
		EnableWindow(hDlgPlayer,FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableWindow(hDlgPlayer,TRUE);
			return 1;
		case IDOK:
			if(SendDlgItemMessage(hdwnd,IDR_ADD,BM_GETCHECK,0,0))
				pc.mode = MODEPARADD;
			else if(SendDlgItemMessage(hdwnd,IDR_SUB,BM_GETCHECK,0,0))
				pc.mode = MODEPARSUB;
			
			//トラックのチェック
			GetDlgItemText(hdwnd,IDE_TRACK,str,4);
			if(ReverseTrackCode(str) >= MAXTRACK){
				//MessageBox(hdwnd,"Trackが異常です","Error(Trans)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_TRACK,IDS_ERROR_TRANS,MB_OK);	// 2014.10.19 A
				return 1;
			}else pc.track = (char)ReverseTrackCode(str);
			//範囲のチェック
			org_data.GetMusicInfo(&mi);
			GetDlgItemText(hdwnd,IDE_MEAS1,str,4);//範囲from
			pc.x1 = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_MEAS2,str,4);//範囲to
			pc.x2 = atol(str)*mi.dot*mi.line - 1;
			GetDlgItemText(hdwnd,IDE_PAR,str,4);//値
			pc.a = (unsigned char)atol(str);
			if(pc.x1 >= pc.x2){
				//MessageBox(hdwnd,"範囲が異常です","Error(Trans)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_HANI,IDS_ERROR_TRANS,MB_OK);	// 2014.10.19 A
				return 1;
			}
			SetUndo();
			org_data.ChangeTransData(&pc);
			//表示
			org_data.PutMusic();
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			//MessageBox(hdwnd,"指定範囲のキーを変更しました","通知",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_CHANGE_KEY,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A
			return 1;
		}
	}
	return 0;
}
//ヴォリュームチェンジダイアログ
BOOL CALLBACK DialogVolume(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[128] = {NULL};
//	long a,b,c,d;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	PARCHANGE pc;
	MUSICINFO mi;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		SendDlgItemMessage(hdwnd,IDR_ADD,BM_SETCHECK,1,0);
		//itoa(org_data.track,str,10);
		//SetDlgItemText(hdwnd,IDE_TRACK,str);
		SetDlgItemText(hdwnd,IDE_TRACK,TrackCode[org_data.track]);
		if(tra<0){
			SetDlgItemText(hdwnd,IDE_MEAS1,"");
			SetDlgItemText(hdwnd,IDE_MEAS2,"");
		}else{
			SetDlgItemInt(hdwnd,IDE_MEAS1,GetSelectMeasBeat(GET_MEAS1),FALSE);
			SetDlgItemInt(hdwnd,IDE_MEAS2,GetSelectMeasBeat(GET_MEAS2),FALSE);
		}
		SetDlgItemText(hdwnd,IDE_PAR,"1");
		EnableWindow(hDlgPlayer,FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableWindow(hDlgPlayer,TRUE);
			return 1;
		case IDOK:
			if(SendDlgItemMessage(hdwnd,IDR_ADD,BM_GETCHECK,0,0))
				pc.mode = MODEPARADD;
			else if(SendDlgItemMessage(hdwnd,IDR_SUB,BM_GETCHECK,0,0))
				pc.mode = MODEPARSUB;
			//トラックのチェック
			GetDlgItemText(hdwnd,IDE_TRACK,str,4);
			if(ReverseTrackCode(str) >= MAXTRACK){
				//MessageBox(hdwnd,"Trackが異常です","Error(Volume)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_TRACK,IDS_ERROR_VOLUME,MB_OK);	// 2014.10.19 A
				return 1;
			}else pc.track = (char)ReverseTrackCode(str);
			//範囲のチェック
			org_data.GetMusicInfo(&mi);
			GetDlgItemText(hdwnd,IDE_MEAS1,str,4);//範囲from
			pc.x1 = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_MEAS2,str,4);//範囲to
			pc.x2 = atol(str)*mi.dot*mi.line - 1;
			GetDlgItemText(hdwnd,IDE_PAR,str,4);//値
			pc.a = (unsigned char)atol(str);
			if(pc.x1 >= pc.x2){
				//MessageBox(hdwnd,"範囲が異常です","Error(Volume)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_HANI,IDS_ERROR_VOLUME,MB_OK);	// 2014.10.19 A
				return 1;
			}
			SetUndo();
			org_data.ChangeVolumeData(&pc);
			//表示
			org_data.PutMusic();
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			//MessageBox(hdwnd,"指定範囲のボリュームを変更しました","通知",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_CHANGE_VOLUME,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A
			return 1;
		}
	}
	return 0;
}
//コピーダイアログ2
int cbox[MAXTRACK] = {
	IDC_USE0,
	IDC_USE1,
	IDC_USE2,
	IDC_USE3,
	IDC_USE4,
	IDC_USE5,
	IDC_USE6,
	IDC_USE7,

	IDC_USE8,
	IDC_USE9,
	IDC_USE10,
	IDC_USE11,
	IDC_USE12,
	IDC_USE13,
	IDC_USE14,
	IDC_USE15,
};
BOOL CALLBACK DialogCopy2(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	char str[128] = {NULL};
	long a,b,c;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	MUSICINFO mi;
//	PARCHANGE pc;
	NOTECOPY nc;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
//		itoa(org_data.track,str,10);
//		SetDlgItemText(hdwnd,IDE_TRACK1,str);
//		SetDlgItemText(hdwnd,IDE_TRACK2,str);

		//SendDlgItemMessage(hdwnd,cbox[org_data.track],BM_SETCHECK,1,0);
		for(i=0;i<16;i++)SendDlgItemMessage(hdwnd,cbox[i],BM_SETCHECK,1,0);
		if(tra<0){
			SetDlgItemText(hdwnd,IDE_MEAS1_1,"");
			{
				char c[32];
				GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS, c , 32);
				SetDlgItemText(hdwnd,IDE_MEAS1_1,c);
			}
			SetDlgItemText(hdwnd,IDE_MEAS1_2,"");
			SetDlgItemText(hdwnd,IDE_BEAT1_1,"0");
			SetDlgItemText(hdwnd,IDE_BEAT1_2,"0");
		}else{
			SetDlgItemInt(hdwnd,IDE_MEAS1_1,GetSelectMeasBeat(GET_MEAS1),FALSE);
			SetDlgItemInt(hdwnd,IDE_MEAS1_2,GetSelectMeasBeat(GET_MEAS2),FALSE);
			SetDlgItemInt(hdwnd,IDE_BEAT1_1,GetSelectMeasBeat(GET_BEAT1),FALSE);
			SetDlgItemInt(hdwnd,IDE_BEAT1_2,GetSelectMeasBeat(GET_BEAT2),FALSE);
		}
		SetDlgItemText(hdwnd,IDE_MEAS2,"");
		SetDlgItemText(hdwnd,IDE_BEAT2,"0");
		SetDlgItemText(hdwnd,IDE_COPYNUM,"1");
		EnableWindow(hDlgPlayer,FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_SETCOPY_FULLCLICK:
			for(i=0;i<16;i++)SendDlgItemMessage(hdwnd,cbox[i],BM_SETCHECK,1,0);

			return 1;
		case IDC_SETCOPY_FULLCLICK2:
			for(i=0;i<16;i++)SendDlgItemMessage(hdwnd,cbox[i],BM_SETCHECK,0,0);
			SendDlgItemMessage(hdwnd,cbox[org_data.track],BM_SETCHECK,1,0);
			return 1;
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableWindow(hDlgPlayer,TRUE);
			return 1;
		case IDOK:
			//コピー回数のチェック
			GetDlgItemText(hdwnd,IDE_COPYNUM,str,4);
			if(atol(str) < 1){
				//MessageBox(hdwnd,"コピー回数が異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_COPY_KAISUU,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			//トラックのチェック
//			GetDlgItemText(hdwnd,IDE_TRACK1,str,4);
//			if(atol(str) >= MAXTRACK){
//				MessageBox(hdwnd,"From Trackが異常です","Error(Copy)",MB_OK);
//				return 1;
//			}else nc.track1 = (char)atol(str);
//			GetDlgItemText(hdwnd,IDE_TRACK2,str,4);
//			if(atol(str) >= MAXTRACK){
//				MessageBox(hdwnd,"To Trackが異常です","Error(Copy)",MB_OK);
//				return 1;
//			}else nc.track2= (char)atol(str);
			//コピー範囲のチェック
			org_data.GetMusicInfo(&mi);
			GetDlgItemText(hdwnd,IDE_MEAS1_1,str,4);//範囲from
			a = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_BEAT1_1,str,4);
			if(atol(str) >= mi.dot*mi.line){
				//MessageBox(hdwnd,"From Beatが異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_FROM_BEAT,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			a += atol(str);
			GetDlgItemText(hdwnd,IDE_MEAS1_2,str,4);//範囲to
			b = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_BEAT1_2,str,4);
			if(atol(str) >= mi.dot*mi.line){
				//MessageBox(hdwnd,"From Beatが異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_FROM_BEAT,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			b += atol(str)-1;
			GetDlgItemText(hdwnd,IDE_MEAS2,str,4);//コピー先to
			c = atol(str)*mi.dot*mi.line;
			GetDlgItemText(hdwnd,IDE_BEAT2,str,4);
			if(atol(str) >= mi.dot*mi.line){
				//MessageBox(hdwnd,"To Beatが異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_TO_BEAT,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			if(a >= b){
				//MessageBox(hdwnd,"コピー範囲が異常です","Error(Copy)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_WARNING_COPY_HANI,IDS_ERROR_COPY,MB_OK);	// 2014.10.19 A
				return 1;
			}
			c += atol(str);
			nc.x1_1 = a;
			nc.x1_2 = b;
			nc.x2 = c;
			GetDlgItemText(hdwnd,IDE_COPYNUM,str,4);//コピー先to
			nc.num = atol(str);
			SetUndo();
			for(i = 0; i < MAXTRACK; i++){
				if(SendDlgItemMessage(hdwnd,cbox[i],BM_GETCHECK,0,0) == 1){
					nc.track1 = nc.track2 = i;
					org_data.CopyNoteData(&nc);
					org_data.CheckNoteTail(nc.track2);
				}
			}
			//表示
			org_data.PutMusic();
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			//MessageBox(hdwnd,"コピーしました","通知",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_COPY,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A
			return 1;
		}
	}
	return 0;
}

BOOL CALLBACK DialogSwap(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[128] = {NULL};
//	long a,b,c,d;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	NOTECOPY nc;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		if(tra<0){
			SetDlgItemText(hdwnd,IDE_TRACK1,TrackCode[org_data.track]);
			SetDlgItemText(hdwnd,IDE_TRACK2,TrackCode[org_data.track]);
		}else{
			SetDlgItemText(hdwnd,IDE_TRACK1,TrackCode[tra]);
			SetDlgItemText(hdwnd,IDE_TRACK2,TrackCode[org_data.track]);
		}
		EnableWindow(hDlgPlayer,FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableWindow(hDlgPlayer,TRUE);
			return 1;
		case IDOK:
			//トラックのチェック
			GetDlgItemText(hdwnd,IDE_TRACK1,str,4);
			if(ReverseTrackCode(str) >= MAXTRACK){
				//MessageBox(hdwnd,"左 Trackが異常です","Error(Swap)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_ERROR_L_TRACK,IDS_ERROR_SWAP,MB_OK);	// 2014.10.19 A
				return 1;
			}else nc.track1 = (char)ReverseTrackCode(str);
			GetDlgItemText(hdwnd,IDE_TRACK2,str,4);
			if(ReverseTrackCode(str) >= MAXTRACK){
				//MessageBox(hdwnd,"右 Trackが異常です","Error(Swap)",MB_OK);	// 2014.10.19 D
				msgbox(hdwnd,IDS_ERROR_R_TRACK,IDS_ERROR_SWAP,MB_OK);	// 2014.10.19 A
				return 1;
			}else nc.track2= (char)ReverseTrackCode(str);

			SetUndo();
			org_data.SwapTrack(&nc);
			//表示
			org_data.PutMusic();
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			//MessageBox(hdwnd,"指定Trackを入れ替えました。","通知",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_SWAP_TRACK,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A
			return 1;
		}
	}
	return 0;
}
