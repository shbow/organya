#include "Setting.h"
#include "Gdi.h"
#include "DefOrg.h"
#include "OrgData.h"
#include "resource.h"
#include <memory.h>
#include <math.h>

#include "Sound.h"
#include "rxoFunction.h"

#define DEFVOLUME	200//255はVOLDUMMY。MAXは254
#define DEFPAN		6
extern HWND hDlgTrack;
extern char *dram_name[];
//指定の数だけNoteDataの領域を確保(初期化)
BOOL OrgData::NoteAlloc(unsigned short alloc)
{
	int i,j;
	for(j = 0; j < MAXTRACK; j++){
		info.tdata[j].wave_no = 0;
		info.tdata[j].note_list = NULL;//コンストラクタにやらせたい
		info.tdata[j].note_p = new NOTELIST[alloc];
		if(info.tdata[j].note_p == NULL)return FALSE;
		//	info.alloc_note = alloc;
		for(i = 0; i < alloc; i++){
			(info.tdata[j].note_p + i)->from = NULL;
			(info.tdata[j].note_p + i)->to = NULL;
			(info.tdata[j].note_p + i)->length = 0;
			(info.tdata[j].note_p + i)->pan = PANDUMMY;
			(info.tdata[j].note_p + i)->volume = VOLDUMMY;
			(info.tdata[j].note_p + i)->y = KEYDUMMY;
		}
	}
	for(j = 0; j < MAXMELODY; j++)
		MakeOrganyaWave(j,info.tdata[j].wave_no, info.tdata[j].pipi);
	for(j = 0; j < MAXDRAM; j++)
		InitDramObject("Bass01",j);

	track = 0;//////////今はここに書いておく
	return TRUE;
}
//NoteDataを開放
void OrgData::ReleaseNote(void)
{
	for(int i = 0; i < MAXTRACK; i++){
		if(info.tdata[i].note_p != NULL)delete info.tdata[i].note_p;
	}
}
//曲情報を取得
void OrgData::GetMusicInfo(MUSICINFO *mi){
	mi->dot = info.dot;
	mi->line = info.line;
	mi->alloc_note = info.alloc_note;
	mi->wait = info.wait;
	mi->repeat_x = info.repeat_x;
	mi->end_x = info.end_x;
	for(int i = 0; i < MAXTRACK; i++){
		mi->tdata[i].freq = info.tdata[i].freq;
		mi->tdata[i].wave_no = info.tdata[i].wave_no;
		mi->tdata[i].pipi = info.tdata[i].pipi;
	}
}
bool OrgData::PutBackGround(void)
{
	if(!MakeMusicParts(info.line,info.dot))return false;//パーツを生成
	MakePanParts(info.line,info.dot);
	//MessageBox(hWnd,"グリッド","",MB_OK);
	return true;
}

