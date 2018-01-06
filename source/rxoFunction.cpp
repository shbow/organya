//#include <wchar.h> charをwchar_tに直すのは根気が要る。
#include "Setting.h"
#include "DefOrg.h"
#include "resource.h"
#include "Gdi.h"
#include "OrgData.h"
#include "Scroll.h"
#include "Mouse.h"
#include "Click.h"
#include <stdio.h>
#include "Filer.h"
#include <string.h>

#include "Sound.h"
#include "Timer.h"
#define GET_MEAS1	1
#define GET_MEAS2	3
#define GET_BEAT1	2
#define GET_BEAT2	4

//#define VIRTUAL_CB_SIZE 640000	// 2014.10.19 D
#define VIRTUAL_CB_SIZE 64	// 2014.10.19 A

#define MESSAGE_STRING_BUFFER_SIZE (1024*1024)
#define MESSAGE_STRING_MAX 1024

/* //↓この一行を追加する★

#include "rxoFunction.h"

  */
/* //EditNoteでのトラック指定
	-1 : 0 ~ 8
	-2 : 8 ~ 16
	-3 : 0 ~ 16 
	-4 : CurrentTrack
  */
MEMORYSTATUS rMem ;
//GlobalMemoryStatus( &Mem ) ;

extern void SetTitlebarChange(void);
extern void ResetTitlebarChange(void);

extern HWND hDlgTrack;
extern int mute_name[MAXTRACK];
extern char timer_sw;
extern NOTECOPY nc_Select; //選択範囲
extern int tra, ful ,haba; 
extern int sGrid;	//範囲選択はグリッド単位で
extern int sACrnt;	//範囲選択は常にｶﾚﾝﾄﾄﾗｯｸ
extern int gDrawDouble;	//両方のトラックグループを描画する
extern CHAR app_path[];
extern int iDragMode;
extern SaveWithInitVolFile;

TCHAR *MessageStringBuffer = NULL;	// 2014.10.19 A
TCHAR *MessageString[MESSAGE_STRING_MAX];

int msgbox(HWND hWnd , int MessageID, int TitleID, UINT uType);

int iChangeEnablePlaying; //2010.09.23 A
int iChangeFinish; //2010.09.23 A

int iActivatePAN = 0; //2014.05.01 A
int iActivateVOL = 0; //2014.05.01 A

int iSlideOverlapNotes = 0; //重なり合う音符の表示を若干ずらす 2014.05.06 A

int Menu_Recent[]={
	IDM_RECENT1, IDM_RECENT2, IDM_RECENT3, IDM_RECENT4, IDM_RECENT5, IDM_RECENT6, IDM_RECENT7, IDM_RECENT8, IDM_RECENT9, IDM_RECENT0
};
char *FileAcc[]={
	"File1", "File2", "File3", "File4", "File5", "File6", "File7", "File8", "File9", "File0"
};

char RecentFileName[10][MAX_PATH];	//最近使ったファイル名
void SetMenuRecent(int iMenuNumber, char *strText, int iDisable);
void CreateMenuRecent();

void ShowStatusMessage(void);

int iRecentTrackM[]={ // 2010.09.23 A 最近使ったトラック番号
	0,1,2,3,4,5,6,7
};
int iRecentTrackD[]={ // 2010.09.23 A 最近使ったトラック番号
	8,9,10,11,12,13,14,15
};

int NoteWidth; //音符の幅
int NoteEnlarge_Until_16px; //表示の際、NOTEの頭を最大16ピクセルまで伸ばす。
int iPushStratch = 0; //2014.05.31
int iLastEditNoteLength = 1;

void setRecentTrack(int iNewTrack){ //最近使ったトラック番号を更新する
	int iRT[MAXMELODY];
	int i,j;
	if(iNewTrack<MAXMELODY){
		for(i=0;i<MAXMELODY;i++){
			iRT[i]=iRecentTrackM[i];
		}
		iRecentTrackM[0]=iNewTrack;
		j=1;
		for(i=0;i<MAXMELODY;i++){
			if(iRT[i]!=iNewTrack){
				iRecentTrackM[j] = iRT[i];
				j++;
			}
		}
	}else{
		for(i=0;i<MAXMELODY;i++){
			iRT[i]=iRecentTrackD[i];
		}
		iRecentTrackD[0]=iNewTrack;
		j=1;
		for(i=0;i<MAXMELODY;i++){
			if(iRT[i]!=iNewTrack){
				iRecentTrackD[j] = iRT[i];
				j++;
			}
		}
	}
	return;
}

