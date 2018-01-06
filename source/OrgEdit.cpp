#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Setting.h"
#include "Gdi.h"
#include "DefOrg.h"
#include "OrgData.h"
#include "rxoFunction.h"
#define PI 3.14159265358979323846
/*
//編集用構造体///////////////
typedef struct{//●コピー構造体
	char track1;//このトラックの
	long x1_1;//ここから
	long x1_2;//ここまでを
	char track2;//このトラックの
	long x2;//ここに
	long num;//何回
}NOTECOPY;

typedef struct{//●トランスポートやパンポット
	char track;//このトラックの
	long x1;//ここから
	long x2;//ここまでを
	unsigned char a;//これだけ 
	unsigned char mode;//たす（ひく）
}PARCHANGE;
*/
BOOL OrgData::DelateNoteData(PARCHANGE *pc)
{
	NOTELIST *np;
	NOTELIST *np_from;//こいつと
	NOTELIST *np_to;//こいつを繋ぐ
	NOTELIST *np_clear;//尻尾削除用
	long note_x1,note_x2;//曲の範囲を保存
	np = info.tdata[pc->track].note_list;
	if(np == NULL)return FALSE;//音符が無ければ終了

	np = info.tdata[pc->track].note_list;
	note_x1 = np->x;//最初のｘを記録
	while(np->to != NULL)np = np->to;//おわりを検索
	note_x2 = np->x;//おわりを保存
	if(note_x1 > pc->x2 || pc->x1 > note_x2){//範囲が楽譜から離れている
		return FALSE;
	}else if(note_x1 < pc->x1 && pc->x2 < note_x2){//範囲が楽譜に内包されている。
		np = info.tdata[pc->track].note_list;
		while(np->x < pc->x1)np = np->to;//範囲の頭を検索
		np_from = np->from;//左端を保存(接続用)
		while(np->x <= pc->x2){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;//範囲の尻を検索
		}
		np_to = np;//右端を保存(接続用)
		//両端を接続
		np_from->to = np_to;
		np_to->from = np_from;
		return TRUE;

	}else if(pc->x1 <= note_x1 && pc->x2 < note_x2){//消す範囲の頭に音が存在しない。
		np = info.tdata[pc->track].note_list;
		while(np->x <= pc->x2){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;//範囲の尻を検索
		}
		np_to = np;//右端を保存(接続用)
		np->from = NULL;//こいつを頭にする
		info.tdata[pc->track].note_list = np;//リストに登録
		return TRUE;
	}else if(note_x1 < pc->x1 && note_x2 <= pc->x2 ){//消す範囲の尻に音が存在しない。
		np = info.tdata[pc->track].note_list;
		while(np->to->x < pc->x1)np = np->to;//範囲の頭を検索
		np_clear = np;//最後になる音譜のアドレス
		np = np->to;
		while(np != NULL){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;
		}
		np_clear->to = NULL;
		return TRUE;
	}else if(note_x1 >= pc->x1 && note_x2 <= pc->x2 ){//完全消去
		np = info.tdata[pc->track].note_list;
		while(np != NULL){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;
		}
		info.tdata[pc->track].note_list = NULL;
		return TRUE;
	}
	return TRUE;
}