//曲情報を設定。flagはアイテムを指定
BOOL OrgData::SetMusicInfo(MUSICINFO *mi,unsigned long flag)
{
	char str[32];
	int i;
	if(flag & SETGRID){//グリッドを有効に
		info.dot = mi->dot;
		info.line = mi->line;
		MakeMusicParts(info.line,info.dot);//パーツを生成
		MakePanParts(info.line,info.dot);
//		MessageBox(hWnd,"グリッド","",MB_OK);

	}
	if(flag & SETALLOC){//領域確保
		info.alloc_note = mi->alloc_note;
		ReleaseNote();
		NoteAlloc(info.alloc_note);
//		MessageBox(hWnd,"唖ロック","",MB_OK);
	}
	if(flag & SETWAIT){
		info.wait = mi->wait;
		itoa(mi->wait,str,10);
		SetDlgItemText(hDlgTrack,IDE_VIEWWAIT,str);
//		MessageBox(hWnd,"うえいと","",MB_OK);
	}
	if(flag & SETREPEAT){
		info.repeat_x = mi->repeat_x;
		info.end_x = mi->end_x;
	}
	if(flag & SETFREQ){
		for(int i = 0; i < MAXMELODY; i++){
			info.tdata[i].freq = mi->tdata[i].freq;
			info.tdata[i].pipi = info.tdata[i].pipi;
		}
	}
	if(flag & SETWAVE){
		for(i = 0; i < MAXTRACK; i++)
			info.tdata[i].wave_no = mi->tdata[i].wave_no;
	}
	if(flag & SETPIPI){
		for(i = 0; i < MAXTRACK; i++)
			info.tdata[i].pipi = mi->tdata[i].pipi;
	}

	return TRUE;
}
//未使用音符を検索
NOTELIST *OrgData::SearchNote(NOTELIST *np)
{
	int i;
	for(i = 0; i < info.alloc_note; i++,np++){
		if(np->length == 0){
			return np;
		}
	}
	return NULL;
}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//以下は音符（キー）の配置、削除
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//音符を配置(左クリックの処理)
void OrgData::TouchKeyboard(unsigned char y)
{
	PlayOrganKey(y,track,info.tdata[track].freq,320);//■
}
BOOL OrgData::SetNote(long x,unsigned char y, int DragMode)
{
	NOTELIST *note;//生成NOTE
	NOTELIST *p;//リストを指すポインター
	NOTELIST *cut_p;//それを置く事によってカットすべき音符
	//未使用NOTEを検索
	if((note = SearchNote(info.tdata[track].note_p)) == NULL)return FALSE;
	//初音符ならリストに登録
	if(info.tdata[track].note_list == NULL){
		PlayOrganKey(y,track,info.tdata[track].freq,100);//■
		info.tdata[track].note_list = note;
		note->from = NULL;
		note->to = NULL;
		note->length = 1;
		note->pan = def_pan[track];
		note->volume = def_volume[track];
		note->x = x;
		note->y = y;
		return TRUE;
	}
	//頭から検索
	p = info.tdata[track].note_list;
	while(p->x < x && p->to != NULL){
		p = p->to;
	}
	//挿入
	if(p->x > x){
		PlayOrganKey(y,track,info.tdata[track].freq,100);//■
		note->to = p;
		note->from = p->from;
		if(p->from == NULL){
			info.tdata[track].note_list = note;//先頭
		}
		else p->from->to = note;//じゃなければ前のやつの次に
		p->from = note;
		note->length = 1;
		note->pan = def_pan[track];
		note->volume = def_volume[track];
		note->x = x;
		note->y = y;
		//カットすべき音符を検索
		cut_p = note->from;
		while(cut_p != NULL && cut_p->y == KEYDUMMY)cut_p = cut_p->from;
		if(cut_p == NULL)return TRUE;
		if(note->x < cut_p->x + cut_p->length)//カット
			cut_p->length = (unsigned char)(note->x - cut_p->x);
	}
	//パラメータ変更
	else if(p->x == x){
		if(p->y == y){
			if(p->length == MAXNOTELENGTH)return FALSE;
			//if(DragMode==0)
			if(iPushStratch)p->length++;//長く
			else p->length = iLastEditNoteLength; //頭を...オプションだったら伸ばすのではなく最後の長さを使う
			//カットすべき音符を検索
			cut_p = p->to;
			while(cut_p != NULL && cut_p->y == KEYDUMMY){
				cut_p = cut_p->to;
			}
			if(cut_p != NULL && p->length + p->x > cut_p->x){//伸ばしすぎると次のを飲み込む
				if(cut_p->to == NULL){//そいつが最後尾なら
					cut_p->from->to = NULL;//一つ前の人に最後尾を任せる。
				}
				else{
					cut_p->from->to = cut_p->to;
					cut_p->to->from = cut_p->from;
				}
				cut_p->length = 0;
				cut_p->from = NULL;
			}
		}else{
			PlayOrganKey(y,track,info.tdata[track].freq,100);//■
			p->y = y;//Ｙ変更
			//カットすべき音符を検索
			cut_p = p->from;
			while(cut_p != NULL && cut_p->y == KEYDUMMY)cut_p = cut_p->from;
			if(cut_p == NULL)return TRUE;
			if(p->x < cut_p->x + cut_p->length)//カット
				cut_p->length = (unsigned char)(p->x - cut_p->x);
		}
	}
	//最後尾追加
	else if(p->to == NULL){
		PlayOrganKey(y,track,info.tdata[track].freq,100);//■
		note->from = p;
		p->to = note;
		note->to = NULL;
		note->length = 1;
		note->pan = def_pan[track];
		note->volume = def_volume[track];
		note->x = x;
		note->y = y;
		//カットすべき音符を検索
		cut_p = note->from;
		while(cut_p != NULL && cut_p->y == KEYDUMMY)cut_p = cut_p->from;
		if(cut_p == NULL)return TRUE;
		if(note->x < cut_p->x + cut_p->length)//カット
			cut_p->length = (unsigned char)(note->x - cut_p->x);
	}
	return TRUE;
}
//音符のカット(右クリックの処理)
BOOL OrgData::CutNote(long x,unsigned char y)
{
	NOTELIST *p;//リストを指すポインター
	//頭から検索
	if(info.tdata[track].note_list == NULL)return FALSE;
	p = info.tdata[track].note_list;
	while(p != NULL && p->x < x)p = p->to;
	if(p == NULL)return FALSE;
	//パラメータ変更
	if(p->x == x && p->y == y){
		if(iPushStratch)p->length--;//短く
		else p->length = 0; //頭を...オプションだったら短くするのではなく消す。
		if(p->length == 0){
			if(p->from == NULL)info.tdata[track].note_list = p->to;
			else p->from->to = p->to;
			if(p->to == NULL);
			else p->to->from = p->from;
		}
		return TRUE;
	}
	return FALSE;
}
//どのトラックにある音符か？の検索(返り血はトラック)
int OrgData::SearchNote(long x, unsigned char y, int YuusenTrack)
{
	NOTELIST *p;//リストを指すポインター
	//頭から検索
	int i;
	int is,ie;

	p = info.tdata[YuusenTrack].note_list;
	while(p != NULL && !(p->x <= x && x < (p->x+p->length)))p = p->to;
	if(p != NULL){
		//hit!!
		if((p->x <= x && x < (p->x+p->length)) &&  p->y == y){
			return YuusenTrack;
		}
	}

	if(track<MAXDRAM){
		is = 0; ie = MAXDRAM;
	}else{
		is  =MAXDRAM; ie = MAXTRACK;
	}
	for(i=ie-1;i>=is;i--){
		if(info.tdata[i].note_list != NULL){
			p = info.tdata[i].note_list;
			//while(p != NULL && p->x < x)p = p->to;
			while(p != NULL && !(p->x <= x && x < (p->x+p->length)))p = p->to;
			if(p != NULL){
				//hit!!
				if((p->x <= x && x < (p->x+p->length)) &&  p->y == y){
					return i;
				}
			}
		}
	}
	return -1;
}
//どのトラックにある音符か？の検索(返り血はトラック)
//上と同じだが、該当する音符の最初と最後をポインタで返す。↓  と       ↓
//YuusenTrack>=0のとき、そのトラックを優先して検索する。
int OrgData::SearchNoteB(long x,unsigned char y, long *lStartx, long *lLastx, int YuusenTrack)
{
	NOTELIST *p;//リストを指すポインター
	//頭から検索
	int i;
	int is,ie;
	if(YuusenTrack>=0){
		p = info.tdata[YuusenTrack].note_list;
		while(p != NULL && !(p->x <= x && x < (p->x+p->length)))p = p->to;
		if(p != NULL){
			//hit!!
			if((p->x <= x && x < (p->x+p->length)) &&  p->y != KEYDUMMY){
				*lStartx = p->x;
				*lLastx  = p->x + p->length - 1;
				return YuusenTrack;
			}
		}
	}
	if(track<MAXDRAM){
		is = 0; ie = MAXDRAM;
	}else{
		is  =MAXDRAM; ie = MAXTRACK;
	}
	for(i=ie-1;i>=is;i--){
		if(info.tdata[i].note_list != NULL){
			p = info.tdata[i].note_list;
			//while(p != NULL && p->x < x)p = p->to;
			while(p != NULL && !(p->x <= x && x < (p->x+p->length)))p = p->to;
			if(p != NULL){
				//hit!!
				if((p->x <= x && x < (p->x+p->length)) &&  p->y == y){
					*lStartx = p->x;
					*lLastx  = p->x + p->length - 1;
					return i;
				}
			}
		}
	}
	return -1;
}