//iOrder:0～7
//isDrumTrack:0 メロディ  ,   isDrumTrack:1 ドラム
int getRecentTrack(int iOrder, int isDrumTrack){ //最近使ったトラックを返す
	int i,j;
	j=0;
 	if(isDrumTrack==0){
		for(i=0;i<MAXMELODY;i++){
			if(7-iOrder==j)return iRecentTrackM[i];
			j++;
		}
	}else{
		for(i=0;i<MAXMELODY;i++){
			if(7-iOrder==j)return iRecentTrackD[i];
			j++;
		}
	}
	return iOrder; //本来、この値で返されることはないはず...
}

//最近使ったファイル群にプット
void PutRecentFile(char *FileName)
{
	int i,j;
	j=9;
	for(i=0;i<10;i++){
		if(strcmp(RecentFileName[i],FileName)==0){ //等しいとき
			j=i; i=999;
		}
	}

	for(i=j;i>=1;i--){
		strcpy(RecentFileName[i],RecentFileName[i-1]);
	}
	strcpy(RecentFileName[0],FileName);
	CreateMenuRecent();
}


void LoadRecentFromIniFile(){
	int i;
	for(i=0;i<10;i++){
		RecentFileName[i][0]='@';
		RecentFileName[i][1]='\0';
		GetPrivateProfileString( "Recent",FileAcc[i],"@",RecentFileName[i],256,app_path);
	}
	CreateMenuRecent();
}

void SetMenuRecent(int iMenuNumber, char *strText, int iDisable)
{

	if(iMenuNumber<0 || iMenuNumber>9)return;
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	char strCc[256];
	strcpy(strCc,"&&");
	itoa((iMenuNumber+1)%10, &strCc[1], 10);
	strCc[2]='\0';
	strcat(strCc," ");
	//strcat(strCc,strText);
	int y,i;
	y = strlen(strText);
	for(i=y;i>0;i--)if(strText[i]=='\\'){i++;break;}
	strcat(strCc,&strText[i]);
	if(iMenuNumber==0){
		strcat(strCc,"\tShift+Ctrl+Home");
	}
	ModifyMenu(hMenu, Menu_Recent[iMenuNumber], MF_BYCOMMAND|MF_STRING, Menu_Recent[iMenuNumber], strCc);
	if(iDisable){
		EnableMenuItem(hMenu,Menu_Recent[iMenuNumber],MF_BYCOMMAND|MF_GRAYED);
	}else{
		EnableMenuItem(hMenu,Menu_Recent[iMenuNumber],MF_BYCOMMAND|MF_ENABLED);
	}
}

void ClearRecentFile()
{
	int a;
	//a = MessageBox(hWnd,"履歴を抹ッ消しますか？","「最近使ったファイル」のクリア",MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);	// 2014.10.19 D
	a = msgbox(hWnd,IDS_NOTIFY_RECENT_INITIALIZE,IDS_CLEAR_RECENT,MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);	// 2014.10.19 A
	if(a == IDOK){
		int i;
		for(i=0;i<10;i++){
			RecentFileName[i][0]='@';
			RecentFileName[i][1]='\0';
		}
		CreateMenuRecent();
		//MessageBox(hWnd,"真っ白になったぜ。","通知",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_INFO_INITIALIZE,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A

	}else{
		//MessageBox(hWnd,"キャンセルしました。","通知",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_CANCEL,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A
	}

	
}

void CreateMenuRecent()
{
	int i;
	for(i=0;i<10;i++){
		if(RecentFileName[i][0]!='@'){
			SetMenuRecent(i,RecentFileName[i],0);
		}else{
			//SetMenuRecent(i,"未使用",1);	// 2014.10.19 D
			SetMenuRecent(i,MessageString[IDS_STRING76],1);	// 2014.10.19 A
		}
	}

}

