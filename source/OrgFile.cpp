#include <string.h>
#include "Setting.h"
#include "Gdi.h"
#include "DefOrg.h"
#include "OrgData.h"
#include <stdio.h>
#include "Sound.h"
#include "resource.h"
#include "Scroll.h"
#include "rxoFunction.h"

//曲のPATHはここに置いておく
char music_file[MAX_PATH]; //NewData.org
char pass[7] = "Org-01";
char pass2[7] = "Org-02";//Pipi
char pass3[7] = "Org-03";//勝手に各種音色追加。

////以下はオルガーニャ曲データ構造体（フィルに含まれる項目）
typedef struct{
	long x;//位置
	unsigned char y;//音の高さ
	unsigned char length;//音の長さ
	unsigned char volume;//ヴォリューム
	unsigned char pan;//パン
}ORGANYANOTE;
typedef struct{
	unsigned short freq;//+α周波数(1000がDefault)
	unsigned char wave_no;//波形No
	unsigned char pipi;//☆
	unsigned short note_num;//音符の数
}ORGANYATRACK;
typedef struct{
	unsigned short wait;
	unsigned char line;
	unsigned char dot;
	long repeat_x;//リピート
	long end_x;//曲の終わり(リピートに戻る)
	ORGANYATRACK tdata[MAXTRACK];
}ORGANYADATA;


//そのトラックに使われている音符の数を検出
unsigned short OrgData::GetNoteNumber(char track,NOTECOPY *nc)
{
	NOTELIST *np;
	unsigned short num = 0;
	np = info.tdata[track].note_list;
	if(nc == NULL){
		while(np != NULL){
			num++;
			np = np->to;
		}
		return num;
	}else{
		//範囲までとぶ
		while(np != NULL && np->x < nc->x1_1){
			np = np->to;
		}
		if(np == NULL)return 0;
		//範囲を超えたら(最大Xが同じ物も検出)
		while(np != NULL && np->x <= nc->x1_2){
			num++;
			np = np->to;
		}
		return num;
	}
}
//曲データを保存
BOOL OrgData::SaveMusicData(void)
{
	ORGANYADATA org_data;
	NOTELIST *np;
	int i,j;
	//記録用曲情報の生成
	org_data.wait = info.wait;
	org_data.line = info.line;
	org_data.dot = info.dot;
	org_data.repeat_x = info.repeat_x;
	org_data.end_x = info.end_x;

	for(i = 0; i < MAXTRACK; i++){
		org_data.tdata[i].freq = info.tdata[i].freq;
		org_data.tdata[i].wave_no = info.tdata[i].wave_no;
		org_data.tdata[i].pipi = info.tdata[i].pipi;
		org_data.tdata[i].note_num = GetNoteNumber(i,NULL);
	}
	//｣｣｣｣｣｣｣｣｣｣｣｣｣｣｣セーブ
	FILE *fp;
	if((fp=fopen(music_file,"wb"))==NULL){
		//MessageBox(hWnd,"ファイルにアクセスできません","Error (Save)",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_WARNING_ACCESS_FILE,IDS_ERROR_SAVE,MB_OK);	// 2014.10.19 A
		return(FALSE);
	}
	//パスワード
	j=2;
	for(i=8;i<15;i++){
		if(org_data.tdata[i].wave_no>=12)j=3;	//ドラムの新しい音を使っていればVer.3
	}
	if(j==2)fwrite(&pass2[0], sizeof(char), 6, fp);
	else fwrite(&pass3[0], sizeof(char), 6, fp);
	//曲情報の書き込み
	fwrite(&org_data, sizeof(ORGANYADATA), 1, fp);
	//音符の保存
	for(j = 0; j < MAXTRACK; j++){
		if(info.tdata[j].note_list == NULL)continue;
		np = info.tdata[j].note_list;//Ｘ座標
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->x,      sizeof(long), 1, fp);
			np = np->to;
		}
		np = info.tdata[j].note_list;//Ｙ座標
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->y,      sizeof(unsigned char), 1, fp);
			np = np->to;
		}
		np = info.tdata[j].note_list;//長さ
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->length, sizeof(unsigned char), 1, fp);
			np = np->to;
		}
		np = info.tdata[j].note_list;//ボリューム
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->volume, sizeof(unsigned char), 1, fp);
			np = np->to;
		}
		np = info.tdata[j].note_list;//パン
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->pan,    sizeof(unsigned char), 1, fp);
			np = np->to;
		}
	}	
	fclose(fp);
	PutRecentFile(music_file);
