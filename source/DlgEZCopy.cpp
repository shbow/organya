#include "Setting.h"
#include "DefOrg.h"
//#include <string.h>
//#include <stdio.h>
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include "Timer.h"
#include "Sound.h"
#include <string.h>
#include "Scroll.h"
#include "rxoFunction.h"
#include <stdio.h>

extern int mute_name[];

extern char timer_sw;
extern HWND hWnd;
extern HWND hDlgPlayer;
extern HWND hDlgEZCopy;
int EZCopyWindowState;
extern int gDrawDouble;	//両方のトラックグループを描画する
extern int iDragMode;

int Bn[]={
	IDC_CTB1,
	IDC_CTB2,
	IDC_CTB3,
	IDC_CTB4,
	IDC_CTB5,
	IDC_CTB6,
	IDC_CTB7,
	IDC_CTB8,
	IDC_CTB9,
	IDC_CTB10,
	IDC_CTB11,
	IDC_CTB12,
};

int Hni[]={
	1,2,3,4,8,16,
	1,2,3,4,8,16,	
};

NOTECOPY nc_Select;
int tra=-256, ful=0,haba=0;
int sGrid = 0;	//範囲選択はグリッド単位で
int sACrnt = 0;	//範囲選択は常にｶﾚﾝﾄﾄﾗｯｸ

char CpHelp[512];
void ShowStatusMessage(void);


void PrintCpHelp(void)
{
	SetDlgItemText(hDlgEZCopy,IDC_COPYNOTION,CpHelp);

}
void SetEZCWindowMessage(char *Mess)
{
	strcpy(CpHelp, Mess );
	PrintCpHelp();
}

char TrackN[]="12345678QWERTYUI";

PARCHANGE pc1;

void ClearEZC_Message()
{
	CpHelp[0]=0;
	tra=-256;
	PrintCpHelp();
}

void EZ_Delete()
{
	if(tra<0){
		wsprintf(CpHelp,MessageString[IDS_STRING67]); //"範囲が指定されていない。"
		PrintCpHelp();
		return;
	}
	//int anss;
	//anss = MessageBox(hWnd,"消去範囲は正しいですか？","確認",MB_OKCANCEL|MB_ICONEXCLAMATION);
	//if(anss!=IDOK)return;
	long scr_h,scr_v,lash;
	scr_data.GetScrollPosition(&scr_h,&scr_v);
	RECT rect = {64,0,WWidth,WHeight};//更新する領域(トラック変更)
	//char str[5];
	char mss[255],mks[512];
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	//GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str,4);//範囲from
	nc_Select.x2 = scr_h* mi.dot * mi.line; //ここにコピー
	nc_Select.num = 1;
	int t;
	if(ful==1){
		int j1,j2;
		if(sACrnt==0){
			j1=0; j2=MAXTRACK;
		}else{
			if(org_data.track<MAXMELODY){
				j1=0; j2=MAXMELODY;	
			}else{
				j1=MAXMELODY; j2=MAXTRACK;
			}
		}
		for(t=j1;t<j2;t++){
			pc1.track=t;
			pc1.x1=nc_Select.x1_1;
			pc1.x2=nc_Select.x1_2;
			org_data.DelateNoteData(&pc1);
			org_data.CheckNoteTail(t);
		}
	}else{
		pc1.track=tra;
		pc1.x1=nc_Select.x1_1;
		pc1.x2=nc_Select.x1_2;
		org_data.DelateNoteData(&pc1);
		org_data.CheckNoteTail(tra);
		
	}
	//MessageBox(NULL,"コピーしました","通知",MB_OK);
	org_data.PutMusic();
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	lash = scr_h + haba;
	wsprintf(mss,MessageString[IDS_STRING68]);//"　消去しました。"


	strcpy(mks,CpHelp);
	strcat(mks,mss);
	SetDlgItemText(hDlgEZCopy,IDC_COPYNOTION,mks);

}