void SaveRecentFilesToInifile()
{
	int i;
	for(i=0;i<10;i++){
		WritePrivateProfileString("Recent",FileAcc[i],RecentFileName[i],app_path);
	}
}

void SetLoadRecentFile(int iNum)
{
	if(iNum<0 || iNum>9)return;
	strcpy(music_file, RecentFileName[iNum]);

}

int GetSelectMeasBeat(int GetToValue, int addValue)
{
	if (tra<0)return 0;
	int r,g;	//line 何泊か  // dot １泊の分割数
	unsigned char line,dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot; line = mi.line;
	//r = nc_Select.x1_1;
	g = dot*line; r = 0;
	switch(GetToValue){
	case GET_MEAS1:
		r = nc_Select.x1_1/g;
		break;
	case GET_MEAS2:
		r = (nc_Select.x1_2+addValue)/g;
		break;
	case GET_BEAT1:
		r = nc_Select.x1_1%g;
		break;
	case GET_BEAT2:
		r = (nc_Select.x1_2+addValue)%g;
		break;
	}
	return r;
}
char *TrackCode[]={"1","2","3","4","5","6","7","8","Q","W","E","R","T","Y","U","I" };

//大文字小文字に注意した↑の逆函数
int ReverseTrackCode(char *strTrack)
{
	int i;
	i=-1;
	do{
		i++;
		switch(strTrack[i]){
		case '1':
			return 0;
		case '2':
			return 1;
		case '3':
			return 2;
		case '4':
			return 3;
		case '5':
			return 4;
		case '6':
			return 5;
		case '7':
			return 6;
		case '8':
			return 7;
		case 'q':
		case 'Q':
			return 8;
		case 'w':
		case 'W':
			return 9;
		case 'e':
		case 'E':
			return 10;
		case 'r':
		case 'R':
			return 11;
		case 't':
		case 'T':
			return 12;
		case 'y':
		case 'Y':
			return 13;
		case 'u':
		case 'U':
			return 14;
		case 'i':
		case 'I':
			return 15;
		}
	}while(strTrack[i]==' '); //先頭の空白は無視
	return 99; //異常なトラック
}

void MuteTrack(int Track)
{
	SendDlgItemMessage(hDlgTrack , mute_name[Track] , BM_CLICK , 0, 0);
	
}

void EditNote(int AddNotes , int Track , int Function)
{
	if(timer_sw!=0)return;
	if(AddNotes==0)return;
	int j,jmin,jmax,Trc;
	Trc = Track;
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	PARCHANGE pc;
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	pc.x1 = 0  * mi.dot * mi.line;
	pc.x2 = mi.end_x * mi.dot * mi.line - 1;	
	if(AddNotes<0){
		pc.mode = MODEPARSUB;
		pc.a = -AddNotes;
	}
	else{
		pc.mode = MODEPARADD;
		pc.a = AddNotes;
	}

	if(tra>=0){ //選択範囲があれば。
		if(ful == 1 || tra == org_data.track){
			pc.x1 = nc_Select.x1_1;
			pc.x2 = nc_Select.x1_2;
		}
		if(ful == 1 && sACrnt>0){
			if(org_data.track<MAXMELODY){
				Trc=-1;
			}else{
				Trc=-2;
			}
		}
	}

	if(Trc<0){
		jmin = 0;
		jmax = MAXMELODY;
		if(Trc==-2){
			jmin = MAXMELODY;
			jmax = MAXTRACK;
		}else if(Trc==-3){
			jmax = MAXTRACK;
		}else if(Trc==-4){ //カレントトラック
			jmin = (int)org_data.track;
			jmax = jmin + 1;
		}
	}else{
		jmin = Track;
		jmax = Track+1;
	}
	for(j=jmin;j<jmax;j++){
		pc.track = j;
		if(Function==0)org_data.ChangeTransData(&pc);
		else if(Function==1)org_data.ChangeVolumeData(&pc);
		else if(Function==2)org_data.ChangePanData(&pc);
		else if(Function==10){
			pc.mode = MODEMULTIPLY;
			org_data.ChangeVolumeData(&pc);
		}else if(Function>=MODEDECAY && Function<MODEDECAY+20){
			pc.mode = (unsigned char)Function;
			org_data.ChangeVolumeData(&pc);
		}else if(Function>=MODEDECAY+20){
			org_data.EnsureEmptyArea(&pc, Function - MODEDECAY - 20);
		}
	}
	org_data.PutMusic();	//表示
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	//MessageBox(hdwnd,"指定範囲のキーを変更しました","通知",MB_OK);
	return;
}

