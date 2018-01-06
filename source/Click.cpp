#include "Setting.h"
#include "DefOrg.h"
#include "OrgData.h"
#include "Mouse.h"
#include "Scroll.h"
#include <stdio.h>//sprintfテスト用
#include <string.h>//sprintfテスト用
#include "rxoFunction.h"
#include "resource.h"

//クリックされた時の処理
extern char timer_sw;//再生スイッチ
extern void SetUndo();
extern void ResetLastUndo(); //取りけし

extern NOTECOPY nc_Select; //選択範囲
extern int tra, ful ,haba; 
extern void SetEZCWindowMessage(char *Mess);

long Last_mouse_x = -99999;	//同位置の複数クリックはアンドゥとして記録しない。
long Last_mouse_y = -99999;

long Last_VOL_Drag_mouse_x = -99999;	//同位置の複数クリックはアンドゥとして記録しない。
long Last_VOL_Drag_mouse_y = -99999;


long DragStartx = -99999;	//ドラッグ開始点
long StartNotex = -99999;	//そのノート

long PutStartX = -99999;	//音符のドラッグ開始点

extern char TrackN[];
extern int sGrid;	//範囲選択はグリッド単位で
extern int MinimumGrid(int x);
extern int MaximumGrid(int x);
void ShowStatusMessage(void);

extern void ChangeTrack(HWND hdwnd, int iTrack);

int iDragMode = 0;	//ﾄﾞﾗｯｸﾞで音符を伸ばす
int alt_down = 0;
int shift_down = -1;
long AdjustX = 0; //   縮小表示時、音符の頭を目立たせる(&O)ための補正


void ClearDrag()
{
	DragStartx = -99999;
	StartNotex = -99999;
}

void RedrawClick()
{
	RECT rect = {64,0,WWidth,WHeight};
	org_data.PutMusic();//楽譜の再描画
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

}

void SelectReset()
{
	tra=-256;
	DragStartx = -99999;
	RedrawClick();
	SetEZCWindowMessage("");
}

void PrintStatusMessage(int Zensentaku){
	char wk[256],wk2[256];
	if(Zensentaku != 0){
		if(ful==0){
			wsprintf(wk2,MessageString[IDS_STRING104], TrackN[tra]); //"トラック%c の"
			strcpy(wk,wk2);
			wsprintf(wk2,MessageString[IDS_STRING115]); // " はじめからおわりまでを"
			strcat(wk,wk2);
			SetEZCWindowMessage(wk);
		}
		else if(ful==1){
			wsprintf(wk2,MessageString[IDS_STRING116]); //"全トラックの はじめからおわりまでを"
			strcpy(wk,wk2);
			SetEZCWindowMessage(wk);
		}
	}else{
		if(ful==0){
			wsprintf(wk2,MessageString[IDS_STRING104], TrackN[tra]); //"トラック%c の"
			strcpy(wk,wk2);
			wsprintf(wk2,MessageString[IDS_STRING105]); // "選択されている範囲を"
			strcat(wk,wk2);
			SetEZCWindowMessage(wk);
		}
		else if(ful==1){
			wsprintf(wk2,MessageString[IDS_STRING106]); // "全トラックの選択されている範囲を"
			strcpy(wk,wk2);
			SetEZCWindowMessage(wk);
		}
	}

}

void SelectAll(int FullTrack)
{
	unsigned char line,dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot; line = mi.line;
	nc_Select.x1_1=0;
	nc_Select.x1_2=999*dot*line;
	tra = org_data.track;
	ful = FullTrack;
	PrintStatusMessage(1);
	RedrawClick();

}

void LButtonUP(WPARAM wParam, LPARAM lParam)
{
	RECT rect = {64,0,WWidth,WHeight};
	if(PutStartX != -99999){
		org_data.CheckNoteTail(org_data.track);
		PutStartX = -99999;
		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

	}
	if(DragStartx<=-99999)return;

	PrintStatusMessage(0);
	ClearDrag();
	org_data.PutMusic();//楽譜の再描画
	ShowStatusMessage();
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	Last_VOL_Drag_mouse_x = -99999; //2014.05.02 A
	Last_VOL_Drag_mouse_y = -99999;
	AdjustX = 0;
}
void RButtonUP(WPARAM wParam, LPARAM lParam)
{
	RECT rect = {64,0,WWidth,WHeight};
	Last_VOL_Drag_mouse_x = -99999; //2014.05.02 A
	Last_VOL_Drag_mouse_y = -99999;
	org_data.PutMusic();//楽譜の再描画
	ShowStatusMessage();
	RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	alt_down = 0; shift_down = -1;

}