//	MessageBox(hWnd,"保存しました","Message (Save)",MB_OK);
	//↓2014.05.06 A
	if(SaveWithInitVolFile != 0){
		AutoSavePVIFile();
	}

	return TRUE;
}
//曲データを読み込み
extern char *dram_name[];
extern HWND hDlgPlayer;

//ファイルがロード可能であれば0異常であれば1を返す。ﾃﾞｰﾀロードはされない。 2014.05.22
int OrgData::FileCheckBeforeLoad(char *checkfile)
{
	FILE *fp;
	char pass_check[6];
	char ver = 0;
	if((fp=fopen(checkfile,"rb"))==NULL){
		//MessageBox(hWnd,"ファイルにアクセスできません","Error (Load)",MB_OK);
		return 1;
	}

	fread(&pass_check[0], sizeof(char), 6, fp);
	if( !memcmp( pass_check, pass, 6 ) )ver = 1;
	if( !memcmp( pass_check, pass2, 6 ) )ver = 2;
	if( !memcmp( pass_check, pass3, 6 ) )ver = 2;
	if( !ver ){
		fclose(fp);
		//MessageBox(hWnd,"このファイルは使えません","Error (Load)",MB_OK);
		return 1;
	}
	fclose(fp);

	return 0;
}
BOOL OrgData::LoadMusicData(void)
{
	ORGANYADATA org_data;
	NOTELIST *np;
	int i,j;
	char pass_check[6];
	char ver = 0;

	//｣｣｣｣｣｣｣｣｣｣｣｣｣｣｣ロード
	FILE *fp;
	if((fp=fopen(music_file,"rb"))==NULL){
		//MessageBox(hWnd,"ファイルにアクセスできません","Error (Load)",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_WARNING_ACCESS_FILE,IDS_ERROR_LOAD,MB_OK);	// 2014.10.19 A
		return(FALSE);
	}
	//パスワードチェック
	fread(&pass_check[0], sizeof(char), 6, fp);
	if( !memcmp( pass_check, pass, 6 ) )ver = 1;
	if( !memcmp( pass_check, pass2, 6 ) )ver = 2;
	if( !memcmp( pass_check, pass3, 6 ) )ver = 2;
	if( !ver ){
		fclose(fp);
		//MessageBox(hWnd,"このファイルは使えません","Error (Load)",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_ERROR_FILE,IDS_ERROR_LOAD,MB_OK);	// 2014.10.19 A
		return FALSE;
	}
//	for(i = 0; i < 4; i++){
//		if(pass[i] != pass_check[i]){
//			MessageBox(hWnd,"このファイルは使えません","Error (Load)",MB_OK);
//			fclose(fp);
//			return FALSE;
//		}
//	}
	//曲情報の読み込み
	fread(&org_data, sizeof(ORGANYADATA), 1, fp);

	//曲の情報を設定
	info.wait = org_data.wait;
	info.line = org_data.line;
	info.dot = org_data.dot;
	info.repeat_x = org_data.repeat_x;
	info.end_x = org_data.end_x;
	for(i = 0; i < MAXTRACK; i++){
		info.tdata[i].freq = org_data.tdata[i].freq;
		if( ver == 1 )info.tdata[i].pipi = 0;
		else info.tdata[i].pipi = org_data.tdata[i].pipi;
		info.tdata[i].wave_no = org_data.tdata[i].wave_no;
	}

	//音符のロード
	for(j = 0; j < MAXTRACK; j++){
		//最初の音符はfromがNULLとなる
		if(org_data.tdata[j].note_num == 0){
			info.tdata[j].note_list = NULL;
			continue;
		}
		//リストを作る
		np = info.tdata[j].note_p;
		info.tdata[j].note_list = info.tdata[j].note_p;
		np->from = NULL;
		np->to = (np + 1);
		np++;
		for(i = 1; i < org_data.tdata[j].note_num; i++){
			np->from = (np - 1);
			np->to = (np + 1);
			np++;
		}
		//最後の音符のtoはNULL
		np--;
		np->to = NULL;

		//内容を代入
		np = info.tdata[j].note_p;//Ｘ座標
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->x,      sizeof(long), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//Ｙ座標
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->y,      sizeof(unsigned char), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//長さ
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->length, sizeof(unsigned char), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//ボリューム
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->volume, sizeof(unsigned char), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//パン
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->pan,    sizeof(unsigned char), 1, fp);
			np++;
		}
	}
	fclose(fp);
	//データを有効に
	for(j = 0; j < MAXMELODY; j++)
		MakeOrganyaWave(j,info.tdata[j].wave_no, info.tdata[j].pipi);
	for(j = MAXMELODY; j < MAXTRACK; j++){
		i = info.tdata[j].wave_no;
		InitDramObject(dram_name[i],j-MAXMELODY);
	}
	//プレイヤーに表示
	char str[32];
	SetPlayPointer(0);//頭出し
	scr_data.SetHorzScroll(0);
	itoa(info.wait,str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWWAIT,str);
	SetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,"0");
	SetDlgItemText(hDlgPlayer,IDE_VIEWXPOS,"0");

	MakeMusicParts(info.line,info.dot);//パーツを生成
	MakePanParts(info.line,info.dot);
	PutRecentFile(music_file);
	//↓2014.05.06 A
	if(SaveWithInitVolFile != 0){
		AutoLoadPVIFile();
	}
	return TRUE;
}