//音の高さを上げる Track=-1ですべて(ドラムはのぞく)
void TransportNote(int AddNotes , int Track )
{
	EditNote(AddNotes , Track , 0);
}

void VolumeEdit(int AddNotes , int Track )
{
	EditNote(AddNotes , Track , 1);
}
void VolumeWariaiEdit(int AddNotes , int Track )  //2014.04.30 A
{
	EditNote(AddNotes , Track , 10);
}

void VolumeDecayEdit(int AddNotes , int Track , int Function )  //2014.05.01 A
{
	EditNote(AddNotes , Track , MODEDECAY + Function);
}

void PanEdit(int AddNotes , int Track )
{
	EditNote(AddNotes , Track , 2);
}

void ShowMemoryState(){ //デバッグ用
	char cc[32]; int y;
	GlobalMemoryStatus( &rMem ) ;
	y=rMem.dwAvailPhys/1000;
	_itoa(y,cc,10);
	MessageBox(NULL,cc,"Mem",MB_OK);
}

//音符の整理
void SortMusicNote(void)
{
	int a;
	//a = MessageBox(hWnd,"長時間の使用により、ノート（音符）がメモリ上に\n散乱してしまいます。（譜面順とメモリ順は異なっている）\nこの関数はノートを譜面の順番に\n並べ換えます。\n尚、データをロードし直しても同じ効果が得られます。\n実行しますか？","使い方と目的",MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);	// 2014.10.19 D
	a = msgbox(hWnd,IDS_INFO_MEMORY,IDS_USAGE,MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);	// 2014.10.19 A
	if(a == IDOK){
		org_data.SortNotes();
		//MessageBox(hWnd,"並べ替え・再構築しました。","通知",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_INFO_NARABEKAE,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A

	}else{
		//MessageBox(hWnd,"キャンセルしました。","通知",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_CANCEL,IDS_NOTIFY_TITLE,MB_OK);	// 2014.10.19 A
	}
}

//仮想的にクリップボードを用意して、データのやり取りはこいつを介して行う。
char VirtualCB[VIRTUAL_CB_SIZE];
char *readVCB;

void ClearVirtualCB(void)
{
	VirtualCB[0]=0;
	readVCB = VirtualCB;
}

void AddIntegerToVirtualCB(int iNum)
{
	char ins[32];
	itoa(iNum,ins,10);
	strcat(VirtualCB,ins);
	strcat(VirtualCB,"|");
}

void AddTrackSeparater(void)
{
	strcat(VirtualCB,"@");
}

//セパレータを発見したらtrue
bool ReadTrackSeparater(void)
{
	if(*readVCB!='@')return false;
	readVCB++;
	return true;
}
void AddStartToVirtualCB(void)
{
	strcpy(VirtualCB,"OrgCBData|");
}

//読み出し開始させると同時に、正規のデータかチェック
bool ReadStartFromVirtualCB(void)
{
	readVCB = &VirtualCB[10]; //先頭の形式？
	if(VirtualCB[0]=='O' && VirtualCB[1]=='r' && VirtualCB[2]=='g' && 
		VirtualCB[3]=='C' && VirtualCB[4]=='B' && VirtualCB[5]=='D' && 
		VirtualCB[6]=='a' && VirtualCB[7]=='t' && VirtualCB[8]=='a')return true;
	VirtualCB[10]='\0'; //データ破棄
	return false;

}

int ReadIntegerFromVirtualCB(void)
{
	if(*readVCB=='\0')return -9999;
	char ons[32], *cp;
	cp = ons;
	do{
		*cp = *readVCB;
		readVCB++;
		cp++;
	}while(*readVCB!='|');
	readVCB++;
	*cp=0;
	int i;
	i = atoi(ons);
	return i;
}