void MouseDrag(WPARAM wParam, LPARAM lParam)
{
	RECT rect = {64,WHeight-16,WWidth,WHeight};
	RECT rect2 = {64,0,WWidth,WHeight};
	RECT rectPAN = {64, WHeight+292-WHNM - 3, WWidth, WHeight+(351+7)-WHNM + 3};	// 2010.08.14 
	RECT rectVOL = {64, WHeight+365-WHNM - 3, WWidth, WHeight+428-WHNM + 3};	// 2010.08.14 

	if(timer_sw)return;
	long mouse_x;
	long mouse_y;
	long ptx;
	long scr_h,scr_v;
	long Note_x;
	unsigned char line,dot;
	scr_data.GetScrollPosition(&scr_h,&scr_v);
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	dot = mi.dot;
	line = mi.line;
	//マウスの座標を取得
	mouse_data.GetMousePosition(&mouse_x,&mouse_y);
	ptx = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
	
	long Ry;	// 2010.08.14 
	Ry = -12*(Last_mouse_y - 95 + scr_v);	// 2010.08.14 
	RECT rectNOTE = {64, Ry, WWidth, Ry + 12};	//現在の音符付近のみ	// 2010.08.14 

	if(DragStartx<=-99999){
		if(wParam & MK_LBUTTON){

			//if(PutStartX>-99999+1){ //音符ドラッグ 2010.09.23 D
			if(PutStartX>-99999+1 && ( 95 - (mouse_y/12 + scr_v)!=Last_mouse_y || (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot != Last_mouse_x)){ //音符ドラッグ //2010.09.23 A
				if(ptx < PutStartX){	// 2010.08.14 C
					ptx = PutStartX;	// 2010.08.14 C
				}
				if(NoteWidth != 16){
					AdjustX = org_data.SearchNoteC(PutStartX, (unsigned char)(95 - (mouse_y/12 + scr_v)), 16 / NoteWidth, 16 % NoteWidth); //2014.05.28
					if(AdjustX > 0) ptx -= AdjustX;
				}else AdjustX = 0;
				if( 95 - (mouse_y/12 + scr_v)!=Last_mouse_y && (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot == Last_mouse_x ){ // 2010.09.23 A y軸のみの移動ではnoteの長さ変えない
					org_data.PutMusic();//楽譜の再描画
					RedrawWindow(hWnd,&rectNOTE,NULL,RDW_INVALIDATE|RDW_ERASENOW);
				}else{
					if(org_data.SetNote_onlyLength(PutStartX, ptx - PutStartX  + 1) == TRUE){ // 2010.08.14 C
						org_data.PutMusic();//楽譜の再描画
						RedrawWindow(hWnd,&rectNOTE,NULL,RDW_INVALIDATE|RDW_ERASENOW);
						iLastEditNoteLength = ptx - PutStartX + 1;
					
					}
				}

			}

			if(mouse_y >= WHeight+292-WHNM && mouse_y < WHeight+(351+7)-WHNM){//パン配置
				mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
				mouse_y = (WHeight+(351+5)-WHNM - mouse_y)/5;//96*12は楽譜の縦サイズ(Pixel)

				if(org_data.SetPan2(mouse_x,(unsigned char)mouse_y)==TRUE){ //存在する音符のみ変更	// 2010.08.14 C
					org_data.PutMusic();//楽譜の再描画
					RedrawWindow(hWnd,&rectPAN,NULL,RDW_INVALIDATE|RDW_ERASENOW);
				}
			}else if(mouse_y >= WHeight+365-WHNM && mouse_y < WHeight+428-WHNM){//ボリューム配置
				mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
				mouse_y = (WHeight+428-WHNM - mouse_y)*4;//96*12は楽譜の縦サイズ(Pixel)
				if(Last_VOL_Drag_mouse_x > -99999){
					long xxx, xxxplusvalue, xxxredraw = 0;
					xxxplusvalue = (mouse_x >= Last_VOL_Drag_mouse_x) ? 1 : -1;
					for(xxx = Last_VOL_Drag_mouse_x; xxx != mouse_x; xxx += xxxplusvalue){ //マウスポインタがすばやくてとんだ間も補完しつつ消す
						long yyy;
						if(mouse_x > Last_VOL_Drag_mouse_x){
							yyy = (mouse_y - Last_VOL_Drag_mouse_y) / (mouse_x - Last_VOL_Drag_mouse_x) * (xxx - Last_VOL_Drag_mouse_x) + Last_VOL_Drag_mouse_y;
						}else if(mouse_x < Last_VOL_Drag_mouse_x){
							yyy = (Last_VOL_Drag_mouse_y - mouse_y) / (Last_VOL_Drag_mouse_x - mouse_x) * (xxx - mouse_x) + mouse_y;
						}else yyy = mouse_y;
						if(org_data.SetVolume(xxx,(unsigned char)yyy)==TRUE){ 
							//org_data.PutMusic();//楽譜の再描画
							xxxredraw++; //RedrawWindow(hWnd,&rectVOL,NULL,RDW_INVALIDATE|RDW_ERASENOW);
						}
					}
					Last_VOL_Drag_mouse_x = mouse_x; //2014.05.02 A
					Last_VOL_Drag_mouse_y = mouse_y;
					if(org_data.SetVolume(mouse_x,(unsigned char)mouse_y)==TRUE){	// 2010.08.14 C
						//org_data.PutMusic();//楽譜の再描画
						xxxredraw++; //RedrawWindow(hWnd,&rectVOL,NULL,RDW_INVALIDATE|RDW_ERASENOW);
					}
					//if(xxxredraw){org_data.PutMusic(); RedrawWindow(hWnd,&rectVOL,NULL,RDW_INVALIDATE|RDW_ERASENOW);} //再描画系はまとめる。	// 2014.05.31 D
					if(xxxredraw){org_data.PutMusic(); RedrawWindow(hWnd,&rect2,NULL,RDW_INVALIDATE|RDW_ERASENOW);} //再描画系はまとめる。	// 2014.05.31 A
				}
			}
		}
		else if(wParam & MK_RBUTTON){
			if(mouse_y >= WHeight+365-WHNM && mouse_y < WHeight+428-WHNM){//ボリューム配置
				mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
				mouse_y = (WHeight+428-WHNM - mouse_y)*4;//96*12は楽譜の縦サイズ(Pixel)
				if(Last_VOL_Drag_mouse_x > -99999){ //2014.05.03 A //ドラッグ開始ポイントがVOLでなかったらSetUndoをキャンセルされている可能性もある。
					long xxx, xxxplusvalue, xxxredraw = 0;
					xxxplusvalue = (mouse_x >= Last_VOL_Drag_mouse_x) ? 1 : -1;
					for(xxx = Last_VOL_Drag_mouse_x; xxx != mouse_x; xxx += xxxplusvalue){ //マウスポインタがすばやくてとんだ間も補完しつつ消す
						if(org_data.CutVolume(xxx,(unsigned char)mouse_y)==TRUE){ 
							//org_data.PutMusic();//楽譜の再描画
							xxxredraw++; //RedrawWindow(hWnd,&rectVOL,NULL,RDW_INVALIDATE|RDW_ERASENOW);
						}
					}
					Last_VOL_Drag_mouse_x = mouse_x; //2014.05.02 A
					Last_VOL_Drag_mouse_y = mouse_y;
					if(org_data.CutVolume(mouse_x,(unsigned char)mouse_y)==TRUE){	// 2010.08.14 C
						//org_data.PutMusic();//楽譜の再描画
						xxxredraw++; //RedrawWindow(hWnd,&rectVOL,NULL,RDW_INVALIDATE|RDW_ERASENOW);
					}
					//if(xxxredraw){org_data.PutMusic(); RedrawWindow(hWnd,&rectVOL,NULL,RDW_INVALIDATE|RDW_ERASENOW);} //再描画系はまとめる。	// 2014.05.31 D
					if(xxxredraw){org_data.PutMusic(); RedrawWindow(hWnd,&rect2,NULL,RDW_INVALIDATE|RDW_ERASENOW);} //再描画系はまとめる。	// 2014.05.31 A
				}
			}
		}
		if((!(wParam & MK_LBUTTON)) && (PutStartX != -99999)){
			//org_data.SetNote_afterSetLength(PutStartX);
			org_data.CheckNoteTail(org_data.track);
			PutStartX = -99999;
			org_data.PutMusic();//楽譜の再描画
			RedrawWindow(hWnd,&rectNOTE,NULL,RDW_INVALIDATE|RDW_ERASENOW);

		}
		return;
	}
	if(wParam & MK_LBUTTON){
		//左ボタンドラッグ
		Note_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ; //ノートのx座標
		if(Note_x>StartNotex){
			if(sGrid || alt_down == 1){
				nc_Select.x1_1=MinimumGrid(StartNotex);
				nc_Select.x1_2=MaximumGrid(Note_x);
			}else if(alt_down == 2){
				nc_Select.x1_1=MinimumGridLine(StartNotex);
				nc_Select.x1_2=MaximumGridLine(Note_x);
			}else{
				nc_Select.x1_1=StartNotex;
				nc_Select.x1_2=Note_x;
			}
		}else if(Note_x<=StartNotex){ //ひっくり返す
			if(sGrid || alt_down == 1){
				nc_Select.x1_1=MinimumGrid(Note_x);
				nc_Select.x1_2=MaximumGrid(StartNotex);
			}else if(alt_down == 2){
				nc_Select.x1_1=MinimumGridLine(Note_x);
				nc_Select.x1_2=MaximumGridLine(StartNotex);
			}else{
				nc_Select.x1_1=Note_x;
				nc_Select.x1_2=StartNotex;
			}
		}else{
			nc_Select.x1_1=Note_x;
			nc_Select.x1_2=StartNotex;
		}
		org_data.RedrawSelectArea();
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

	}



}

void ClickProcL(WPARAM wParam, LPARAM lParam)
{
	RECT rect = {64,0,WWidth,WHeight};
	long mouse_x;
	long mouse_y;
	long scr_h,scr_v;
	long Note_x;
	unsigned char line,dot;
	if(timer_sw)return;

	scr_data.GetScrollPosition(&scr_h,&scr_v);
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	dot = mi.dot;
	line = mi.line;
	PutStartX = -99999;
	Last_VOL_Drag_mouse_x = -99999;
	alt_down = (GetKeyState(VK_MENU)<0)? 1: ((GetKeyState(VK_APPS)<0)? 2: 0) ;
	shift_down = (GetKeyState(VK_TAB)<0)? 1: 0;

	//マウスの座標を取得
	mouse_data.GetMousePosition(&mouse_x,&mouse_y);

	//Shiftが押されていたらSelect(範囲選択)扱い
	if(shift_down == 1 && mouse_y < WHeight+288-WHNM+144){
		mouse_y = WHeight+288-WHNM+144 + 8;
	}

	if(mouse_x < 64){
//		if(mouse_x >= 0 && mouse_y >= 0 && mouse_y < 288+WDWHEIGHTPLUS){//鍵盤
		if(mouse_x >= 0 && mouse_y >= 0 && mouse_y < WHeight+288-WHNM){//鍵盤
			org_data.TouchKeyboard(unsigned char(95 - (mouse_y/12 + scr_v)));//96*12は楽譜の縦サイズ
		}
		if(mouse_x >= 0 && mouse_y >= WHeight+288-WHNM+144 && mouse_y < WHeight+288-WHNM+144+16){//Selectの部位 2014.05.01
			ChangeSelAlwaysCurrent();
			org_data.PutMusic();//楽譜を表示
			rect.left = 0; rect.right=64; rect.top = WHeight+288-WHNM+144; rect.bottom = WHeight+288-WHNM+144+16;
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			return;			
		}
		if(mouse_x >= 0 && mouse_y >= WHeight+288-WHNM && mouse_y < WHeight+288-WHNM+72){//PAN の部位 2014.05.01
			iActivatePAN = -iActivatePAN + 1;
			org_data.PutMusic();//楽譜を表示
			rect.left = 0; rect.right=64; rect.top = WHeight+288-WHNM; rect.bottom = WHeight+288-WHNM+72;
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			return;			
		}
		if(mouse_x >= 0 && mouse_y >= WHeight+288-WHNM+72 && mouse_y < WHeight+288-WHNM+144){//VOL の部位 2014.05.01
			iActivateVOL = -iActivateVOL + 1;
			org_data.PutMusic();//楽譜を表示
			rect.left = 0; rect.right=64; rect.top = WHeight+288-WHNM+72; rect.bottom = WHeight+288-WHNM+144 ;
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			return;			
		}

		return;
	}
	
//	if(mouse_y >= 0 && mouse_y < 288+WDWHEIGHTPLUS){//音符配置
	if(mouse_y >= 0 && mouse_y < WHeight+288-WHNM){//音符配置
		//取得した座標を楽譜座標に変換
		mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
		mouse_y = 95 - (mouse_y/12 + scr_v);//96*12は楽譜の縦サイズ(Pixel)
		if(mouse_y<0)return; //範囲外
		//音符を配置
		SetUndo();
		if(iDragMode && (org_data.track<MAXMELODY))PutStartX = mouse_x; //初期位置記憶
		if(NoteWidth != 16){
			AdjustX = org_data.SearchNoteC(mouse_x, (unsigned char)mouse_y, 16 / NoteWidth, 16 % NoteWidth); //2014.05.28
			if(AdjustX > 0) mouse_x -= AdjustX;
		}else AdjustX = 0;
		if(org_data.SetNote(mouse_x,(unsigned char)mouse_y,iDragMode)==FALSE)ResetLastUndo(); //長くしすぎて失敗したときは戻す
		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		ClearDrag();
//	}else if(mouse_y >= 292+WDWHEIGHTPLUS && mouse_y < 351+7+WDWHEIGHTPLUS){//パン配置
	}else if(mouse_y >= WHeight+292-WHNM && mouse_y < WHeight+(351+7)-WHNM){//パン配置
		mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
//		mouse_y = (351+5+WDWHEIGHTPLUS - mouse_y)/5;//96*12は楽譜の縦サイズ(Pixel)
		mouse_y = (WHeight+(351+5)-WHNM - mouse_y)/5;//96*12は楽譜の縦サイズ(Pixel)
		if((mouse_x != Last_mouse_x) ||(mouse_y != Last_mouse_y))SetUndo();
		org_data.SetPan(mouse_x,(unsigned char)mouse_y);
		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		ClearDrag();
//	}else if(mouse_y >= 365+WDWHEIGHTPLUS && mouse_y < 428+WDWHEIGHTPLUS){//ボリューム配置
	}else if(mouse_y >= WHeight+365-WHNM && mouse_y < WHeight+428-WHNM){//ボリューム配置
		mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
//		mouse_y = (428+WDWHEIGHTPLUS - mouse_y)*4;//96*12は楽譜の縦サイズ(Pixel)
		mouse_y = (WHeight+428-WHNM - mouse_y)*4;//96*12は楽譜の縦サイズ(Pixel)
		if((mouse_x != Last_mouse_x) ||(mouse_y != Last_mouse_y))SetUndo();
		org_data.SetVolume(mouse_x,(unsigned char)mouse_y);
//		org_data.SetVolume2(mouse_x,(unsigned char)mouse_y,0); //やりかけ
		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		Last_VOL_Drag_mouse_x = mouse_x; //2014.05.02 A
		Last_VOL_Drag_mouse_y = mouse_y;
		ClearDrag();
	}else if(mouse_y >=WHeight-13 && mouse_y<=WHeight-2){ //選択範囲
		if((wParam & MK_SHIFT)&&(tra>=0)){ //追加モード
			Note_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ; //ノートのx座標
			if(Note_x>nc_Select.x1_1){
				if(sGrid || alt_down == 1){
					nc_Select.x1_2=MaximumGrid(Note_x);
				}else if(alt_down == 2){
					nc_Select.x1_2=MaximumGridLine(Note_x);
				}else{
					nc_Select.x1_2=Note_x;
				}
			}else{ //ひっくり返す
				if(sGrid || alt_down == 1){
					nc_Select.x1_1=MinimumGrid(Note_x);
				}else if(alt_down == 2){
					nc_Select.x1_1=MinimumGridLine(Note_x);
				}else{
					nc_Select.x1_1=Note_x;
				}
			}
		}else{
			//シフトが押されていなかったら開始点とみなす
			DragStartx = mouse_x;
			StartNotex = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ; //ノートのx座標
			if(sGrid || alt_down == 1){ //グリッド使用
				nc_Select.x1_1 = MinimumGrid(StartNotex);
				nc_Select.x1_2 = MaximumGrid(StartNotex);
			}else if(alt_down == 2){
				nc_Select.x1_1 = MinimumGridLine(StartNotex);
				nc_Select.x1_2 = MaximumGridLine(StartNotex);
			}else{
				nc_Select.x1_1 = StartNotex;
				nc_Select.x1_2 = StartNotex;
			}
			tra = org_data.track;
			if(wParam & MK_CONTROL){ //コントロールを押していれば全トラック
				ful=1;
			}else ful=0;
		}

		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		ShowStatusMessage();
	}
	Last_mouse_x = mouse_x;
	Last_mouse_y = mouse_y;
	//Last_VOL_Drag_mouse_x = mouse_x; //2014.05.02 A
	//Last_VOL_Drag_mouse_y = mouse_y;

	//以下はテスト用
//	char str[80];
//	HDC hdc;
//	hdc = GetDC(hWnd);
//	sprintf(str,"ypos:%4d",mouse_y);
//	TextOut(hdc,200,100,str,strlen(str));
//	ReleaseDC(hWnd,hdc);
}
void ClickProcR(WPARAM wParam, LPARAM lParam)
{
	RECT rect = {64,0,WWidth,WHeight};
	long mouse_x;
	long mouse_y;
	long scr_h,scr_v;
	long AdjustX;
	unsigned char line,dot;
	int NextTrack;
	if(timer_sw)return;
	
	scr_data.GetScrollPosition(&scr_h,&scr_v);
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	dot = mi.dot;
	line = mi.line;
	//マウスの座標を取得
	mouse_data.GetMousePosition(&mouse_x,&mouse_y);

	if(GetKeyState(VK_TAB)<0){
		mouse_y = WHeight+288-WHNM+144 + 8;
	}

	SetUndo();

	if(mouse_x < 64)return;
	//音符カット
//	if(mouse_y >= 0 && mouse_y < 288+WDWHEIGHTPLUS){
	if(mouse_y >= 0 && mouse_y < WHeight+288-WHNM){
		//取得した座標を楽譜座標に変換
		mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
		mouse_y = 95  - (mouse_y/12 + scr_v);//96*12は楽譜の縦サイズ(Pixel)
		if(NoteWidth != 16){
			AdjustX = org_data.SearchNoteC(mouse_x, (unsigned char)mouse_y, 16 / NoteWidth, 16 % NoteWidth); //2014.05.28
			if(AdjustX > 0) mouse_x -= AdjustX;
		}

		//音符を配置
		if(org_data.CutNote(mouse_x,(unsigned char)mouse_y)==FALSE){
			//削除するべき音符は存在しなかった。
			ResetLastUndo();
			NextTrack = org_data.SearchNote(mouse_x,(unsigned char)mouse_y,org_data.track);
			//MessageBox(NULL,"l",NULL,NULL);
			if(NextTrack>=0 && NextTrack!=org_data.track){
				//トラック変更
				ChangeTrack(hWnd, NextTrack);
			}
		}
		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
//	}else if(mouse_y >= 292+WDWHEIGHTPLUS && mouse_y < 351+7+WDWHEIGHTPLUS){//パン配置
	}else if(mouse_y >= WHeight+292-WHNM && mouse_y < WHeight+351+7-WHNM){//パン配置
		mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
//		mouse_y = (351+5+WDWHEIGHTPLUS - mouse_y)/5;//96*12は楽譜の縦サイズ(Pixel)
		mouse_y = (WHeight+351+5-WHNM - mouse_y)/5;//96*12は楽譜の縦サイズ(Pixel)
		if(org_data.CutPan(mouse_x,(unsigned char)mouse_y)==FALSE)ResetLastUndo();
		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		ClearDrag();
//	}else if(mouse_y >= 365+WDWHEIGHTPLUS && mouse_y < 428+WDWHEIGHTPLUS){//ボリューム配置
	}else if(mouse_y >= WHeight+365-WHNM && mouse_y < WHeight+428-WHNM){//ボリューム配置
		mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
//		mouse_y = (428+WDWHEIGHTPLUS - mouse_y)*4;//96*12は楽譜の縦サイズ(Pixel)
		mouse_y = (WHeight+428-WHNM - mouse_y)*4;//96*12は楽譜の縦サイズ(Pixel)
		//if(org_data.CutVolume(mouse_x,(unsigned char)mouse_y)==FALSE)ResetLastUndo(); //2014.05.03 D //このままドラッグされるとアンドゥできない
		org_data.PutMusic();//楽譜の再描画
		ClearDrag();
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		Last_VOL_Drag_mouse_x = mouse_x; //2014.05.02 A //これはVOLでしか使わない。
		Last_VOL_Drag_mouse_y = mouse_y;
	}else if(mouse_y >=WHeight-13 && mouse_y<=WHeight-2){ //選択範囲
		//DragStartx = mouse_x; //ドラッグ開始点
		tra=-256;
		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		SetEZCWindowMessage("");
		ClearDrag();
		ShowStatusMessage();
	}

	//以下はテスト用
//	char str[80];
//	HDC hdc;
//	hdc = GetDC(hWnd);
//	sprintf(str,"ypos:%4d",mouse_y);
//	TextOut(hdc,200,100,str,strlen(str));
//	ReleaseDC(hWnd,hdc);
}

void ClickProcM(WPARAM wParam, LPARAM lParam)
{
	RECT rect = {64,0,WWidth,WHeight};
	long mouse_x;
	long mouse_y;
	long scr_h,scr_v;
	unsigned char line,dot;
	int NextTrack, NextTrackB, iTrc;
	long x1, x2;
	if(timer_sw)return;
	
	scr_data.GetScrollPosition(&scr_h,&scr_v);
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	dot = mi.dot;
	line = mi.line;
	//マウスの座標を取得
	mouse_data.GetMousePosition(&mouse_x,&mouse_y);
	if(mouse_x < 64)return;

	if(mouse_y >= 0 && mouse_y < WHeight+288-WHNM){
		//取得した座標を楽譜座標に変換
		mouse_x = (mouse_x - KEYWIDTH)/NoteWidth + scr_h*line*dot ;
		mouse_y = 95  - (mouse_y/12 + scr_v);//96*12は楽譜の縦サイズ(Pixel)


		NextTrack = org_data.SearchNote(mouse_x,(unsigned char)mouse_y,org_data.track);
		iTrc = (NextTrack >=0) ? NextTrack : org_data.track; 
		NextTrackB = org_data.SearchNoteB(mouse_x, (unsigned char)mouse_y , &x1, &x2, iTrc); 

		//MessageBox(NULL,"l",NULL,NULL);
		if(NextTrack>=0 && NextTrack!=org_data.track){
			//トラック変更
			//ChangeTrack(hWnd, NextTrack);
		}
		if(NextTrack<0 && NextTrackB>=0){
			NextTrack = NextTrackB;
		}
		if(NextTrack>=0){
			//音符の開始と終了を取得する
			
			//ここではGridを無視する。
			nc_Select.x1_1 = x1;
			nc_Select.x1_2 = x2;
			tra = NextTrack;
			ful = (wParam & MK_CONTROL)? 1 : 0;
			PrintStatusMessage(0);
			org_data.RedrawSelectArea();
		}else{
			//選択範囲のｸﾘｱ
			tra=-256;
			org_data.PutMusic();//楽譜の再描画
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			SetEZCWindowMessage("");
			ClearDrag();
			ShowStatusMessage();
		}

		org_data.PutMusic();//楽譜の再描画
		RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	}else if(mouse_y >= WHeight+292-WHNM && mouse_y < WHeight+351+7-WHNM){//パン配置
	}else if(mouse_y >= WHeight+365-WHNM && mouse_y < WHeight+428-WHNM){//ボリューム配置
	}else if(mouse_y >=WHeight-13 && mouse_y<=WHeight-2){ //選択範囲
	}
	//以下はテスト用
//	char str[80];
//	HDC hdc;
//	hdc = GetDC(hWnd);
//	sprintf(str,"ypos:%4d",mouse_y);
//	TextOut(hdc,200,100,str,strlen(str));
//	ReleaseDC(hWnd,hdc);
}