BOOL OrgData::CopyNoteData(NOTECOPY *nc)
{
	int i,j;
	PARCHANGE pc;//ペースト領域クリア用
	NOTELIST *np;
//	NOTELIST *p_in1,*p_in2;//挿入すべき場所
	NOTELIST *p_list1,*p_list2;//挿入すべきリスト
	NOTELIST *work;//ワークエリア
	NOTELIST *wp;//ワークポインタ
	long ind_x;//インデックス値（x)
	long copy_num;//コピーする音譜の数
	if(nc->num == 0)return FALSE;//コピー回数０
	copy_num = GetNoteNumber(nc->track1,nc);
	if(copy_num == 0)return FALSE;
	wp = work = (NOTELIST *)malloc(sizeof(NOTELIST)*copy_num);//ワーク用に領域を確保
	ind_x = nc->x1_1;
	
	//ワーク領域にコピー
	np = info.tdata[nc->track1].note_list;
	while(np != NULL && np->x < nc->x1_1)np = np->to;
	if(np == NULL){
		free( work );
		return TRUE;//コピー元に音譜無し(頭を検索中にOUT)
	}
	for(i = 0; i < copy_num; i++){//１ペースト
		wp->length = np->length;
		wp->pan    = np->pan;
		wp->volume = np->volume;
		wp->y      = np->y;
		//インデックス値は差し引く
		wp->x      = np->x - ind_x;
		//次へ
		np = np->to;
		wp++;
	}

	//ペースト先をクリア
	pc.track = nc->track2;//このトラックの
	pc.x1 = nc->x2;//ここから
	pc.x2 = nc->x2 + (nc->x1_2+1 - nc->x1_1)*nc->num-1;//ここまでをx1_1+1(x1_2は－１した値)
	DelateNoteData(&pc);//消す

	//ペーストリストの生成
	np = p_list1 = p_list2= SearchNote(info.tdata[nc->track2].note_p);
	np->length = 1;//仮生成
	if(np == NULL){//未使用音譜が不足
		free( work );
		return FALSE;
	}
	for(i = 1; i < copy_num*nc->num; i++){
		np = SearchNote(info.tdata[nc->track2].note_p);
		if(np == NULL){//未使用音譜が不足
			free( work );
			return FALSE;
		}
		np->length = 1;//仮生成
		p_list2->to = np;
		np->from = p_list2;
		p_list2 = np;
	}

	long index;
	//ペーストリストにペースト
	np = p_list1;
	for(j = 0; j < nc->num;j++){//ペースト回数
		wp = work;
		index = nc->x2+ (nc->x1_2 - nc->x1_1 +1)*j;
		for(i = 0; i < copy_num; i++){//１ペースト
			np->length    = wp->length;
			np->pan       = wp->pan;
			np->volume    = wp->volume; 
			np->y         = wp->y; 
			//インデックス値を加える
			np->x         = wp->x + index;

			//次へ
			np = np->to;
			wp++;
		}
	}



	np = info.tdata[nc->track2].note_list;
	if(np == NULL){//完全新規
		info.tdata[nc->track2].note_list = p_list1;
		p_list1->from = NULL;
		p_list2->to = NULL;
	}else{
		//挿入すべき場所を検出
//		np = info.tdata[nc->track2].note_list;
		if(np->x > p_list2->x){//頭に追加
			np->from = p_list2;
			p_list2->to = np;
			p_list1->from = NULL;
			info.tdata[nc->track2].note_list = p_list1;
			free( work );
			return TRUE;
		}
		while(np->to != NULL && np->to->x < nc->x2)np = np->to;
		if(np->to == NULL){//尻に追加
			np->to = p_list1;
			p_list1->from = np;
			p_list2->to = NULL;
			free( work );
			return TRUE;
		}else{//挿入
			np->to->from = p_list2;
			p_list2->to = np->to;
			np->to = p_list1;
			p_list1->from = np;
			delete work;
			return TRUE;
		}

	}
	free( work );
	return TRUE;
}
//パンは最小で０最大が１２となる
BOOL OrgData::ChangePanData(PARCHANGE *pc)
{
	int i;
	long num = 0;
	NOTECOPY nc;
	NOTELIST *np;
	//音譜の数を検出
	nc.x1_1 = pc->x1;
	nc.x1_2 = pc->x2;
	num = GetNoteNumber(pc->track,&nc);

	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	while(np != NULL && np->x < pc->x1)np = np->to;
	if(pc->a == 254){ //2014.05.03 逆転
		for(i = 0; i < num; i++){
			if(np->pan == PANDUMMY);
			else np->pan = 12 - np->pan;
			np = np->to;
		}
		
	}else{
		if(pc->mode == MODEPARADD){//加算モード
			for(i = 0; i < num; i++){
				if(np->pan == PANDUMMY);
				else if(np->pan + pc->a > 12)np->pan = 12;
				else np->pan += pc->a;
				np = np->to;
			}
		}else{//減算モード
			for(i = 0; i < num; i++){
				if(np->pan == PANDUMMY);
				else if(np->pan - pc->a < 0)np->pan = 0;
				else np->pan -= pc->a;
				np = np->to;
			}
		}
	}
	return TRUE;
}