//本物のCBにコピー
void SetClipBoardFromVCB(void)
{
	//MessageBox(NULL,VirtualCB,"Error(Copy)",MB_OK);
	HGLOBAL hText;
	char *pText;
	hText = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, 640000);
	pText = (char*)GlobalLock(hText);
	lstrcpy(pText, VirtualCB);
	GlobalUnlock(hText);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hText);
	CloseClipboard();

	ClearVirtualCB();

}

//VCBへ代入
void GetClipBoardToVCB(void)
{
	HANDLE hText;
	char *pText;

	OpenClipboard(NULL);

	hText = GetClipboardData(CF_TEXT);
	if(hText == NULL) {
		//printf("クリップボードにテキストデータはない。\n");
	} else {
		pText = (char*)GlobalLock(hText);
		int i;
		for(i=0;i<640000;i++){
			VirtualCB[i]=pText[i];
			if(pText[i]=='\0')i=640000+1; //強引にループ終了
		}

		GlobalUnlock(hText);
	}

	CloseClipboard();
	ReadStartFromVirtualCB();
}

RECT rect1 = {0,0,WWidth,WHeight};//更新する領域

void ReplaseUndo()
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(org_data.ReplaceFromUndoData()>0){ //これ以上UNDO出来ない
		EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_GRAYED);
	}
	org_data.PutBackGround();
	org_data.PutMusic();	//表示
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	//通常の状態に戻すには
	EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_ENABLED);
	DrawMenuBar(hWnd);//メニューを再描画
	if(org_data.MinimumUndoCursor==0 && org_data.CurrentUndoCursor==0){
		ResetTitlebarChange();
	}else{
		SetTitlebarChange();
	}
}

void SetUndo()
{
	if(org_data.SetUndoData()>0){ //セットし、もし、メニューが灰色表示なら濃くする
		HMENU hMenu;
		hMenu=GetMenu(hWnd);
		//通常の状態に戻すには
		EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_GRAYED);
		DrawMenuBar(hWnd);//メニューを再描画
	}
	SetTitlebarChange();
}

void ResetLastUndo() //取りけし
{
	if(org_data.ResetLastUndo()>0){ //セットし、もし、メニューが灰色表示なら濃くする
		HMENU hMenu;
		hMenu=GetMenu(hWnd);
		//通常の状態に戻すには
		EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_GRAYED);
		DrawMenuBar(hWnd);//メニューを再描画
	}

}

void ClearUndo()
{
	org_data.ClearUndoData();
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_GRAYED);
	EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_GRAYED);
	//通常の状態に戻すには
	//EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_ENABLED);
	DrawMenuBar(hWnd);//メニューを再描画

}

void ReplaceRedo()
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(org_data.ReplaceFromRedoData()>0){
		//通常の状態に戻すには
		EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_GRAYED);
	}
	EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_ENABLED);
	DrawMenuBar(hWnd);//メニューを再描画
	org_data.PutBackGround();
	org_data.PutMusic();	//表示
	//RedrawWindow(hWnd,&rect1,NULL,RDW_INVALIDATE|RDW_ERASENOW);

}

void ChangeGridMode(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)sGrid=iValue;
	else{
		sGrid = 1 - sGrid;
	}
	if(sGrid==0)
		CheckMenuItem(hMenu,IDM_GRIDMODE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_GRIDMODE,(MF_BYCOMMAND|MFS_CHECKED));
	ShowStatusMessage();
}

//2010.09.23 A
void ChangeEnablePlaying(int iValue){
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iChangeEnablePlaying=iValue;
	else{
		iChangeEnablePlaying = 1 - iChangeEnablePlaying;
	}
	if(iChangeEnablePlaying==0)
		CheckMenuItem(hMenu,IDM_ENABLEPLAYING,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_ENABLEPLAYING,(MF_BYCOMMAND|MFS_CHECKED));
	ShowStatusMessage();
}
//2010.09.23 A
void ChangeFinish(int iValue){
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iChangeFinish=iValue;
	else{
		iChangeFinish = 1 - iChangeFinish;
	}
	if(iChangeFinish==0)
		CheckMenuItem(hMenu,IDM_CHANGEFINISH,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_CHANGEFINISH,(MF_BYCOMMAND|MFS_CHECKED));
	ShowStatusMessage();
}