//縮小表示時の検索。
int OrgData::SearchNoteC(long x,unsigned char y, long xWidth, long xMod)
{
	NOTELIST *p;//リストを指すポインター
	//頭から検索
	int i;

	for(p = info.tdata[track].note_list; p != NULL ; p = p->to){
		//if(p != NULL && p->y == y){	// 2014.05.28 D
		if(p != NULL){	// 2014.05.28 A
			for(i = 0; i <= xWidth; i++){
				if( p->x == x - i && x < (p->x + p->length) ){
					return i;
				}
			}
			if(p->x > x + xWidth)break;
		}
	}

	return -1;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//以下は音符（パン）の配置、削除
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//音符を配置(左クリックの処理)
BOOL OrgData::SetPan(long x,unsigned char y)
{
	NOTELIST *note;//生成NOTE
	NOTELIST *p;//リストを指すポインター
	//未使用NOTEを検索
	if((note = SearchNote(info.tdata[track].note_p)) == NULL)return FALSE;
	//初音符ならリストに登録
	if(info.tdata[track].note_list == NULL){
		info.tdata[track].note_list = note;
		note->from = NULL;
		note->to = NULL;
		note->length = 1;
		note->pan = y;
		note->volume = VOLDUMMY;
		note->x = x;
		note->y = KEYDUMMY;
		return TRUE;
	}
	//頭から検索
	p = info.tdata[track].note_list;
	while(p->x < x && p->to != NULL){
		p = p->to;
	}
	//挿入
	if(p->x > x){
		note->to = p;
		note->from = p->from;
		if(p->from == NULL){
			info.tdata[track].note_list = note;//先頭
		}
		else p->from->to = note;//じゃなければ前のやつの次に
		p->from = note;
		note->length = 1;
		note->pan = y;
		note->volume = 255;
		note->x = x;
		note->y = KEYDUMMY;
	}
	//パラメータ変更
	else if(p->x == x){
		p->pan = y;//Ｙ変更
	}
	//最後尾追加
	else if(p->to == NULL){
		note->from = p;
		p->to = note;
		note->to = NULL;
		note->length = 1;
		note->pan = y;
		note->volume = VOLDUMMY;
		note->x = x;
		note->y = KEYDUMMY;
	}
	return TRUE;
}
BOOL OrgData::SetPan2(long x,unsigned char y)
{
	NOTELIST *note;//生成NOTE
	NOTELIST *p;//リストを指すポインター
	//未使用NOTEを検索
	if((note = SearchNote(info.tdata[track].note_p)) == NULL)return FALSE;
	//初音符ならリストに登録
	if(info.tdata[track].note_list == NULL){
		return FALSE;
	}
	//頭から検索
	p = info.tdata[track].note_list;
	while(p->x < x && p->to != NULL){
		p = p->to;
	}
	//挿入
	if(p->x == x){
		if(p->pan != y){ // 2010.08.14 A
			p->pan = y;//Ｙ変更
		}else{
			return FALSE;	// 2010.08.14 A
		}
	}
	return TRUE;
}
//音符のカット(右クリックの処理)
BOOL OrgData::CutPan(long x,unsigned char y)
{
	NOTELIST *p;//リストを指すポインター
	//頭から検索
	if(info.tdata[track].note_list == NULL)return FALSE;
	p = info.tdata[track].note_list;
	while(p != NULL && p->x < x)p = p->to;
	if(p == NULL)return FALSE;
	//パラメータ変更
	if(p->x == x){
		p->length = 0;
		if(p->from == NULL)info.tdata[track].note_list = p->to;
		else p->from->to = p->to;
		if(p->to == NULL);
		else p->to->from = p->from;
	}
	return TRUE;
}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//以下は音符（ボリューム）の配置、削除
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//音符を配置(左クリックの処理)
BOOL OrgData::SetVolume(long x,unsigned char y)
{
	NOTELIST *note;//生成NOTE
	NOTELIST *p;//リストを指すポインター
	//未使用NOTEを検索
	if((note = SearchNote(info.tdata[track].note_p)) == NULL)return FALSE;
	//初音符ならリストに登録
	if(info.tdata[track].note_list == NULL){
		info.tdata[track].note_list = note;
		note->from = NULL;
		note->to = NULL;
		note->length = 1;
		note->pan = PANDUMMY;
		note->volume = y;
		note->x = x;
		note->y = KEYDUMMY;
		return TRUE;
	}
	//頭から検索
	p = info.tdata[track].note_list;
	while(p->x < x && p->to != NULL){
		p = p->to;
	}
	//挿入
	if(p->x > x){
		note->to = p;
		note->from = p->from;
		if(p->from == NULL){
			info.tdata[track].note_list = note;//先頭
		}
		else p->from->to = note;//じゃなければ前のやつの次に
		p->from = note;
		note->length = 1;
		note->pan = PANDUMMY;
		note->volume = y;
		note->x = x;
		note->y = KEYDUMMY;
//		if(note->from != NULL && note->x < note->from->x + note->from->length)
//			note->length = (unsigned char)(note->from->length - (note->x - note->from->x));
	}
	//パラメータ変更
	else if(p->x == x){
		if(p->volume != y){	// 2010.08.14 A
			p->volume = y;//Ｙ変更
		}else{
			return FALSE;
		}
	}
	//最後尾追加
	else if(p->to == NULL){
		note->from = p;
		p->to = note;
		note->to = NULL;
		note->length = 1;
		note->pan = PANDUMMY;
		note->volume = y;
		note->x = x;
		note->y = KEYDUMMY;
//		if(note->from != NULL && note->x < note->from->x + note->from->length)
//			note->length = (unsigned char)(note->from->length - (note->x - note->from->x));
	}
	return TRUE;
}
//
BOOL OrgData::SetVolume2(long x,unsigned char y,long fade)
{
	NOTELIST *note;//生成NOTE
	NOTELIST *p;//リストを指すポインター
	unsigned char lastlength = 1;
	unsigned char vv;
	double dv;
	long lastx = 0;
	int i;
	//未使用NOTEを検索
	if((note = SearchNote(info.tdata[track].note_p)) == NULL)return FALSE;
	//初音符ならリストに登録
	if(info.tdata[track].note_list == NULL){
		info.tdata[track].note_list = note;
		note->from = NULL;
		note->to = NULL;
		note->length = 1;
		note->pan = PANDUMMY;
		note->volume = y;
		note->x = x;
		note->y = KEYDUMMY;
		return TRUE;
	}
	//頭から検索
	p = info.tdata[track].note_list;
	while(p->x < x && p->to != NULL){
		if(p->y != KEYDUMMY){
			lastx = p->x;
			lastlength = p->length;
		}
		p = p->to;
	}
	//挿入
	if(p->x > x){
		note->to = p;
		note->from = p->from;
		if(p->from == NULL){
			info.tdata[track].note_list = note;//先頭
		}
		else p->from->to = note;//じゃなければ前のやつの次に
		p->from = note;
		note->length = 1;
		note->pan = PANDUMMY;
		switch(fade){
		case 0: //直線的減衰
			dv = (double)(p->x - lastx)/(double)lastlength;
			dv = 200 - dv * 200;
			if(dv < 0)dv = 0; else if(dv > 255)dv = 255;
			vv = (unsigned char)dv;
			break;
		case 1: //すぐに減衰
			dv = 200.0f;
			for(i=0;i<(p->x - lastx);i++){
				dv = dv / 2.0f;
			}
			if(dv < 0)dv = 0; else if(dv > 255)dv = 255;
			vv = (unsigned char)dv;
			break;
		case 2: //ゆっくりと減衰
		case 3: //ゆっくりと減衰(ビブラート）
			dv = sqrt(sqrt(sqrt(((double)lastlength-(double)p->x)/(double)lastlength))) * 200;
			i = (p->x - lastx) % 2;
			if(i==1 && fade==3)dv = dv - 10.0;
			if(dv < 0)dv = 0; else if(dv > 255)dv = 255;
			vv = (unsigned char)dv;
			break;
		}
		note->volume = vv;
		note->x = x;
		note->y = KEYDUMMY;
//		if(note->from != NULL && note->x < note->from->x + note->from->length)
//			note->length = (unsigned char)(note->from->length - (note->x - note->from->x));
	}
	//パラメータ変更
	else if(p->x == x){
		if(p->volume != y){	// 2010.08.14 A
			p->volume = y;//Ｙ変更
		}else{
			return FALSE;
		}
	}
	//最後尾追加
	else if(p->to == NULL){
		note->from = p;
		p->to = note;
		note->to = NULL;
		note->length = 1;
		note->pan = PANDUMMY;
		note->volume = y;
		note->x = x;
		note->y = KEYDUMMY;
//		if(note->from != NULL && note->x < note->from->x + note->from->length)
//			note->length = (unsigned char)(note->from->length - (note->x - note->from->x));
	}
	return TRUE;
}
//音符のカット(右クリックの処理)
BOOL OrgData::CutVolume(long x,unsigned char y)
{
	NOTELIST *p;//リストを指すポインター
	//頭から検索
	if(info.tdata[track].note_list == NULL)return FALSE;
	p = info.tdata[track].note_list;
	while(p != NULL && p->x < x)p = p->to;
	if(p == NULL)return FALSE;
	//パラメータ変更
	if(p->x == x){
		p->length = 0;
		if(p->from == NULL)info.tdata[track].note_list = p->to;
		else p->from->to = p->to;
		if(p->to == NULL);
		else p->to->from = p->from;
	}else{
		return FALSE;	// 2010.08.14 A
	}
	return TRUE;
}
//■■■■■■■■■■■■■■■■■■■■■■■■
void OrgData::InitOrgData(void)
{
//	MUSICINFO mi;
	track = 0;
	info.alloc_note = ALLOCNOTE;//とりあえず10000個確保
	info.dot = 4;
	info.line = 4;
	info.wait = 128;
	info.repeat_x = info.dot * info.line * 0;
	info.end_x = info.dot * info.line * 255;
	int i;
	for(i = 0; i < MAXTRACK; i++){
		info.tdata[i].freq = 1000;
		info.tdata[i].wave_no = 0;
		info.tdata[i].pipi = 0;
		def_pan[i] = DEFPAN;
		def_volume[i] = DEFVOLUME;
	}
	SetMusicInfo(&info,SETALL);

	// 以下 2014.05.07 追加
	for(i=0; i<MAXMELODY; i++){
		info.tdata[i].wave_no = i*11;
		MakeOrganyaWave(i, info.tdata[i].wave_no, info.tdata[i].pipi);
	}
	info.tdata[ 8].wave_no = 39;
	info.tdata[ 9].wave_no = 32;
	info.tdata[10].wave_no = 35;
	info.tdata[11].wave_no = 34;
	info.tdata[12].wave_no = 20;
	info.tdata[13].wave_no = 33;
	info.tdata[14].wave_no = 22;
	info.tdata[15].wave_no = 40;
	for(i = MAXMELODY; i < MAXTRACK; i++){
		InitDramObject(dram_name[ info.tdata[i].wave_no ],i-MAXMELODY);
	}
}
void OrgData::GetNoteUsed(long *use,long*left,char track)
{
	NOTELIST *np;
	long u,l;
	int i;
	u = 0;
	l = 0;
	np = info.tdata[track].note_p;
	for(i = 0; i < ALLOCNOTE; i++){
		if(np->length)u++;
		else l++;
		np++;
	}
	*use = u;
	*left = l;
}
OrgData::OrgData()
{
	int i;
	for(i = 0; i < MAXTRACK; i++){
		info.tdata[i].note_list = NULL;
		info.tdata[i].note_p = NULL;
	}
	CurrentUndoCursor = 0;
	MinimumUndoCursor = 0;
	MaximumUndoCursor = 0;
	RedoEnable = false;
	UndoEnable = false;
	//noteon = new unsigned char[65536];
}
OrgData::~OrgData() //デストラクタ
{
	//delete [] noteon;
}



int OrgData::SetUndoData()
{
	int j,cc;
	cc = (CurrentUndoCursor % 32);
	memcpy(&ud_tdata[cc] , &info, sizeof(MUSICINFO));
	for(j=0;j<16;j++){
		memcpy(&ud_note[cc][j] , info.tdata[j].note_p , sizeof(NOTELIST)*4096);
	}
	CurrentUndoCursor++;
	MaximumUndoCursor = CurrentUndoCursor;
	if(CurrentUndoCursor - 32+1 > MinimumUndoCursor)MinimumUndoCursor = CurrentUndoCursor - 32+1;
	
	if(!UndoEnable){
		UndoEnable = true;
		return 1;
	}
	if(RedoEnable){
		RedoEnable = false;
		return 2;
	}
	return 0;
}

void OrgData::ClearUndoData()
{
	CurrentUndoCursor = 0;
	MinimumUndoCursor = 0;
	MaximumUndoCursor = 0;
	RedoEnable = false;
	UndoEnable = false;
}

//アンドゥから復帰
int OrgData::ReplaceFromUndoData()
{
	if(!UndoEnable)return 1;
	if(MaximumUndoCursor == CurrentUndoCursor){
		SetUndoData();
		CurrentUndoCursor--;
	}
	int j,cc,r;
	CurrentUndoCursor--;
	r=0;
	RedoEnable = true;
	if(CurrentUndoCursor <= MinimumUndoCursor){
		CurrentUndoCursor = MinimumUndoCursor;
		r=2; //もうこれ以上元に戻せないぞ。
		UndoEnable = false;
	}
	cc = (CurrentUndoCursor % 32);
	memcpy(&info, &ud_tdata[cc] ,  sizeof(MUSICINFO));
	for(j=0;j<16;j++){
		memcpy(info.tdata[j].note_p , &ud_note[cc][j] ,  sizeof(NOTELIST)*4096);
	}
	return r;
}

//リドゥ
int OrgData::ReplaceFromRedoData()
{
	if(!RedoEnable)return 1;
	int j,cc,r;
	CurrentUndoCursor++;
	r=0;
	UndoEnable = true;
	if(CurrentUndoCursor >= MaximumUndoCursor-1){
		CurrentUndoCursor = MaximumUndoCursor-1;
		r=2; //もうこれ以上元に戻せないぞ。
		RedoEnable = false;
	}
	cc = (CurrentUndoCursor % 32);
	memcpy(&info, &ud_tdata[cc] ,  sizeof(MUSICINFO));
	for(j=0;j<16;j++){
		memcpy(info.tdata[j].note_p , &ud_note[cc][j] ,  sizeof(NOTELIST)*4096);
	}
	return r;

}


int OrgData::ResetLastUndo()
{
	int r = 0;
	CurrentUndoCursor--;
	MaximumUndoCursor = CurrentUndoCursor;
	if(CurrentUndoCursor <= MinimumUndoCursor){
		CurrentUndoCursor = MinimumUndoCursor;
		r=2; //もうこれ以上元に戻せないぞ。
		UndoEnable = false;
	}
	return r;

}

//最後の判定をしない
BOOL OrgData::SetNote_onlyLength(long x, long Length)
{
	NOTELIST *note;//生成NOTE
	NOTELIST *p;//リストを指すポインター
	//未使用NOTEを検索
	if((note = SearchNote(info.tdata[track].note_p)) == NULL)return FALSE;
	if(info.tdata[track].note_list == NULL)return FALSE;
	p = info.tdata[track].note_list;
	while(p->x < x && p->to != NULL){
		p = p->to;
	}
	if(p->x == x){
		if(Length<MAXNOTELENGTH){
			if(p->length != (char)Length){ //A 2010.08.14 
				p->length = (char)Length;
			}else{
				return FALSE; //A 2010.08.14 
			}
		}
	}
	return TRUE ;
}

BOOL OrgData::SetNote_afterSetLength(long x)
{
	NOTELIST *note;//生成NOTE
	NOTELIST *p;//リストを指すポインター
	NOTELIST *cut_p;//それを置く事によってカットすべき音符
	//未使用NOTEを検索
	if((note = SearchNote(info.tdata[track].note_p)) == NULL)return FALSE;
	if(info.tdata[track].note_list == NULL)return FALSE;
	p = info.tdata[track].note_list;
	while(p->x < x && p->to != NULL){
		p = p->to;
	}
	if(p->x == x){
		cut_p = p->to;
		while(cut_p != NULL && cut_p->y == KEYDUMMY){
			cut_p = cut_p->to;
		}
		if(cut_p != NULL && p->length + p->x > cut_p->x){//伸ばしすぎると次のを飲み込む
			if(cut_p->to == NULL){//そいつが最後尾なら
				cut_p->from->to = NULL;//一つ前の人に最後尾を任せる。
			}
			else{
				cut_p->from->to = cut_p->to;
				cut_p->to->from = cut_p->from;
			}
			cut_p->length = 0;
			cut_p->from = NULL;
		}

	}
	return TRUE;

}

