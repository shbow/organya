#include <string.h>
#include "Setting.h"
#include "DefOrg.h"
#include "Gdi.h"
#include "OrgData.h"
#include "Scroll.h"

#include "rxoFunction.h"

extern int sACrnt;	//範囲選択は常にｶﾚﾝﾄﾄﾗｯｸ

//◆◆表示部◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
RECT note_rect[] = {
	{  0,  0, 16,  6},//キー音符(編集)
	{  0,  6, 16, 12},//キー音符
	{ 16,  0, 32,  4},//尻尾(編集)
	{ 16,  4, 32,  8},//尻尾
	{ 16, 16, 32, 21},//パン・ボリューム
	{  0, 32, 16, 44},//Repeat(5)
	{ 16, 32, 32, 44},//end
};

RECT note_blue_rect[]={ //数字入りのキー音符（頭）
	{ 0,  48, 16,  54},
	{16,  48, 32,  54},
	{ 0,  56, 16,  62},
	{16,  56, 32,  62},
	{ 0,  64, 16,  70},
	{16,  64, 32,  70},
	{ 0,  72, 16,  78},
	{16,  72, 32,  78},
	{ 0,  80, 16,  86},
	{16,  80, 32,  86},
	{ 0,  88, 16,  94},
	{16,  88, 32,  94},
	{ 0,  96, 16, 102},
	{16,  96, 32, 102},
	{ 0, 104, 16, 110},
	{16, 104, 32, 110},
	{ 0, 112, 16, 118},
	{16, 112, 32, 118},
	{ 0, 120, 16, 126},
	{16, 120, 32, 126},
	{ 0, 128, 16, 134},
	{16, 128, 32, 134},
	{ 0, 136, 16, 142},
	{16, 136, 32, 142},
	{ 0, 144, 16, 150},
	{16, 144, 32, 150},
	{ 0, 152, 16, 158},
	{16, 152, 32, 158},
	{ 0, 160, 16, 166},
	{16, 160, 32, 166},
	{ 0, 168, 16, 174},
	{16, 168, 32, 174},
};

/*
RECT note_tail_rect[]={ //しっぽ
	{32,   0, 64,   4},
	{32,   4, 64,   8},
	{32,   8, 64,  12},
	{32,  12, 64,  16},
	{32,  16, 64,  20},
	{32,  20, 64,  24},
	{32,  24, 64,  28},
	{32,  28, 64,  32},
	{32,  32, 64,  36},
	{32,  36, 64,  40},
	{32,  40, 64,  44},
	{32,  44, 64,  48},
	{32,  48, 64,  52},
	{32,  52, 64,  56},
	{32,  56, 64,  60},
	{32,  60, 64,  64},
	{32,  64, 64,  68}, //yobun
};
*/
RECT note_tail_rect[]={ //しっぽ
	{32,   0, 48,   4},
	{32,   4, 48,   8},
	{32,   8, 48,  12},
	{32,  12, 48,  16},
	{32,  16, 48,  20},
	{32,  20, 48,  24},
	{32,  24, 48,  28},
	{32,  28, 48,  32},
	{32,  32, 48,  36},
	{32,  36, 48,  40},
	{32,  40, 48,  44},
	{32,  44, 48,  48},
	{32,  48, 48,  52},
	{32,  52, 48,  56},
	{32,  56, 48,  60},
	{32,  60, 48,  64},
	{32,  64, 48,  68}, //yobun
};
RECT msc_rect[] = {
	{  0,  0, 64,144+16},//鍵盤
	{ 64,  0, 80,144+16},//小節ライン
	{ 80,  0, 96,144+16},//一拍ライン
	{ 96,  0,112,144+16},//1/16ライン
	{112+  0,  0,112+ 64,144+16},//鍵盤
	{112+ 64,  0,112+ 80,144+16},//小節ライン
	{112+ 80,  0,112+ 96,144+16},//一拍ライン
	{112+ 96,  0,112+112,144+16},//1/16ライン
};
RECT num_rect[] = {
	{  0,  0,  8, 12},
	{  8,  0, 16, 12},
	{ 16,  0, 24, 12},
	{ 24,  0, 32, 12},
	{ 32,  0, 40, 12},
	{ 40,  0, 48, 12},
	{ 48,  0, 56, 12},
	{ 56,  0, 64, 12},
	{ 64,  0, 72, 12},
	{ 72,  0, 80, 12},

	{  0, 12,  8, 24},
	{  8, 12, 16, 24},
	{ 16, 12, 24, 24},
	{ 24, 12, 32, 24},
	{ 32, 12, 40, 24},
	{ 40, 12, 48, 24},
	{ 48, 12, 56, 24},
	{ 56, 12, 64, 24},
	{ 64, 12, 72, 24},
	{ 72, 12, 80, 24},
};
RECT rc_SelArea[] ={
	{0, 176, 16, 187}, //選択範囲
	{16, 176, 32, 187}, //選択範囲
	{32, 176, 48, 187}, //選択範囲
	{0, 192, 16, 203},  //選択範囲(他のTr)
	{16, 192, 32, 203}, //選択範囲(他のTr)
	{32, 192, 48, 203}, //選択範囲(他のTr)
	{32, 160, 48, 171}, //FULL
	{32, 144, 48, 155}, //１マス
};