//2010.09.23 A
void ChangeNoteEnlarge(int iValue){
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)NoteEnlarge_Until_16px = iValue;
	else{
		NoteEnlarge_Until_16px = 1 - NoteEnlarge_Until_16px;
	}
	if(NoteEnlarge_Until_16px == 0)
		CheckMenuItem(hMenu,IDM_NOTE_ENLARGE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_NOTE_ENLARGE,(MF_BYCOMMAND|MFS_CHECKED));
	//ShowStatusMessage();
}

int MinimumGrid(int x)
{
	int r;	//line 何泊か  // dot １泊の分割数
	unsigned char dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot;
	r = x - (x % dot);
	return r;

}
int MaximumGrid(int x)
{
	int r;	//line 何泊か  // dot １泊の分割数
	unsigned char dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot;
	r = x - (x % dot) + dot-1;
	return r;
}

int MinimumGridLine(int x)
{
	int r;	//line 何泊か  // dot １泊の分割数
	unsigned char dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot * mi.line;
	r = x - (x % dot);
	return r;

}
int MaximumGridLine(int x)
{
	int r;	//line 何泊か  // dot １泊の分割数
	unsigned char dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot * mi.line;
	r = x - (x % dot) + dot-1;
	return r;
}

void ChangeSelAlwaysCurrent(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)sACrnt=iValue;
	else{
		sACrnt = 1 - sACrnt;
	}
	if(sACrnt==0)
		CheckMenuItem(hMenu,IDM_ALWAYS_CURRENT,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_ALWAYS_CURRENT,(MF_BYCOMMAND|MFS_CHECKED));

	ShowStatusMessage();

}

void ChangeDrawDouble(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)gDrawDouble=iValue;
	else{
		gDrawDouble = 1 - gDrawDouble;
	}
	if(gDrawDouble==0)
		CheckMenuItem(hMenu,IDM_DRAWDOUBLE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_DRAWDOUBLE,(MF_BYCOMMAND|MFS_CHECKED));
		//ModifyMenu(hMenu, IDM_DRAWDOUBLE, MF_BYCOMMAND|MF_STRING, IDM_DRAWDOUBLE, "おえ");
	org_data.PutMusic();

	ShowStatusMessage();
}

void ChangeDragMode(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iDragMode=iValue;
	else{
		iDragMode = 1 - iDragMode;
	}
	if(iDragMode==0)
		CheckMenuItem(hMenu,IDM_DRAGMODE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_DRAGMODE,(MF_BYCOMMAND|MFS_CHECKED));
	org_data.PutMusic();
	ShowStatusMessage();

}

void ChangeSlideOverlapNoteMode(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iSlideOverlapNotes=iValue;
	else{
		iSlideOverlapNotes = 1 - iSlideOverlapNotes;
	}
	if(iSlideOverlapNotes==0)
		CheckMenuItem(hMenu,IDM_SLIDEOVERLAPNOTES,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_SLIDEOVERLAPNOTES,(MF_BYCOMMAND|MFS_CHECKED));
	org_data.PutMusic();
	ShowStatusMessage();

}

void ChangePushStratchNOTE(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iPushStratch=iValue;
	else{
		iPushStratch = 1 - iPushStratch;
	}
	if(iPushStratch==0)
		CheckMenuItem(hMenu,IDM_PRESSNOTE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_PRESSNOTE,(MF_BYCOMMAND|MFS_CHECKED));
	org_data.PutMusic();
	ShowStatusMessage();

}

bool AutoLoadPVIFile()
{
	//2014.05.06 A
	//2014.05.30 M
	char *PVIFile;
	PVIFile = new char[MAX_PATH];
	strcpy(PVIFile, music_file);
	int ml = strlen(PVIFile);
	if(ml>4){
		if(PVIFile[ml-4] == '.'){
			strcpy(PVIFile+ml-4, ".pvi");
		}
	}else{
		delete [] PVIFile;
		return false;
	}
	FILE *fp;
	fp = fopen(PVIFile, "rt");
	if(fp==NULL){
		delete [] PVIFile;
		return false;
	}
	int t,r;
	for(t=0;t<MAXTRACK;t++){
		fscanf(fp,"%d",&r);
		org_data.def_pan[t] = (unsigned char)r;
		fscanf(fp,"%d",&r);
		org_data.def_volume[t] = (unsigned char)r;
	}
	fclose(fp);
	delete [] PVIFile;
	return true;
}