//キーは95がMAX
BOOL OrgData::ChangeTransData(PARCHANGE *pc)
{
	int i;
	long num = 0;
	NOTECOPY nc;
	NOTELIST *np;
	//音譜の数を検出
	nc.x1_1 = pc->x1;
	nc.x1_2 = pc->x2;
	num = GetNoteNumber(pc->track,&nc);

	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	while(np != NULL && np->x < pc->x1)np = np->to;
	if(pc->mode == MODEPARADD){//加算モード
		for(i = 0; i < num; i++){
			if(np->y == KEYDUMMY);
			else if(np->y + pc->a > 95)np->y = 95;
			else np->y += pc->a;
			np = np->to;
		}
	}else{//減算モード
		for(i = 0; i < num; i++){
			if(np->y == KEYDUMMY);
			else if(np->y - pc->a < 0)np->y = 0;
			else np->y -= pc->a;
			np = np->to;
		}
	}
	return TRUE;
}
//ヴォリュームは254がMAX
BOOL OrgData::ChangeVolumeData(PARCHANGE *pc)
{
	int i,j;
	long num = 0, lFirstx, lLastx;
	NOTECOPY nc;
	NOTELIST *np;
	unsigned char uc, ucMax = 0, ucMin = 254, ucFirstFlg = 255, ucFirst, ucLast;
	double d, dnum, di, dnorm = 0, dnormnum = 0, dt;
	//音譜の数を検出
	nc.x1_1 = pc->x1;
	nc.x1_2 = pc->x2;
	num = GetNoteNumber(pc->track,&nc);
	dnum = num - 1.0f;

	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	
	//頭だし
	while(np != NULL && np->x < pc->x1)np = np->to;
	//ucMax, ucMinを調べる
	for(i = 0; i < num; i++){
		if(np->volume != VOLDUMMY){
			if(ucFirstFlg == 255){ucFirst = np->volume; lFirstx = np->x; ucFirstFlg = 0;}
			ucLast = np->volume;
			lLastx = np->x;
			if(np->volume > ucMax)ucMax = np->volume; else if(np->volume < ucMin)ucMin = np->volume;
			dnorm += (double)np->volume;
			dnormnum += 1.0f;
		}
		np = np->to;
	}
	if(dnormnum > 0)dnorm = dnorm / dnormnum; //平均値

	//巻き戻し と 頭だし
	np = info.tdata[pc->track].note_list;
	while(np != NULL && np->x < pc->x1)np = np->to;


	if(pc->mode == MODEPARADD){//加算モード
		for(i = 0; i < num; i++){
			if(np->volume == VOLDUMMY);
			else if(np->volume + pc->a > 254)np->volume = 254;
			else np->volume += pc->a;
			np = np->to;
		}
	}else if(pc->mode == MODEPARSUB){//減算モード
		for(i = 0; i < num; i++){
			if(np->volume == VOLDUMMY);
			else if(np->volume - pc->a < 0)np->volume = 0;
			else np->volume -= pc->a;
			np = np->to;
		}
	}else if(pc->mode == MODEMULTIPLY){//乗算モード
		// 既存のvolumeを a / 128倍 する
		for(i = 0; i < num; i++){
			d = (double)np->volume * (double) pc->a / (double)128.0;
			if(d>254.0)d=254.0; else if(d<0)d=0;

			uc = (unsigned char)d;

			if(np->volume == VOLDUMMY);
			else np->volume = uc;
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 3)){ //線形減衰 2014.05.01
		for(i = 0; i < num; i++){
			if(np->volume != VOLDUMMY){
				d = ((double)ucMax - (double)0) / (double)(num - 0) * (double)(num - 0 - i); if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 12)){ //２乗上昇 2014.05.17
		for(i = 0; i < num; i++){
			if(np->volume != VOLDUMMY){
				if(num > 1){
					d = (double)(ucMax) / (double)((num - 1)*(num - 1))  * (double)(i*i); if(d>254.0)d=254.0; else if(d<0)d=0;
				}else{
					d = ucMax;
				}
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 13)){ //線形上昇 2014.05.17
		for(i = 0; i < num; i++){
			if(np->volume != VOLDUMMY){
				if(num > 1){
					d = (double)ucMax / (double)(num - 1) * (double)i; if(d>254.0)d=254.0; else if(d<0)d=0;
				}else{
					d = ucMax;
				}
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 2)){ //上に凸減衰 2014.05.01
		if(num < 4)dnum = (double)num;
		else if(num == 4)dnum = (double)num-0.5f;
		for(i = 0; i < num; i++){
			di = (double)i;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = 1.0f - ((exp( (di/dnum)*(di/dnum) ) - 1.0f) / 1.718281828459045);
				}else d = 1.0f;
				d = d * (double)ucMax; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 1)){ //上に凸減衰 2014.05.01
		if(num < 4)dnum = (double)num-0.4f;
		else if(num == 4)dnum = (double)num-0.5f;
		else dnum = num - 0.8f;
		for(i = 0; i < num; i++){
			di = (double)i;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = 1.0f - ((exp( (di/dnum)*(di/dnum)*(di/dnum)*(di/dnum) ) - 1.0f) / 1.718281828459045);
				}else d = 1.0f;
				d = d * (double)ucMax; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 4)){ //下に凸減衰 2014.05.01
		for(i = 0; i < num; i++){
			di = (double)i; dnum = (double)num;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = (exp( pow(((dnum-di)/dnum), 2)) - 1.0f) / 1.718281828459045;
				}else d = 1.0f;
				d = d * (double)ucMax; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 5)){ //下に凸減衰 2014.05.01
		for(i = 0; i < num; i++){
			di = (double)i; dnum = (double)num;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = (exp( pow(((dnum-di)/dnum), 4)) - 1.0f) / 1.718281828459045;
				}else d = 1.0f;
				d = d * (double)ucMax; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 6)){ //ノーマライズ 2014.05.01
		for(i = 0; i < num; i++){
			//di = (double)i; dnum = (double)num;
			if(np->volume != VOLDUMMY){
				//np->volume = (unsigned char)dnorm;
				np->volume = org_data.def_volume[pc->track];//初期化
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 11)){ //グラデーション 2014.05.01
		for(i = 0; i < num; i++){
			di = (double)i; dnum = (double)num;
			if(np->volume != VOLDUMMY && num > 1){
				d = ((double)ucFirst - (double)ucLast) / (double)(num - 1) * (double)(num - 1 - i) + (double)ucLast ;
				if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 8)){ //最後で減衰
		dt = (double)np->volume;
		j = lLastx - lFirstx; 
		for(i = 0; i < num; i++){
			di = (double)i; dnum = (double)num;
			if(np->volume != VOLDUMMY){
				//d = dt;
				switch(j){
				case 0:
					d = dt;
					break;
				case 1:
					if     (lFirstx + 0 == np->x)d = dt;
					else if(lFirstx + 1 == np->x)d = dt * 0.92;
					break;
				case 2:
					if     (lFirstx + 0 == np->x)d = dt;
					else if(lFirstx + 1 == np->x)d = dt * 0.95;
					else if(lFirstx + 2 == np->x)d = dt * 0.87;
					break;
				case 3:
					if     (lFirstx + 0 == np->x)d = dt;
					else if(lFirstx + 1 == np->x)d = dt * 0.95;
					else if(lFirstx + 2 == np->x)d = dt * 0.90;
					else if(lFirstx + 3 == np->x)d = dt * 0.75;
					break;
				default:
					if     (lLastx - 2 == np->x)d = dt * 0.92;
					else if(lLastx - 1 == np->x)d = dt * 0.87;
					else if(lLastx - 0 == np->x)d = dt * 0.55;
					else d = dt;
					break;

				}

				if(d>254.0)d=254.0; else if(d<0)d=0;

				np->volume = (unsigned char)d;
				if(d == dt) dt = (double)np->volume;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 9)){ //ビブラート(ベースは上に凸減衰パターン) 2014.05.01
		dt = 1.02;
		for(i = 0; i < num; i++){
			di = (double)i; dnum = (double)num - 0.8f;
			dt += 0.0075; if(dt>1.3)dt=1.3;
			if(np->volume != VOLDUMMY && num > 1){
				if(dnum > 0){
					d = 1.0f - ((exp( (di/dnum)*(di/dnum) ) - 1.0f) / 1.718281828459045);
				}else d = 1.0f;
				d = d * (double)ucMax; 
				if(i%2==0)d=d*dt;else d=d/dt;
				
				if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 10)){ //ランダム 2014.05.15
		for(i = 0; i < num; i++){
			double x, y;
			double s, t;
			double r1, r2;
			di = (double)i; dnum = (double)num;
			if(np->volume != VOLDUMMY){
				// 一様分布に従う擬似乱数 x, y を作る
				do { x = (double)rand()/(double)RAND_MAX; } while (x == 0.0); // x が 0 になるのを避ける
				y = (double)rand()/(double)RAND_MAX;
    
				// s, t の計算
				s = sqrt(-2.0 * log(x)); // C, Java では log が自然対数
				t = 2.0 * PI * y;
    
				// 標準正規分布に従う擬似乱数 r1, r2
				r1 = s * cos(t);  r2 = s * sin(t);
				double dval = (double)np->volume + (i%2==1 ? r1 : r2) * 12.0;
				if(dval < 0)dval = 0; else if(dval > 250)dval = 250.0f;

				np->volume = (unsigned char)dval;
		
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 7)){ //χのようなＳ字曲線 2014.05.15
		//if(num < 4)dnum = (double)num;
		//else if(num == 4)dnum = (double)num-0.5f;
		dnum = (double)num / 2.0;
		long halfnum = (num+1) / 2; if (halfnum <= 0) halfnum = 1;

		for(i = 0; i < halfnum; i++){ //上に凸
			di = (double)i;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = 1.0f - ((exp( (di/dnum)*(di/dnum) ) - 1.0f) / 1.718281828459045);
				}else d = 1.0f;
				d = d * ((double)ucMax / 2.0) + ((double)ucMax / 2.0); if( d > 254.0) d = 254.0; else if(d < 0)d = 0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}

		for(i = halfnum; i < num; i++){ //下に凸
			di = (double)(i - halfnum); dnum = (double)num / 2.0;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = (exp( pow(((dnum-di)/dnum), 2)) - 1.0f) / 1.718281828459045;
				}else d = 1.0f;
				d = d * (double)ucMax / 2.0; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}
	return TRUE;
}

char *test[16]={
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"11",
	"12",
	"13",
	"14",
	"15",
};
//音譜の尻尾を総チェック
BOOL OrgData::CheckNoteTail(char tr)
{
	NOTELIST *np;
	NOTELIST *tail,*head;
	np = info.tdata[tr].note_list;
//	MessageBox(hWnd,test[track],test[track],MB_OK);
	while(np != NULL){
		if(np->y != KEYDUMMY){//長さのある音譜なら
			tail = np;
			head = np->to;
			while(head != NULL && head->y == KEYDUMMY){
	//			MessageBox(hWnd,test[track],test[track],MB_OK);
				head = head->to;
			}
			if(head == NULL)return TRUE;
			else{
				if(tail->x + tail->length > head->x){
					tail->length = (unsigned char)(head->x - tail->x);
				}
//				np = head->from;// =to を見越して
			}
		}
		np = np->to;
	}
	return TRUE;	
}

//[新]音符をPower倍に引き伸ばす
BOOL OrgData::EnlargeAllNotes(int Power)
{
	if(Power<=0)return FALSE;
	int i,j;
	NOTELIST *np;
	for(i=0;i<16;i++){
		np = info.tdata[i].note_list; //初めの音符
		if(np != NULL){
			while(np != NULL){
				j = np->x * Power;
				np->x = j; //倍しマース
				j = np->length * Power;
				if(j>255)j=255;
				if(info.tdata[i].pipi != 0)j = np->length;
				if(np->y != KEYDUMMY && i < MAXMELODY) np->length = (unsigned char)j; //2014.05.02 Modify
				np = np->to;
			}
		}
		
	}
	for(i=0;i<16;i++){ //念のため尻尾を見とくか。
		CheckNoteTail(i);
	}
	info.wait = info.wait / Power; if(info.wait <=0) info.wait = 1;
	info.repeat_x = info.repeat_x * Power;
	info.end_x = info.end_x * Power;
	j = info.dot * Power;
	if(j<=255)info.dot = (unsigned char)j;
	MakeMusicParts(info.line,info.dot);//パーツを生成
	MakePanParts(info.line,info.dot);

	return TRUE;
}
//[新]音符を1/Power倍に縮こめる
BOOL OrgData::ShortenAllNotes(int Power)
{
	if(Power<=1)return FALSE;
	int i,j,k;
	NOTELIST *np, *np2;
	for(i=0;i<16;i++){
		np = info.tdata[i].note_list; //初めの音符
		if(np != NULL){
			while(np != NULL){
				np2 = np->to;
				k = np->x % Power; //これが0でないときは無条件で消すよ。
				if(k==0){
					j = np->x / Power;			
					np->x = j; //倍しマース
					j = np->length / Power;
					//長さ１の音符はかわいそうだ残してやろう。
					if(np->length != 1) np->length = (unsigned char)j;
				}else{
					//音符を消すよ。
					PARCHANGE p;
					p.track = i;
					p.x1 = np->x;
					p.x2 = p.x1 ;
					DelateNoteData(&p);
				}
				np = np2;
			}
		}
		
	}
	for(i=0;i<16;i++){ //念のため尻尾を見とくか。
		CheckNoteTail(i);
	}

	info.wait = info.wait * Power;
	 
	info.repeat_x = info.repeat_x / Power;
	info.end_x = info.end_x / Power;

	j = info.dot / Power;
	if(j>=1)info.dot = (unsigned char)j;
	MakeMusicParts(info.line,info.dot);//パーツを生成
	MakePanParts(info.line,info.dot);

	return TRUE;
	
}

//[新]空白を埋める 2014.05.01
BOOL OrgData::EnsureEmptyArea(PARCHANGE *pc, int Function)
{
	int j, iLength, iFlg;
	int iBufMax;
	long num = 0, tmpx;
	NOTECOPY nc;
	NOTELIST *np, *p, *npnext;
	PARCHANGE tmpc;

	nc.x1_1 = pc->x1; nc.x1_2 = pc->x2; num = GetNoteNumber(pc->track,&nc);
	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	
	iBufMax =  (pc->x2 - pc->x1 + 4096);

	while(np != NULL && np->x < pc->x1)np = np->to;

	for(;np != NULL && np->x <= pc->x2; np = npnext){
		iLength = np->length;
		npnext = np->to;

		for(j = 2; j <= iLength; j++){
			iFlg = 1;
			tmpx = np->x + j - 1;
			if(tmpx >= pc->x1  &&  tmpx <= pc->x2){
				for(p = np; p!= NULL && p->x <= np->x+iLength-1; p = p->to){
					if(p->x == tmpx){
						iFlg = 0;
						break;
					}
				}
				if(Function == 8 && iFlg == 1 && tmpx < np->x + iLength - 3)iFlg = 0; //最後でなければわざわざ追加しなくてもいいでしょう。
				if(Function < 20){
					if(iFlg == 1){
						if(FALSE == SetVolume(tmpx, np->volume)){
							return FALSE;
						}
					}
				}else if(Function == 20){
					if(iFlg == 0){
						if(FALSE == CutVolume(tmpx, 0)){ //第二引数は意味ないと思う。
							return FALSE;
						}
					}

				}
			}
		}
		if(Function >= 1 && Function < 20){
			tmpc.track = pc->track;  tmpc.x1 = np->x;  tmpc.x2 = np->x + np->length - 1;  tmpc.a = 1;  
			tmpc.mode = (unsigned char)(MODEDECAY + Function);
			if(np->y!=KEYDUMMY)ChangeVolumeData(&tmpc);
		}else if(Function == 20){ //ｸﾘｱ
			//何もしない
		}
	}
	return TRUE;
}

//iFullTrack==1のときはiTrackは無視。
bool OrgData::CopyNoteDataToCB(NOTECOPY *nc, int iTrack, int iFullTrack )
{
	/*
	int i,j,t,n;
	PARCHANGE pc;//ペースト領域クリア用
	NOTELIST *np;
//	NOTELIST *p_in1,*p_in2;//挿入すべき場所
	NOTELIST *p_list1,*p_list2;//挿入すべきリスト
	NOTELIST *work;//ワークエリア
	NOTELIST *wp;//ワークポインタ
	long ind_x;//インデックス値（x)
	long copy_num;//コピーする音譜の数

	ClearVirtualCB();
	AddStartToVirtualCB(); //クリップボードに代入開始
	AddIntegerToVirtualCB(iFullTrack);
	AddIntegerToVirtualCB(iTrack);
	AddIntegerToVirtualCB(nc->x1_1); //ここから
	AddIntegerToVirtualCB(nc->x1_2); //ここまで

	//あとはとにかく、クリップボードに内容を移す

	ind_x = nc->x1_1;

	for(t=0;t<MAXTRACK;t++){
		if(iFullTrack==1 || iTrack==t){
			copy_num = GetNoteNumber(t,nc);//その範囲に使われている音符の数を検出
			AddIntegerToVirtualCB(copy_num); //音符は何個？

			if(copy_num == 0){
				AddTrackSeparater(); //＠を挿入
				continue;
			}

			//wp = work = (NOTELIST *)malloc(sizeof(NOTELIST)*copy_num);//ワーク用に領域を確保
			//ワーク領域にコピー
			np = info.tdata[nc->track1].note_list;
			while(np != NULL && np->x < nc->x1_1)np = np->to;
			if(np == NULL){
				//free( work );
				AddTrackSeparater(); //＠を挿入
				continue;//コピー元に音譜無し(頭を検索中にOUT)...そんな状況あるのか？
			}
			for(i = 0; i < copy_num; i++){//１ペースト
				//wp->length = np->length;
				//wp->pan    = np->pan;
				//wp->volume = np->volume;
				//wp->y      = np->y;
				//インデックス値は差し引く
				//wp->x      = np->x - ind_x;
				//次へ
				AddIntegerToVirtualCB((int)np->length);
				AddIntegerToVirtualCB((int)np->pan);
				AddIntegerToVirtualCB((int)np->volume);
				AddIntegerToVirtualCB((int)np->y);
				n = np->x - ind_x;
				AddIntegerToVirtualCB(n);
				np = np->to;
				//wp++;
			}
			//free( work );
			AddTrackSeparater(); //＠を挿入
		}
	}
	SetClipBoardFromVCB(); //真のクリップボードに
*/
	return true;
}


BOOL OrgData::SwapTrack(NOTECOPY *pc)
{
	TRACKDATA tmp;
	
	memcpy(&tmp, &info.tdata[ pc->track1 ], sizeof(TRACKDATA));
	memcpy(&info.tdata[ pc->track1 ], &info.tdata[ pc->track2 ], sizeof(TRACKDATA));
	memcpy(&info.tdata[ pc->track2 ], &tmp, sizeof(TRACKDATA));

	unsigned char uctmp;
	uctmp = def_pan[pc->track1];
	def_pan[pc->track1] = def_pan[pc->track2];
	def_pan[pc->track2] = uctmp;

	uctmp = def_volume[pc->track1];
	def_volume[pc->track1] = def_volume[pc->track2];
	def_volume[pc->track2] = uctmp;

	//char ctmp;
	//ctmp = mute[pc->track1];
	//mute[pc->track1] = mute[pc->track2];
	//mute[pc->track2] = ctmp;


	return TRUE;
}