RECT rc_CurrentSelect = {0, 208, 64, 224}; //2014.04.30
RECT rc_ActivePAN = {0, 224, 64, 296}; //2014.05.01 A
RECT rc_ActiveVOL = {0, 296, 64, 368}; //2014.05.01 A

RECT rc_TCPY[]={
	{32,88,48,94},
	{32,104,48,110},
	{32,96,48,100},
	{32,100,48,104},
};

RECT rc_PushKB[] ={    // 2010.09.23 A 押された鍵盤
	{112, 132, 208, 144}, //
	{112, 120, 208, 132}, //
	{112, 108, 208, 120}, //
	{112, 96, 208, 108}, //
	{112, 84, 208, 96}, //
	{112, 72, 208, 84}, //
	{112, 60, 208, 72}, //
	{112, 48, 208, 60}, //
	{112, 36, 208, 48}, //
	{112, 24, 208, 36}, //
	{112, 12, 208, 24}, //
	{112, 0, 208, 12}, //
};

RECT rc_PushKB2[] ={	// 2010.09.23 A 押された鍵盤（光る鍵盤背景つき）
	{112, 132, 176, 144}, //
	{112, 120, 176, 132}, //
	{112, 108, 176, 120}, //
	{112, 96, 176, 108}, //
	{112, 84, 176, 96}, //
	{112, 72, 176, 84}, //
	{112, 60, 176, 72}, //
	{112, 48, 176, 60}, //
	{112, 36, 176, 48}, //
	{112, 24, 176, 36}, //
	{112, 12, 176, 24}, //
	{112, 0, 176, 12}, //
};

RECT rc_PushKB3[] ={    // 2010.09.24 A 押された鍵盤(オクターブ違い）
	{208, 132, 272, 144}, //
	{208, 120, 272, 132}, //
	{208, 108, 272, 120}, //
	{208, 96, 272, 108}, //
	{208, 84, 272, 96}, //
	{208, 72, 272, 84}, //
	{208, 60, 272, 72}, //
	{208, 48, 272, 60}, //
	{208, 36, 272, 48}, //
	{208, 24, 272, 36}, //
	{208, 12, 272, 24}, //
	{208, 0, 272, 12}, //
};


extern NOTECOPY nc_Select; //選択範囲
extern int tra, ful ,haba; 
extern int gDrawDouble;	//両方のトラックグループを描画する

extern int iKeyPushDown[256]; // 2010.09.22 A ピアノキーの押され具合