void EZ_DeleteAndTrim()
{
	NOTECOPY  delNC, tmpNC;
	delNC.num = 1;
	long scr_h,scr_v,lash;
	char mss[255],mks[512];
	MUSICINFO mi;
	int tmptra, tmpful, tmpsACrnt;
	
	scr_data.GetScrollPosition(&scr_h,&scr_v);
	RECT rect = {64,0,WWidth,WHeight};//更新する領域(トラック変更)
	org_data.GetMusicInfo(&mi);
	//GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str,4);//範囲from
	delNC.x2 = scr_h* mi.dot * mi.line; //ここにコピー
	if(tra<0){
		wsprintf(CpHelp,MessageString[IDS_STRING69]); //"範囲が指定されていないので 画面左端から 1ドットを"
		//wsprintf(CpHelp,"範囲が指定されていない。");
		//PrintCpHelp();
		//return;
		tmptra = org_data.track;
		tmpful = 1;
		tmpsACrnt = sACrnt;
		delNC.x1_1 = scr_h* mi.dot * mi.line;
		//delNC.x1_2 = delNC.x1_1 + mi.dot - 1;	// 2014.05.31 D
		delNC.x1_2 = delNC.x1_1 ;	// 2014.05.31 A
		
	}else{
		delNC.x1_1 = nc_Select.x1_1;
		delNC.x1_2 = nc_Select.x1_2;
		tmptra = tra;
		tmpful = ful;
		tmpsACrnt = sACrnt;
	}
	tmpNC.x1_1 = delNC.x1_2 + 1;
	tmpNC.x1_2 = 512 * mi.dot * mi.line;
	tmpNC.x2   = delNC.x1_1;
	tmpNC.num = 1;
	//int anss;
	//anss = MessageBox(hWnd,"消去範囲は正しいですか？","確認",MB_OKCANCEL|MB_ICONEXCLAMATION);
	//if(anss!=IDOK)return;
	//char str[5];
	int t;
	if(tmpful==1){
		int j1,j2;
		if(tmpsACrnt==0){
			j1=0; j2=MAXTRACK;
		}else{
			if(org_data.track<MAXMELODY){
				j1=0; j2=MAXMELODY;	
			}else{
				j1=MAXMELODY; j2=MAXTRACK;
			}
		}
		for(t=j1;t<j2;t++){
			pc1.track=t;
			pc1.x1=delNC.x1_1;
			pc1.x2=delNC.x1_2;
			org_data.DelateNoteData(&pc1);
			org_data.CheckNoteTail(t);
			tmpNC.track1=t;
			tmpNC.track2=t;
			org_data.CopyNoteData(&tmpNC);
			org_data.CheckNoteTail(t);
		}
	}else{
		pc1.track=tmptra;
		pc1.x1=delNC.x1_1;
		pc1.x2=delNC.x1_2;
		org_data.DelateNoteData(&pc1);
		org_data.CheckNoteTail(tmptra);
		tmpNC.track1 = tmptra;
		tmpNC.track2 = tmptra;
		org_data.CopyNoteData(&tmpNC);
		org_data.CheckNoteTail(tmptra);
		
	}

	//MessageBox(NULL,"コピーしました","通知",MB_OK);
	org_data.PutMusic();
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	lash = scr_h + haba;
	wsprintf(mss,MessageString[IDS_STRING70]); //"　消去し、詰めました。"

	strcpy(mks,CpHelp);
	strcat(mks,mss);
	SetDlgItemText(hDlgEZCopy,IDC_COPYNOTION,mks);

}