void OrgData::SortNotes()
{
	NOTELIST *pntl,*pNtls,*np;
	ORGANYADATA org_data;
	int i,j;

	org_data.wait = info.wait;
	org_data.line = info.line;
	org_data.dot = info.dot;
	org_data.repeat_x = info.repeat_x;
	org_data.end_x = info.end_x;

	for(i = 0; i < MAXTRACK; i++){
		org_data.tdata[i].freq = info.tdata[i].freq;
		org_data.tdata[i].wave_no = info.tdata[i].wave_no;
		org_data.tdata[i].pipi = info.tdata[i].pipi;
		org_data.tdata[i].note_num = GetNoteNumber(i,NULL);
	}

	pNtls = new NOTELIST[4096]; //退避用
	
	for(j = 0; j < MAXTRACK; j++){
		if(info.tdata[j].note_list == NULL)continue;
		pntl = pNtls;
		np = info.tdata[j].note_list;//音符の先頭
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			pNtls[i].x = np->x;
			pNtls[i].y = np->y;
			pNtls[i].pan = np->pan;
			pNtls[i].volume = np->volume;
			pNtls[i].length = np->length;
			np = np->to;
		}
		//リストを作る
		if(true){
			np = info.tdata[j].note_p; //領域の先頭
			info.tdata[j].note_list = info.tdata[j].note_p;
			np->from = NULL;
			np->to = (np + 1);
			np++;
			for(i = 1; i < org_data.tdata[j].note_num; i++){
				np->from = (np - 1);
				np->to = (np + 1);
				np++;
			}
			//最後の音符のtoはNULL
			np--;
			np->to = NULL;
			np = info.tdata[j].note_p;//Ｘ座標
			for(i = 0; i < org_data.tdata[j].note_num; i++){
				np->x = pNtls[i].x;
				np->y = pNtls[i].y;
				np->length = pNtls[i].length;
				np->pan = pNtls[i].pan;
				np->volume = pNtls[i].volume;
				np++;
			}
		}else{ //実験的にサカシにする
			np = info.tdata[j].note_p+4095; //領域の先頭
			info.tdata[j].note_list = info.tdata[j].note_p;
			np->from = NULL;
			np->to = (np - 1);
			np--;
			for(i = 1; i < org_data.tdata[j].note_num; i++){
				np->from = (np + 1);
				np->to = (np - 1);
				np--;
			}
			//最後の音符のtoはNULL
			np++;
			np->to = NULL;
			np = info.tdata[j].note_p;//Ｘ座標
			for(i = 0; i < org_data.tdata[j].note_num; i++){
				np->x = pNtls[i].x;
				np->y = pNtls[i].y;
				np->length = pNtls[i].length;
				np->pan = pNtls[i].pan;
				np->volume = pNtls[i].volume;
				np++;
			}

		}
	}	
	delete [] pNtls;	// 2014.10.18 解放を追加した。
	//プレイヤーに表示
	char str[32];
	SetPlayPointer(0);//頭出し
	scr_data.SetHorzScroll(0);
	itoa(info.wait,str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWWAIT,str);
	SetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,"0");
	SetDlgItemText(hDlgPlayer,IDE_VIEWXPOS,"0");

	MakeMusicParts(info.line,info.dot);//パーツを生成
	MakePanParts(info.line,info.dot);

}