//キーボードとピアノロールの位置
void OrgData::PutNumber(void)
{
	long scr_h,scr_v;
	char k100,k10,k1;
	char i,j;
	char k = info.dot*info.line;
	j = (WWidth/NoteWidth)/k;
	//小節を表示
	for(i = 0; i <= j; i++){
		scr_data.GetScrollPosition(&scr_h,&scr_v);
		scr_h += i;
		k100 = k10 = k1 = 0;
		while(scr_h >= 100){
			k100++;
			scr_h -= 100;
		}
		while(scr_h >= 10){
			k10++;
			scr_h -= 10;
		}
		
		PutBitmap(k*i*NoteWidth+0 +KEYWIDTH+1,0,&num_rect[k100],BMPNUMBER);
		PutBitmap(k*i*NoteWidth+8 +KEYWIDTH+1,0,&num_rect[k10],BMPNUMBER);
		PutBitmap(k*i*NoteWidth+16+KEYWIDTH+1,0,&num_rect[scr_h],BMPNUMBER);
		if(WHeight>550){
			PutBitmap(k*i*NoteWidth+0 +KEYWIDTH+1,WHeight+288-WHNM-12,&num_rect[k100],BMPNUMBER);
			PutBitmap(k*i*NoteWidth+8 +KEYWIDTH+1,WHeight+288-WHNM-12,&num_rect[k10],BMPNUMBER);
			PutBitmap(k*i*NoteWidth+16+KEYWIDTH+1,WHeight+288-WHNM-12,&num_rect[scr_h],BMPNUMBER);
		}
	}
	//キーを表示
	for(i = 0; i <  8; i++){
		PutBitmap(55,(95 - scr_v - i*12)*12,&num_rect[i+10],BMPNUMBER);
	}
}
void OrgData::PutRepeat(void)
{
	long scr_h,scr_v;
	long x;
	scr_data.GetScrollPosition(&scr_h,&scr_v);
	x = (info.repeat_x - scr_h*info.dot*info.line)*NoteWidth + 64;
	PutBitmap(x,WHeight+276-WHNM-12,&note_rect[5],BMPNOTE);
	x = (info.end_x - scr_h*info.dot*info.line)*NoteWidth + 64;
	PutBitmap(x,WHeight+276-WHNM-12,&note_rect[6],BMPNOTE);
}