void EZ_Insert()
{
	NOTECOPY  delNC, tmpNC;
	delNC.num = 1;
	long scr_h,scr_v,lash;
	char mss[255],mks[512];
	int tmptra, tmpful, tmpsACrnt;
	MUSICINFO mi;
	scr_data.GetScrollPosition(&scr_h,&scr_v);
	RECT rect = {64,0,WWidth,WHeight};//更新する領域(トラック変更)
	org_data.GetMusicInfo(&mi);
	//GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str,4);//範囲from
	delNC.x2 = scr_h* mi.dot * mi.line; //ここにコピー
	if(tra<0){
		wsprintf(CpHelp,MessageString[IDS_STRING69]); //"範囲が指定されていないので 画面左端から 1ドットを"
		//wsprintf(CpHelp,"範囲が指定されていない。");
		//PrintCpHelp();
		//return;
		tmptra = org_data.track;
		tmpful = 1;
		tmpsACrnt = sACrnt;
		delNC.x1_1 = scr_h* mi.dot * mi.line;
		//delNC.x1_2 = delNC.x1_1 + mi.dot - 1;	// 2014.05.31 D
		delNC.x1_2 = delNC.x1_1 ;	// 2014.05.31 A
	}else{
		delNC.x1_1 = nc_Select.x1_1;
		delNC.x1_2 = nc_Select.x1_2;
		tmptra = tra;
		tmpful = ful;
		tmpsACrnt = sACrnt;
	}
	tmpNC.x1_1 = delNC.x1_1;
	tmpNC.x1_2 = 512 * mi.dot * mi.line;
	tmpNC.x2   = delNC.x1_2 + 1;
	tmpNC.num = 1;
	//int anss;
	//anss = MessageBox(hWnd,"消去範囲は正しいですか？","確認",MB_OKCANCEL|MB_ICONEXCLAMATION);
	//if(anss!=IDOK)return;
	//char str[5];
	int t;
	if(tmpful==1){
		int j1,j2;
		if(tmpsACrnt==0){
			j1=0; j2=MAXTRACK;
		}else{
			if(org_data.track<MAXMELODY){
				j1=0; j2=MAXMELODY;	
			}else{
				j1=MAXMELODY; j2=MAXTRACK;
			}
		}
		for(t=j1;t<j2;t++){
			pc1.track=t;
			pc1.x1=delNC.x1_1;
			pc1.x2=delNC.x1_2;
			tmpNC.track1=t;
			tmpNC.track2=t;
			org_data.CopyNoteData(&tmpNC);
			org_data.CheckNoteTail(t);
			org_data.DelateNoteData(&pc1);
			org_data.CheckNoteTail(t);
		}
	}else{
		pc1.track=tmptra;
		pc1.x1=delNC.x1_1;
		pc1.x2=delNC.x1_2;
		tmpNC.track1 = tmptra;
		tmpNC.track2 = tmptra;
		org_data.CopyNoteData(&tmpNC);
		org_data.CheckNoteTail(tmptra);
		org_data.DelateNoteData(&pc1);
		org_data.CheckNoteTail(tmptra);
		
	}

	//MessageBox(NULL,"コピーしました","通知",MB_OK);
	org_data.PutMusic();
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	lash = scr_h + haba;
	wsprintf(mss,MessageString[IDS_STRING71]);//"　後ろにずらしました。"

	strcpy(mks,CpHelp);
	strcat(mks,mss);
	SetDlgItemText(hDlgEZCopy,IDC_COPYNOTION,mks);

}





void EZ_Paste(int iNum)
{
	if(tra<0){
		wsprintf(CpHelp,MessageString[IDS_STRING67]); //範囲が指定されていない
		PrintCpHelp();
		return;
	}
	long scr_h,scr_v,lash;
	scr_data.GetScrollPosition(&scr_h,&scr_v);
	RECT rect = {64,0,WWidth,WHeight};//更新する領域(トラック変更)
	//char str[5];
	char mss[255],mks[512];
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	//GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str,4);//範囲from
	nc_Select.x2 = scr_h* mi.dot * mi.line; //ここにコピー
	nc_Select.num = iNum;
	int t;
	if(ful==1){
		int j1,j2;
		if(sACrnt==0){
			j1=0; j2=MAXTRACK;
		}else{
			if(org_data.track<MAXMELODY){
				j1=0; j2=MAXMELODY;	
			}else{
				j1=MAXMELODY; j2=MAXTRACK;
			}
		}
		for(t=j1;t<j2;t++){
			nc_Select.track1=t;
			nc_Select.track2=t;
			org_data.CopyNoteData(&nc_Select);
			org_data.CheckNoteTail(t);
		}
	}else{
		nc_Select.track1 = tra;
		nc_Select.track2 = org_data.track;
		org_data.CopyNoteData(&nc_Select);
		org_data.CheckNoteTail(tra);
		
	}
	//MessageBox(NULL,"コピーしました","通知",MB_OK);
	org_data.PutMusic();
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	lash = scr_h + haba;
	//if(ful==1)wsprintf(mss,"　%d小節～%d小節に%d回コピーしました。",scr_h,lash,nc_Select.num);	// 2014.10.19 D
	if(ful==1)wsprintf(mss,MessageString[IDS_STRING72],scr_h,lash,nc_Select.num);	// 2014.10.19 A
	//else wsprintf(mss,"　トラック%c の%d小節～%d小節に%d回コピーしました。",TrackN[nc_Select.track2],scr_h,lash,nc_Select.num);	// 2014.10.19 D
	else wsprintf(mss,MessageString[IDS_STRING73],TrackN[nc_Select.track2],scr_h,lash,nc_Select.num);	// 2014.10.19 A

	strcpy(mks,CpHelp);
	strcat(mks,mss);
	SetDlgItemText(hDlgEZCopy,IDC_COPYNOTION,mks);

}