bool AutoSavePVIFile()
{
	//2014.05.06 A
	char *PVIFile;
	PVIFile = new char[MAX_PATH];
	strcpy(PVIFile, music_file);
	int ml = strlen(PVIFile);
	if(ml>4){
		if(PVIFile[ml-4] == '.'){
			strcpy(PVIFile+ml-4, ".pvi");
		}
	}else{
		delete [] PVIFile;
		return false;
	}
	FILE *fp;
	fp = fopen(PVIFile, "wt");
	if(fp==NULL){
		delete [] PVIFile;
		return false;
	}
	int t,r;
	for(t=0;t<MAXTRACK;t++){
		r = (int)org_data.def_pan[t];
		fprintf(fp,"%d\n",r);
		r = (int)org_data.def_volume[t];
		fprintf(fp,"%d\n",r);
	}
	fclose(fp);
	delete [] PVIFile;
	return true;
}

void ChangeAutoLoadMode(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)SaveWithInitVolFile=iValue;
	else{
		SaveWithInitVolFile = 1 - SaveWithInitVolFile;
	}
	if(SaveWithInitVolFile==0)
		CheckMenuItem(hMenu,IDM_AUTOLOADPVI,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_AUTOLOADPVI,(MF_BYCOMMAND|MFS_CHECKED));
	ShowStatusMessage();
}

// StringTableを参照するメッセージボックス //2014.10.18 
int msgbox(HWND hWnd , int MessageID, int TitleID, UINT uType)
{
	TCHAR strMesssage[2048];
	TCHAR strTitle[1024];
	LoadString(GetModuleHandle(NULL), MessageID, strMesssage, 2048);
	LoadString(GetModuleHandle(NULL), TitleID  , strTitle   , 1024);
	return MessageBox(hWnd, strMesssage, strTitle, uType);
}

void FreeMessageStringBuffer(void)
{
	free(MessageStringBuffer);
}

int AllocMessageStringBuffer(void)
{
	int i, r, flg;
	TCHAR *ptr, *p;
	if(MessageStringBuffer == NULL){
		MessageStringBuffer = (TCHAR *)calloc(MESSAGE_STRING_BUFFER_SIZE, sizeof(TCHAR));
	}
	for(i = 0; i < MESSAGE_STRING_MAX; i++){
		MessageString[i] = NULL;
	}
	ptr = MessageStringBuffer;
	for(i = 1; i < MESSAGE_STRING_MAX; i++){
		MessageString[i] = ptr;
		r = LoadString(GetModuleHandle(NULL), i, ptr, 1024); //1024は適当です。正確にはMESSAGE_STRING_BUFFER_SIZEから計算する必要があります。
		if(r > 0){
			//末尾の!!を検出する
			for(p = ptr + r - 3, flg = 0; *p != 0 ; p++){
				if(*p == '!')flg++; else flg = 0;
				if(flg >= 2)break;
			}
			//!!のとき、頭から!を\0に置換
			if(flg == 2){
				for(p = ptr ; *p != 0 ; p++)if(*p == '!')*p = 0;
			}
			//ポインタ移動
			ptr += (r + 1 + 1); //+ 1はNULLの分、もう+1は予備。
		}else{ //ｴﾗｰの場合(存在しないとか)
			break; //抜ける
		}
	}

	//lpstrFilterについては"!"を\0に変換する必要アリ。
	//for(ptr = MessageString[108]; *ptr != 0; ptr++)if(*ptr == '!')*ptr = 0;
	//for(ptr = MessageString[109]; *ptr != 0; ptr++)if(*ptr == '!')*ptr = 0;
	//for(ptr = MessageString[110]; *ptr != 0; ptr++)if(*ptr == '!')*ptr = 0;
	//for(ptr = MessageString[111]; *ptr != 0; ptr++)if(*ptr == '!')*ptr = 0;
	return 0;
}