//音符の表示
void OrgData::PutNotes(int TPCY)
{
	int i,j,k,t,addY;
	int n; //2010.09.23 A
	NOTELIST *p;
	long xpos,ypos;
	long scr_h,scr_v;
	unsigned char line,dot;
	//unsigned char trkbit[MAXMELODY];  
	//unsigned char trkmaskLower[MAXMELODY];
	//unsigned char trkmaskUpper[MAXMELODY];
	MUSICINFO mi;
	
	/*
	for(i=0,k=1; i<MAXMELODY && i<8; i++,k*=2){
		trkbit[i]       = (unsigned char)k;         // 1,     2,   4,   8,   16, ...
		trkmaskLower[i] = (unsigned char)(k-1);     // 0,     1,   3,   7,   15, ...
		trkmaskUpper[i] = (unsigned char)(256-2*k); // 254, 252, 248, 240,  224, ...
	}
	*/
	//必要なデータを取得
	GetMusicInfo(&mi);line = mi.line;dot = mi.dot;
	scr_data.GetScrollPosition(&scr_h,&scr_v);

	//for(i=0;i<128*92;i++)noteon[i] = 0;

	/*
	//音符の重なり具合を確認する //A 2014.05.03
	memset((void *)noteon, 0, 128*12*8);
	for(n = 0; n < MAXMELODY; n++){
		k = getRecentTrack(n , 0);
		if(k == track)continue;
		if((p = info.tdata[k].note_list) == NULL)continue;//音符が無ければ中止
		while(p != NULL && p->to != NULL){//どこから表示するか
			xpos = (p->x - line*dot*scr_h)*16 + KEYWIDTH;
			if(xpos >= KEYWIDTH)break;//表示領域に入った
			p = p->to;
		}
		//尻尾を表示する為に、一つ前の音符から・・・。
		if(p == NULL)continue;
		if(p->from != NULL)p = p->from;
		while(p->from != NULL && p->y == KEYDUMMY)p = p->from;
		//音符ががなくなるか、X座標が表示領域を超えるまで表示。
		while(p != NULL){
			//xmax128
			if(p->y != KEYDUMMY){
				for(e = p->x - line*dot*scr_h; (e < 128) && (e < (p->x - line*dot*scr_h + p->length) ); e++){
					noteon[p->y * 128 + e] |= trkbit[n];
				}
			}
			p = p->to;
		}
	}
	*/
	Dw_BeginToDraw();

	//編集しない音符を先に表示///////////
	for(n = 0; n < MAXMELODY; n++){
		k = getRecentTrack(n , 0);
		if(k == track)continue;
		if((p = info.tdata[k].note_list) == NULL)continue;//音符が無ければ中止
		while(p != NULL && p->to != NULL){//どこから表示するか
			xpos = (p->x - line*dot*scr_h)*NoteWidth + KEYWIDTH;
			if(xpos >= KEYWIDTH)break;//表示領域に入った
			p = p->to;
		}
		//尻尾を表示する為に、一つ前の音符から・・・。
		if(p == NULL)continue;
		if(p->from != NULL)p = p->from;
		while(p->from != NULL && p->y == KEYDUMMY)p = p->from;
		//音符ががなくなるか、X座標が表示領域を超えるまで表示。
		while(p != NULL){
			ypos = (95 - p->y - scr_v)*12;//下が0になる95が最大
			xpos = (p->x - line*dot*scr_h)*NoteWidth + KEYWIDTH;
			t = (p->y % 12); if(t==1 || t==3 || t==6 || t==8 || t==10)t=1;else t=0;
			if(xpos > WWidth)break;//表示領域を超えた。
			if(ypos >= 0 && ypos < WHeight+286-WHNM){//表示範囲YPOS
				addY = 0;
				/*
				for(i = p->length-1; i >= 0; i--){//尻尾ふくめて(>0)頭まで(==0)検索 //A 2014.05.06
					e = p->x - line*dot*scr_h + i;
					unsigned char uct = noteon[p->y * 128 + e];
					if((uct & trkmaskLower[track]) && track < n){ //自Trより小さいTr番号の音符があった
						addY-=2; break;
					}else if((uct & trkmaskLower[track]) && track > n){ //自Trより大きいTr番号の音符があった
						addY+=2; break;
					}
				}
				addY=-n*3; if(addY>=0)addY++;
				*/
				if(iSlideOverlapNotes!=0){
					addY=-k/2+2; //if(addY>=0)addY++;
					if(k>=4)addY--;
				}

				for(j = 0,i = p->length-1; i > 0; i--,j++){//尻尾
					if(TPCY==0)Dw_PutBitmap(xpos+j*NoteWidth+NoteWidth,ypos+3 +addY  ,&note_tail_rect[k+8],BMPNOTE);
					else Dw_PutBitmap(xpos+j*NoteWidth+NoteWidth,ypos+3 +addY  ,&rc_TCPY[2+t],BMPNOTE);
				}
				if(TPCY==0)Dw_PutBitmap_Head(xpos,ypos+2 +addY  ,&note_blue_rect[k],BMPNOTE,p->length);//音符(新)
				else Dw_PutBitmap_Head(xpos,ypos+2 +addY  ,&rc_TCPY[t],BMPNOTE,p->length);//音符(新)
			}
			p = p->to;
		}
	}
	//編集する音符を表示///////////
	if((p = info.tdata[track].note_list) == NULL || TPCY==1){
		//return;//音符が無ければ中止
	}else{
		while(p != NULL && p->to != NULL){//どこから表示するか
			xpos = (p->x - line*dot*scr_h)*NoteWidth + KEYWIDTH;
			if(xpos >= KEYWIDTH)break;
			p = p->to;
		}
		//尻尾を表示する為に、一つ前の音符から・・・。
		if(p == NULL){
			//return;
		}else{
			if(p->from != NULL)p = p->from;
			while(p->from != NULL && p->y == KEYDUMMY)p = p->from;
			//音符ががなくなるか、X座標が表示領域を超えるまで表示。
			while(p != NULL){
				ypos = (95 - p->y - scr_v)*12;//下が0になる95が最大
				xpos = (p->x - line*dot*scr_h)*NoteWidth + KEYWIDTH;
				if(xpos > WWidth)break;//表示領域を超えた。
				if(ypos >= 0 && ypos < WHeight+286-WHNM){//表示範囲YPOS
					//tBitmap(xpos,ypos+2,&note_rect[0],BMPNOTE);//音符
					for(j = 0,i = p->length-1; i > 0; i--,j++){//尻尾
						//PutBitmap(xpos+j*16+16,ypos+3,&note_rect[2],BMPNOTE);
						Dw_PutBitmap(xpos+j*NoteWidth+NoteWidth,ypos+3,&note_tail_rect[track],BMPNOTE);
					}
					Dw_PutBitmap_Head(xpos,ypos+2,&note_blue_rect[track+16],BMPNOTE,p->length);//音符(新)
				}
				if(p->pan != PANDUMMY){
					ypos = WHeight + 351-WHNM - (p->pan * 5);//パン
					Dw_PutBitmap_Center(xpos,ypos,&note_rect[4],BMPNOTE);
				}
				if(p->volume != VOLDUMMY){
					ypos = WHeight + 426 -WHNM - (p->volume/4);//ボリューム
					Dw_PutBitmap_Center(xpos,ypos,&note_rect[4],BMPNOTE);
				}
				p = p->to;
			}
		}
	}
	Dw_FinishToDraw();
}
//音符(ドラム)の表示
void OrgData::PutNotes2(int TPCY)
{
	int i,j,k,t,addY;
	int n; //2010.09.23 A
	NOTELIST *p;
	long xpos,ypos;
	long scr_h,scr_v;
	unsigned char line,dot;
	MUSICINFO mi;
	//必要なデータを取得
	GetMusicInfo(&mi);line = mi.line;dot = mi.dot;
	scr_data.GetScrollPosition(&scr_h,&scr_v);

	Dw_BeginToDraw();

	//編集しない音符を先に表示///////////
	for(n = MAXMELODY; n < MAXTRACK; n++){
		k = getRecentTrack(n - MAXMELODY, 1);
		if(k == track)continue;
		if((p = info.tdata[k].note_list) == NULL){
//			MessageBox(hWnd,"","",MB_OK);
			continue;//音符が無ければ中止
		}
		while(p != NULL && p->to != NULL){//どこから表示するか
			xpos = (p->x - line*dot*scr_h)*NoteWidth + KEYWIDTH;
			if(xpos >= KEYWIDTH)break;
			p = p->to;
		}
		//尻尾を表示する為に、一つ前の音符から・・・。
		if(p == NULL)continue;
		if(p->from != NULL)p = p->from;
		while(p->from != NULL && p->y == KEYDUMMY)p = p->from;
		//音符ががなくなるか、X座標が表示領域を超えるまで表示。
		while(p != NULL){
			ypos = (95 - p->y - scr_v)*12;//下が0になる95が最大
			xpos = (p->x - line*dot*scr_h)*NoteWidth + KEYWIDTH;
			t = (p->y % 12); if(t==1 || t==3 || t==6 || t==8 || t==10)t=1;else t=0;
			if(xpos > WWidth)break;//表示領域を超えた。
//			if(ypos >= 0 && ypos < 286+WDWHEIGHTPLUS){//表示範囲YPOS
			if(ypos >= 0 && ypos < WHeight+286-WHNM){//表示範囲YPOS
				addY = 0;
				/*if(iSlideOverlapNotes!=0){
					addY=-(k-MAXMELODY)/2+2; //if(addY>=0)addY++;
					if((k-MAXMELODY)>=4)addY--;
				}*/
				for(j = 0,i = p->length-1; i > 0; i--,j++){//尻尾
					if(TPCY==0)Dw_PutBitmap(xpos+j*NoteWidth+NoteWidth,ypos+3+addY,&note_tail_rect[k],BMPNOTE);
					else Dw_PutBitmap(xpos+j*NoteWidth+NoteWidth,ypos+3+addY,&rc_TCPY[2+t],BMPNOTE);
				}
				if(TPCY==0)Dw_PutBitmap_Head(xpos,ypos+2+addY,&note_blue_rect[k],BMPNOTE,p->length);//音符
				else Dw_PutBitmap_Head(xpos,ypos+2+addY,&rc_TCPY[0+t],BMPNOTE,p->length);//音符
			}
			p = p->to;
		}
	}
	//編集する音符を表示///////////
	if((p = info.tdata[track].note_list) == NULL || TPCY==1){
		//return;//音符が無ければ中止
	}else{
		while(p != NULL && p->to != NULL){//どこから表示するか
			xpos = (p->x - line*dot*scr_h)*16 + KEYWIDTH;
			if(xpos >= KEYWIDTH)break;
			p = p->to;
		}
		//尻尾を表示する為に、一つ前の音符から・・・。
		if(p == NULL){
			//return;
		}else{
			if(p->from != NULL)p = p->from;
			while(p->from != NULL && p->y == KEYDUMMY)p = p->from;
			//音符ががなくなるか、X座標が表示領域を超えるまで表示。
			while(p != NULL){
				ypos = (95 - p->y - scr_v)*12;//下が0になる95が最大
				xpos = (p->x - line*dot*scr_h)*NoteWidth + KEYWIDTH;
				if(xpos > WWidth)break;//表示領域を超えた。
				if(ypos >= 0 && ypos < WHeight+286-WHNM){//表示範囲YPOS
					//PutBitmap(xpos,ypos+2,&note_rect[0],BMPNOTE);//音符
					//Dw_PutBitmap(xpos,ypos+2,&note_blue_rect[track+16],BMPNOTE);//音符	// 2014.05.27 D
					for(j = 0,i = p->length-1; i > 0; i--,j++){//尻尾
						Dw_PutBitmap(xpos+j*NoteWidth+NoteWidth,ypos+3,&note_tail_rect[track-8],BMPNOTE);
					}
					Dw_PutBitmap_Head(xpos,ypos+2,&note_blue_rect[track+16],BMPNOTE,p->length);//音符	// 2014.05.27 A
				}
				if(p->pan != PANDUMMY){
					ypos = WHeight + 351-WHNM - (p->pan * 5);//パン
					Dw_PutBitmap_Center(xpos,ypos,&note_rect[4],BMPNOTE);
				}
				if(p->volume != VOLDUMMY){
					ypos = WHeight + 426-WHNM - (p->volume/4);//ボリューム
					Dw_PutBitmap_Center(xpos,ypos,&note_rect[4],BMPNOTE);
				}
				p = p->to;
			}
		}
	}

	Dw_FinishToDraw();
}
//楽譜の表示
void OrgData::PutMusic(void)
{
	int j;
	int i; // A 2010.09.24
	long hpos,vpos,vpos2;
	scr_data.GetScrollPosition(&hpos,&vpos);
	vpos2=vpos;
	vpos = -(vpos%12)*12;
	//ここ以降に楽譜表示を記述
	for(j = 0; j < 8; j++)PutMusicParts(64,j*144 +vpos);//楽譜
	PutPanParts();//パンライン

	//キーボード鍵盤（譜面背景を光らす部分）
	for(j = 0; j < 96 ; j++){ // 2010.09.22 A
		if(iKeyPushDown[j]!=0){
			PutBitmap(0,  (95 - j - vpos2)*12, &rc_PushKB[j%12],BMPMUSIC);//鍵盤
		}
	}


	if(gDrawDouble==0){
		if(track < MAXMELODY)PutNotes();
		else PutNotes2();
	}else{
		if(track < MAXMELODY){
			PutNotes2(1);	//半透明表示
			PutNotes();		//実体表示
		}
		else{
			PutNotes(1);	//半透明表示
			PutNotes2();	//実体表示
		}
		
	}
	for(j = 0; j < 8; j++)PutBitmap(0,j*144 +vpos,&msc_rect[0],BMPMUSIC);//鍵盤

	//キーボード鍵盤（鍵盤部分）
	for(j = 0; j < 96 ; j++){ // 2010.09.22 A
		if(iKeyPushDown[j]!=0){
			//A 2010.09.24
			for(i = 0; i < 8; i++){
				PutBitmap(0,  (95 - (j%12)- i*12 - vpos2)*12, &rc_PushKB3[j%12],BMPMUSIC);//オクターブ違い鍵盤
			}
			PutBitmap(0,  (95 - j - vpos2)*12, &rc_PushKB2[j%12],BMPMUSIC);//鍵盤
		}
	}

	PutSelectArea();

	PutNumber();
	PutRepeat();
	PutBitmap(0,WHeight+288-WHNM,&msc_rect[0],BMPPAN);//パン

	if(sACrnt){ //2014.04.30
		PutBitmap(0,WHeight+288-WHNM+144,&rc_CurrentSelect,BMPNOTE);//範囲選択は常にｶﾚﾝﾄﾄﾗｯｸのとき
	}
	if(iActivatePAN){ //2014.05.01
		PutBitmap(0,WHeight+288-WHNM,&rc_ActivePAN, BMPNOTE);
	}
	if(iActivateVOL){ //2014.05.01
		PutBitmap(0,WHeight+288-WHNM+72,&rc_ActiveVOL, BMPNOTE);
	}
}