BOOL CALLBACK DialogEZCopy(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect = {64,0,WWidth,WHeight};//更新する領域(トラック変更)
	int i,bt;
	long scr_h,scr_v;
	MUSICINFO mi;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		PrintCpHelp();
		return 1;
	case WM_CLOSE:
		ShowWindow(hdwnd, SW_HIDE);
		EZCopyWindowState = 0;
		return 1;
	case WM_RBUTTONDOWN:
		ClearEZC_Message();
		org_data.PutMusic();
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		break;
	case WM_MOUSEMOVE:
		if(wParam & MK_LBUTTON){
			ReleaseCapture();
			SendMessage( hdwnd, WM_SYSCOMMAND, SC_MOVE | 2, 0 );

		}
		break;
	case WM_COMMAND:
		if(timer_sw==0){
			bt=-1;
			for(i=0;i<12;i++){
				if(LOWORD(wParam) == Bn[i])bt=i;
			}
			if(bt>=0){ //＋１とかのボタンが押された。
				int a,b,d,bb;
				//char str[5];
				org_data.GetMusicInfo(&mi);
				scr_data.GetScrollPosition(&scr_h,&scr_v);
				//GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str,4);//範囲from
				//b = atol(str);
				b = scr_h;
				bb = b + Hni[bt];
				haba = Hni[bt];
				a = b * mi.dot * mi.line; //開始点
				d = bb* mi.dot * mi.line - 1; //終了点
				nc_Select.x1_1 = a;
				nc_Select.x1_2 = d;
				if(bt>=6)ful=1;else ful=0; //全トラック？
				tra = org_data.track;
				//org_data.CopyNoteDataToCB(&nc_Select, tra, ful);
				//if(ful==0)wsprintf(CpHelp,"トラック%c の%d小節～%d小節までを",TrackN[tra],b,bb);	// 2014.10.19 D
				if(ful==0)wsprintf(CpHelp,MessageString[IDS_STRING74],TrackN[tra],b,bb);	// 2014.10.19 A
				//else wsprintf(CpHelp,"全トラックの%d小節～%d小節までを",b,bb);	// 2014.10.19 D
				else wsprintf(CpHelp,MessageString[IDS_STRING75],b,bb);	// 2014.10.19 A
				PrintCpHelp();
				org_data.PutMusic();
				RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			}
			switch(LOWORD(wParam)){
			case IDC_PST1:
				SetUndo();
				EZ_Paste(1);
				break;
			case IDC_PST2:
				SetUndo();
				EZ_Paste(2);
				break;
			case IDC_PST3:
				SetUndo();
				EZ_Paste(3);
				break;
			case IDC_PST4:
				SetUndo();
				EZ_Paste(4);
				break;
			case IDC_DELETEBUTTON:
				SetUndo();
				EZ_Delete();
				break;
			case IDC_INSERTBUTTON:
				SetUndo();
				EZ_Insert();
				break;
			case IDC_DELETEBUTTON_2:
				SetUndo();
				EZ_DeleteAndTrim();
				break;
			case IDC_BTNCLOSE:
				ShowWindow(hdwnd, SW_HIDE);
				EZCopyWindowState = 0;
				return 1;

				break;
			}
		}
		ShowStatusMessage();
		SetFocus(hWnd);
		return 1;
	}
	return 0;
}

void ShowStatusMessage(void)
{
	char Mess[16];
	if(sACrnt)strcpy(Mess,"Current");
	else strcpy(Mess,"");
	SetDlgItemText(hDlgEZCopy, IDC_STATUS1, Mess);

	if(sGrid)strcpy(Mess,"Grid");
	else strcpy(Mess,"");
	SetDlgItemText(hDlgEZCopy, IDC_STATUS2, Mess);

	if(iDragMode)strcpy(Mess,"Drag");
	else strcpy(Mess,"");
	SetDlgItemText(hDlgEZCopy, IDC_STATUS3, Mess);


	//SetDlgItemText(hDlgEZCopy, IDC_STATUS, Mess);

}