//選択範囲の表示
void OrgData::PutSelectArea()
{
	if(tra<0)return; //選択されてへん
	long ypos;
	long scr_h,scr_v;
	unsigned char line,dot;
	MUSICINFO mi;
	//必要なデータを取得
	GetMusicInfo(&mi);line = mi.line;dot = mi.dot;
	scr_data.GetScrollPosition(&scr_h,&scr_v);

	int xSelS, xSelE,xx,t;
	t = 0; if(ful==0 && tra!=track)t=3;
	xSelS = (nc_Select.x1_1 - line*dot*scr_h)*NoteWidth + KEYWIDTH; //選択開始点
	xSelE = (nc_Select.x1_2 - line*dot*scr_h)*NoteWidth + KEYWIDTH; //選択終了点
	ypos = WHeight - 13;
	for(xx=KEYWIDTH;xx<=WWidth+NoteWidth;xx+=NoteWidth){
		if(xx==xSelS){
			if(xx==xSelE)PutBitmapCenter16(xx,ypos,&rc_SelArea[7],BMPNOTE);
			else PutBitmapCenter16(xx,ypos,&rc_SelArea[0+t],BMPNOTE);
		}else if(xx>xSelS && xx<xSelE){
			if(((xx-KEYWIDTH)/NoteWidth + scr_h)%4==0 && ful==1 ){
				PutBitmapCenter16(xx,ypos,&rc_SelArea[6],BMPNOTE); //FULL
			}else{
				PutBitmapCenter16(xx,ypos,&rc_SelArea[1+t],BMPNOTE);
			}
		}else if(xx==xSelE){
			PutBitmapCenter16(xx,ypos,&rc_SelArea[2+t],BMPNOTE);
		}
	}


}

void OrgData::RedrawSelectArea()
{
	PutSelectParts();//パンライン
	PutSelectArea();

}
