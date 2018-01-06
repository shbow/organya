#include <windows.h>
#include <stdio.h>
#include <dplay.h>
#include <dsound.h>
#include "Setting.h"
#include "DefOrg.h"
#define SE_MAX	512

// シンボル定義.
#define	SMPFRQ			48000				//!< サンプリング周波数.
#define	BUFSIZE			((SMPFRQ * 4) / 10)	//!< データバッファサイズ (100ms相当).


// DirectSound構造体 
LPDIRECTSOUND       lpDS = NULL;            // DirectSoundオブジェクト
LPDIRECTSOUNDBUFFER lpPRIMARYBUFFER = NULL; // 一時バッファ
LPDIRECTSOUNDBUFFER lpSECONDARYBUFFER[SE_MAX] = {NULL};
LPDIRECTSOUNDBUFFER lpORGANBUFFER[8][8][2] = {NULL};
LPDIRECTSOUNDBUFFER lpDRAMBUFFER[8] = {NULL};

//録音用
//HANDLE						CapEvent[2];			//!< 入力イベント・オブジェクト.
//DWORD						CapBufSize;				//!< キャプチャバッファ・サイズ.
//DWORD						GetPos;					//!< キャプチャバッファの読み込み開始位置.
//DWORD						PutPos;					//!< キャプチャバッファの書き込み開始位置.
//BYTE*						DataBuff;				//!< データバッファ.
//LPDIRECTSOUNDCAPTURE 		CapDev;					//!< IDirectSoundCaptureインターフェイス ポインタ.
//LPDIRECTSOUNDCAPTUREBUFFER	CapBuf;					//!< IDirectSoundBufferインターフェイス ポインタ.

DWORD						OutBufSize;				//!< ストリームバッファ・サイズ.


// DirectSoundの開始 
BOOL InitDirectSound(HWND hwnd)
{
//    int i;
    DSBUFFERDESC dsbd;

    // DirectDrawの初期化
    if(DirectSoundCreate(NULL, &lpDS, NULL) != DS_OK){
		lpDS = NULL;
		return(FALSE);
	}
    lpDS->SetCooperativeLevel(hwnd, DSSCL_EXCLUSIVE);

    // 一次バッファの初期化
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER; // | DSBCAPS_CTRLPOSITIONNOTIFY;
    lpDS->CreateSoundBuffer(&dsbd, &lpPRIMARYBUFFER, NULL);

//    for(i = 0; i < SE_MAX; i++) lpSECONDARYBUFFER[i] = NULL;
	
	//キャプチャバッファの作成 第一引数NULLでデフォルト。これはどうか。
//	if( DirectSoundCaptureCreate( NULL, &CapDev, NULL ) != S_OK ){
//		return FALSE;
//	}
//	dsbd.dwFlags = 0; //セカンダリバッファ
//	CapDev->CreateCaptureBuffer(&dsbd, &CapBuf, NULL);


    return(TRUE);
}

// DirectSoundの終了 
void EndDirectSound(void)
{
    int i;

    for(i = 0; i < 8; i++){
        if(lpSECONDARYBUFFER[i] != NULL){
			lpSECONDARYBUFFER[i]->Release();
			lpSECONDARYBUFFER[i] = NULL;
		}
    }
    if(lpPRIMARYBUFFER != NULL){
		lpPRIMARYBUFFER->Release();
		lpPRIMARYBUFFER = NULL;
	}
    if(lpDS != NULL){
		lpDS->Release();
		lpDS = NULL;
	}
//	if( CapBuf ){
//		CapBuf->Stop();
//	}
//    if(CapDev != NULL){
//		CapDev->Release();
//		CapDev = NULL;
//	}
}
void ReleaseSoundObject(int no){
	if(lpSECONDARYBUFFER[no] != NULL){
		lpSECONDARYBUFFER[no]->Release();
		lpSECONDARYBUFFER[no] = NULL;
	}
}


// サウンドの設定 
BOOL InitSoundObject( LPCSTR resname, int no)
{
    HRSRC hrscr;
    DSBUFFERDESC dsbd;
    DWORD *lpdword;//リソースのアドレス
    // リソースの検索
    if((hrscr = FindResource(NULL, resname, "WAVE")) == NULL)
                                                    return(FALSE);
    // リソースのアドレスを取得
    lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));
	// 二次バッファの生成
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = 
		DSBCAPS_STATIC|
		DSBCAPS_STICKYFOCUS
		|DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)lpdword+0x36);//WAVEデータのサイズ
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(lpdword+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no],
								NULL) != DS_OK) return(FALSE);
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
    // 二次バッファのロック
    lpSECONDARYBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)lpdword+0x36),
                        &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	// 音源データの設定
	CopyMemory(lpbuf1, (BYTE*)lpdword+0x3a, dwbuf1);
    if(dwbuf2 != 0) CopyMemory(lpbuf2, (BYTE*)lpdword+0x3a+dwbuf1, dwbuf2);
	// 二次バッファのロック解除
	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 

    return(TRUE);
}

//extern LPDIRECTDRAW            lpDD;	// DirectDrawオブジェクト
BOOL LoadSoundObject(char *file_name, int no)
{
	DWORD i;
	DWORD file_size = 0;
	char check_box[58];
	FILE *fp;
	if((fp=fopen(file_name,"rb"))==NULL){
//		char msg_str[64];				//数値確認用
//		lpDD->FlipToGDISurface(); //メッセージ表示の方にフリップ
//		sprintf(msg_str,"%sが見当たりません",file_name);
//		MessageBox(hWND,msg_str,"title",MB_OK);
//		SetCursor(FALSE); // カーソル消去
		return(FALSE);
	}
	for(i = 0; i < 58; i++){
		fread(&check_box[i],sizeof(char),1,fp);
	}
	if(check_box[0] != 'R')return(FALSE);
	if(check_box[1] != 'I')return(FALSE);
	if(check_box[2] != 'F')return(FALSE);
	if(check_box[3] != 'F')return(FALSE);
	file_size = *((DWORD *)&check_box[4]);

	DWORD *wp;
	wp = (DWORD*)malloc(file_size);//ファイルのワークスペースを作る
	fseek(fp,0,SEEK_SET);
	for(i = 0; i < file_size; i++){
		fread((BYTE*)wp+i,sizeof(BYTE),1,fp);
	}
	fclose(fp);
	//セカンダリバッファの生成
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_STATIC|DSBCAPS_STICKYFOCUS|DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)wp+0x36);//WAVEデータのサイズ
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(wp+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no],
								NULL) != DS_OK){
		free(wp);
		return(FALSE);	
	}
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
	HRESULT hr;
	hr = lpSECONDARYBUFFER[no]->Lock(0, file_size-58,
							&lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	if(hr != DS_OK){
		free(wp);
		return (FALSE);
	}
	CopyMemory(lpbuf1, (BYTE*)wp+0x3a, dwbuf1);//+3aはデータの頭
	if(dwbuf2 != 0)	CopyMemory(lpbuf2, (BYTE*)wp+0x3a+dwbuf1, dwbuf2);
	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	
	free(wp);
	return(TRUE);
}

// サウンドの再生 
void PlaySoundObject(int no, int mode)
{
    if(lpSECONDARYBUFFER[no] != NULL){
		switch(mode){
		case 0: // 停止
			lpSECONDARYBUFFER[no]->Stop();
			break;
		case 1: // 再生
			lpSECONDARYBUFFER[no]->Stop();
			lpSECONDARYBUFFER[no]->SetCurrentPosition(0);
            lpSECONDARYBUFFER[no]->Play(0, 0, 0);
            break;
		case -1: // ループ再生
			lpSECONDARYBUFFER[no]->Play(0, 0, DSBPLAY_LOOPING);
			break;
		}
    }
}

void ChangeSoundFrequency(int no, DWORD rate)//100がMIN9999がMAXで2195?がﾉｰﾏﾙ
{
	if(lpSECONDARYBUFFER[no] != NULL)
		lpSECONDARYBUFFER[no]->SetFrequency( rate );
}
void ChangeSoundVolume(int no, long volume)//300がMAXで300がﾉｰﾏﾙ
{
	if(lpSECONDARYBUFFER[no] != NULL)
		lpSECONDARYBUFFER[no]->SetVolume((volume-300)*8);
}
void ChangeSoundPan(int no, long pan)//512がMAXで256がﾉｰﾏﾙ
{
	if(lpSECONDARYBUFFER[no] != NULL)
		lpSECONDARYBUFFER[no]->SetPan((pan-256)*10);
}

/////////////////////////////////////////////
//■オルガーニャ■■■■■■■■■■■■///////
/////////////////////


typedef struct{
	short wave_size;//波形のサイズ
	short oct_par;//オクターブを実現する掛け率(/8)
	short oct_size;//オクターブを実現する掛け率(/8)
}OCTWAVE;

OCTWAVE oct_wave[8] = {
	{256,  1, 4},//0 Oct
	{256,  2, 8},//1 Oct
	{128,  4, 12},//2 Oct
	{128,  8, 16},//3 Oct
	{ 64, 16, 20},//4 Oct
	{ 32, 32, 24},
	{ 16, 64, 28},
	{  8,128, 32},
};
BYTE format_tbl2[] = {0x01,0x00,0x01,0x00,0x22,0x56,0x00,//22050HzのFormat
0x00,0x22,0x56,0x00,0x00,0x01,0x00,0x08,0x00,0x00,0x00};
//BYTE format_tbl3[] = {0x01,0x00,0x01,0x00,0x44,0xac,0x00,//441000HzのFormat
//0x00,0x44,0xac,0x00,0x00,0x08,0x00,0x00,0x00,0x66,0x61};
BOOL MakeSoundObject8(char *wavep,char track, char pipi )
{
	DWORD i,j,k;
	unsigned long wav_tp;//WAVテーブルをさすポインタ
	DWORD wave_size;//256;
	DWORD data_size;
	BYTE *wp;
	BYTE *wp_sub;
	int work;
	//セカンダリバッファの生成
	DSBUFFERDESC dsbd;

	for(j = 0; j < 8; j++){
		for(k = 0; k < 2; k++){
			wave_size = oct_wave[j].wave_size;
			if( pipi )data_size = wave_size * oct_wave[j].oct_size;
			else data_size = wave_size;
			ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
			dsbd.dwSize = sizeof(DSBUFFERDESC);
			dsbd.dwFlags = DSBCAPS_STATIC|
					DSBCAPS_STICKYFOCUS|
					DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;//DSBCAPS_STATIC|DSBCAPS_CTRLDEFAULT;
			dsbd.dwBufferBytes = data_size;//file_size;
			dsbd.lpwfxFormat = (LPWAVEFORMATEX)(&format_tbl2[0]);
				if(lpDS->CreateSoundBuffer(&dsbd, &lpORGANBUFFER[track][j][k],//j = se_no
										NULL) != DS_OK) return(FALSE);
			wp = (BYTE*)malloc(data_size);//ファイルのワークスペースを作る
			wp_sub = wp;
			wav_tp = 0;
			for(i = 0; i < data_size; i++){
				work = *(wavep+wav_tp);
				work+=0x80;
				*wp_sub=(BYTE)work;
				wav_tp += 256/wave_size;
				if( wav_tp > 255 ) wav_tp -= 256;
				wp_sub++;
			}
			//データの転送
			LPVOID lpbuf1, lpbuf2;
			DWORD dwbuf1, dwbuf2=0;
			HRESULT hr;
			hr = lpORGANBUFFER[track][j][k]->Lock(0, data_size,//-58,
								&lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);		
			if(hr != DS_OK){
				free( wp );
				return (FALSE);
			}
			CopyMemory(lpbuf1, (BYTE*)wp,dwbuf1);//+3aはデータの頭
			if(dwbuf2 != 0)	CopyMemory(lpbuf2, (BYTE*)wp+dwbuf1, dwbuf2);
			lpORGANBUFFER[track][j][k]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
			lpORGANBUFFER[track][j][k]->SetCurrentPosition(0);
			free( wp );
		}
	}
	return(TRUE);
}
//2.1.0で 整数型から小数点型に変更してみた。20140401
//short freq_tbl[12] = {261,278,294,311,329,349,371,391,414,440,466,494};
double freq_tbl[12] = {261.62556530060, 277.18263097687, 293.66476791741, 311.12698372208, 329.62755691287, 349.22823143300, 369.99442271163, 391.99543598175, 415.30469757995, 440.00000000000, 466.16376151809, 493.88330125612};
void ChangeOrganFrequency(unsigned char key,char track, DWORD a)
{
	double tmpDouble;
	for(int j = 0; j < 8; j++)
		for(int i = 0; i < 2; i++){
			tmpDouble = (((double)oct_wave[j].wave_size * freq_tbl[key])*(double)oct_wave[j].oct_par)/8.00f + ((double)a - 1000.0f);
			
			
			lpORGANBUFFER[track][j][i]->SetFrequency(//1000を+αのデフォルト値とする
				(DWORD)tmpDouble
//				((oct_wave[j].wave_size*freq_tbl[key])*oct_wave[j].oct_par)/8 + (a-1000)
			);
		}
}
short pan_tbl[13] = {0,43,86,129,172,215,256,297,340,383,426,469,512}; 
unsigned char old_key[MAXTRACK] = {255,255,255,255,255,255,255,255};//再生中の音
unsigned char key_on[MAXTRACK] = {0};//キースイッチ
unsigned char key_twin[MAXTRACK] = {0};//今使っているキー(連続時のノイズ防止の為に二つ用意)
void ChangeOrganPan(unsigned char key, unsigned char pan,char track)//512がMAXで256がﾉｰﾏﾙ
{
	if(old_key[track] != 255)
		lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetPan((pan_tbl[pan]-256)*10);
}
void ChangeOrganVolume(int no, long volume,char track)//300がMAXで300がﾉｰﾏﾙ
{
	if(old_key[track] != 255)
		lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetVolume((volume-255)*8);
}
// サウンドの再生 
void PlayOrganObject(unsigned char key, int mode,char track,DWORD freq)
{
	
    if(lpORGANBUFFER[track][key/12][key_twin[track]] != NULL){
		switch(mode){
		case 0: // 停止
			lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Stop();
			lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetCurrentPosition(0);
			break;
		case 1: // 再生
//			if(key_on == 1 && no == old_key/12)//
//				lpORGANBUFFER[old_key/12]->Stop();
//				ChangeOrganFrequency(key%12);//周波数を設定して
//				lpORGANBUFFER[no]->Play(0, 0, 0);
//			if(key_on == 1 && no == old_key/12){//鳴ってるWAVが同じWAVNOなら
//				old_key = key;
//				ChangeOrganFrequency(key%12);//周波数を変えるだけ
//			}
			break;
		case 2: // 歩かせ停止
			if(old_key[track] != 255){
				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);
				old_key[track] = 255;
			}
            break;
		case -1:
			if(old_key[track] == 255){//新規鳴らす
				ChangeOrganFrequency(key%12,track,freq);//周波数を設定して
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, DSBPLAY_LOOPING);
				old_key[track] = key;
				key_on[track] = 1;
			}else if(key_on[track] == 1 && old_key[track] == key){//同じ音
				//今なっているのを歩かせ停止
				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);
				key_twin[track]++;
				if(key_twin[track] == 2)key_twin[track] = 0; 
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, DSBPLAY_LOOPING);
			}else{//違う音を鳴らすなら
				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);//今なっているのを歩かせ停止
				key_twin[track]++;
				if(key_twin[track] == 2)key_twin[track] = 0; 
				ChangeOrganFrequency(key%12,track,freq);//周波数を設定して
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, DSBPLAY_LOOPING);
				old_key[track] = key;
			}
			break;
		}
    }
}
//ぴぴ
void PlayOrganObject2(unsigned char key, int mode,char track,DWORD freq)
{
	
    if(lpORGANBUFFER[track][key/12][key_twin[track]] != NULL){
		switch(mode){
		case 0: // 停止
			lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Stop();
			lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetCurrentPosition(0);
			break;
		case 1: // 再生
//			if(key_on == 1 && no == old_key/12)//
//				lpORGANBUFFER[old_key/12]->Stop();
//				ChangeOrganFrequency(key%12);//周波数を設定して
//				lpORGANBUFFER[no]->Play(0, 0, 0);
//			if(key_on == 1 && no == old_key/12){//鳴ってるWAVが同じWAVNOなら
//				old_key = key;
//				ChangeOrganFrequency(key%12);//周波数を変えるだけ
//			}
			break;
		case 2: // 歩かせ停止
			if(old_key[track] != 255){
//				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);
				old_key[track] = 255;
			}
            break;
		case -1:
			if(old_key[track] == 255){//新規鳴らす
				ChangeOrganFrequency(key%12,track,freq);//周波数を設定して
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, 0);//DSBPLAY_LOOPING);
				old_key[track] = key;
				key_on[track] = 1;
			}else if(key_on[track] == 1 && old_key[track] == key){//同じ音
				//今なっているのを歩かせ停止
//				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);
				key_twin[track]++;
				if(key_twin[track] == 2)key_twin[track] = 0; 
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, 0);//DSBPLAY_LOOPING);
			}else{//違う音を鳴らすなら
//				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);//今なっているのを歩かせ停止
				key_twin[track]++;
				if(key_twin[track] == 2)key_twin[track] = 0; 
				ChangeOrganFrequency(key%12,track,freq);//周波数を設定して
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, 0);//DSBPLAY_LOOPING);
				old_key[track] = key;
			}
			break;
		}
    }
}
//オルガーニャオブジェクトを開放
void ReleaseOrganyaObject(char track){
	for(int i = 0; i < 8; i++){
		if(lpORGANBUFFER[track][i][0] != NULL){
			lpORGANBUFFER[track][i][0]->Release();
			lpORGANBUFFER[track][i][0] = NULL;
		}
		if(lpORGANBUFFER[track][i][1] != NULL){
			lpORGANBUFFER[track][i][1]->Release();
			lpORGANBUFFER[track][i][1] = NULL;
		}
	}
}
//波形データをロード
//char wave_data[100*256];
char *wave_data = NULL;
BOOL InitWaveData100(void)
{
	if(wave_data == NULL)wave_data = (char *)malloc(sizeof(char) * 256 * 256);
    HRSRC hrscr;
    DWORD *lpdword;//リソースのアドレス
    // リソースの検索
    if((hrscr = FindResource(NULL, "WAVE100", "WAVE")) == NULL)
                                                    return(FALSE);
    // リソースのアドレスを取得
    lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));
	memcpy(wave_data,lpdword,100*256);
	return TRUE;
}
BOOL LoadWaveData100(void)
{
	if(wave_data == NULL)wave_data = (char *)malloc(sizeof(char) * 256 * 256);
	FILE *fp;
	if((fp=fopen("Wave.dat","rb"))==NULL){
		return FALSE;
	}
//	wave_data = new char[100*256];
	fread(wave_data, sizeof(char), 256*100, fp);
	fclose(fp);
	return TRUE;
}
BOOL DeleteWaveData100(void)
{
//	delete wave_data;
	free(wave_data);
	return TRUE;
}
//波形を１００個の中から選択して作成
BOOL MakeOrganyaWave(char track,char wave_no, char pipi)
{
	if(wave_no > 99)return FALSE;
	ReleaseOrganyaObject(track);
	MakeSoundObject8(&wave_data[0] + wave_no*256,track, pipi);	
	return TRUE;
}
/////////////////////////////////////////////
//■オルガーニャドラムス■■■■■■■■///////
/////////////////////
//オルガーニャオブジェクトを開放
void ReleaseDramObject(char track){
	for(int i = 0; i < 8; i++){
		if(lpDRAMBUFFER[track] != NULL){
			lpDRAMBUFFER[track]->Release();
			lpDRAMBUFFER[track] = NULL;
		}
	}
}
// サウンドの設定 
BOOL InitDramObject( LPCSTR resname, int no)
{
    HRSRC hrscr;
    DSBUFFERDESC dsbd;
    DWORD *lpdword;//リソースのアドレス
    // リソースの検索
	ReleaseDramObject(no); //ここにおいてみた。

    if((hrscr = FindResource(NULL, resname, "WAVE")) == NULL)
                                                    return(FALSE);
    // リソースのアドレスを取得
    lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));
	// 二次バッファの生成
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = 
		DSBCAPS_STATIC|
		DSBCAPS_STICKYFOCUS
		|DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)lpdword+0x36);//WAVEデータのサイズ
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(lpdword+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpDRAMBUFFER[no],NULL) != DS_OK) return(FALSE);
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
    // 二次バッファのロック
    lpDRAMBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)lpdword+0x36),
                        &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	// 音源データの設定
	CopyMemory(lpbuf1, (BYTE*)lpdword+0x3a, dwbuf1);
    if(dwbuf2 != 0){
		CopyMemory(lpbuf2, (BYTE*)lpdword+0x3a+dwbuf1, dwbuf2);
		
		
	}

	// 二次バッファのロック解除
	lpDRAMBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	lpDRAMBUFFER[no]->SetCurrentPosition(0);

    return(TRUE);
}

/*
BOOL LoadDramObject(char *file_name, int no)
{
	DWORD i;
	DWORD file_size = 0;
	char check_box[58];
	FILE *fp;
	ReleaseDramObject(no);
	if((fp=fopen(file_name,"rb"))==NULL){
//		char msg_str[64];				//数値確認用
//		lpDD->FlipToGDISurface(); //メッセージ表示の方にフリップ
//		sprintf(msg_str,"%sが見当たりません",file_name);
//		MessageBox(hWND,msg_str,"title",MB_OK);
//		SetCursor(FALSE); // カーソル消去
		return(FALSE);
	}
	for(i = 0; i < 58; i++){
		fread(&check_box[i],sizeof(char),1,fp);
	}
	if(check_box[0] != 'R')return(FALSE);
	if(check_box[1] != 'I')return(FALSE);
	if(check_box[2] != 'F')return(FALSE);
	if(check_box[3] != 'F')return(FALSE);
	file_size = *((DWORD *)&check_box[4]);

	DWORD *wp;
	wp = (DWORD*)malloc(file_size);//ファイルのワークスペースを作る
	fseek(fp,0,SEEK_SET);
	for(i = 0; i < file_size; i++){
		fread((BYTE*)wp+i,sizeof(BYTE),1,fp);
	}
	fclose(fp);
	//セカンダリバッファの生成
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_STATIC|
		DSBCAPS_STICKYFOCUS
		|DSBCAPS_CTRLDEFAULT;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)wp+0x36);//WAVEデータのサイズ
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(wp+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpDRAMBUFFER[no],
								NULL) != DS_OK) return(FALSE);	
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
	HRESULT hr;
	hr = lpDRAMBUFFER[no]->Lock(0, file_size-58,
							&lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	if(hr != DS_OK)return (FALSE);
	CopyMemory(lpbuf1, (BYTE*)wp+0x3a, dwbuf1);//+3aはデータの頭
	if(dwbuf2 != 0)	CopyMemory(lpbuf2, (BYTE*)wp+0x3a+dwbuf1, dwbuf2);
	lpDRAMBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	
	free(wp);
	return(TRUE);
}*/
void ChangeDramFrequency(unsigned char key,char track)
{
	lpDRAMBUFFER[track]->SetFrequency(key*800+100);
}
void ChangeDramPan(unsigned char pan,char track)//512がMAXで256がﾉｰﾏﾙ
{
	lpDRAMBUFFER[track]->SetPan((pan_tbl[pan]-256)*10);
}
void ChangeDramVolume(long volume,char track)//
{
	lpDRAMBUFFER[track]->SetVolume((volume-255)*8);
}
// サウンドの再生 
void PlayDramObject(unsigned char key, int mode,char track)
{
	
    if(lpDRAMBUFFER[track] != NULL){
		switch(mode){
		case 0: // 停止
			lpDRAMBUFFER[track]->Stop();
			lpDRAMBUFFER[track]->SetCurrentPosition(0);
			break;
		case 1: // 再生
			lpDRAMBUFFER[track]->Stop();
			lpDRAMBUFFER[track]->SetCurrentPosition(0);
			ChangeDramFrequency(key,track);//周波数を設定して
			lpDRAMBUFFER[track]->Play(0, 0, 0);
			break;
		case 2: // 歩かせ停止
            break;
		case -1:
			break;
		}
    }
}
void PlayOrganKey(unsigned char key,char track,DWORD freq,int Nagasa)
{
	if(key>96)return;
	if(track < MAXMELODY){
		DWORD wait = timeGetTime();
		ChangeOrganFrequency(key%12,track,freq);//周波数を設定して
		lpORGANBUFFER[track][key/12][0]->SetVolume((160-255)*8);
		lpORGANBUFFER[track][key/12][0]->Play(0, 0, DSBPLAY_LOOPING);
		do{
		}while(timeGetTime() < wait + (DWORD)Nagasa);
//		lpORGANBUFFER[track][key/12][0]->Play(0, 0, 0); //C 2010.09.23 即時停止する。
		lpORGANBUFFER[track][key/12][0]->Stop();
	}else{
		lpDRAMBUFFER[track - MAXMELODY]->Stop();
		lpDRAMBUFFER[track - MAXMELODY]->SetCurrentPosition(0);
		ChangeDramFrequency(key,track - MAXMELODY);//周波数を設定して
		lpDRAMBUFFER[track - MAXMELODY]->SetVolume((160-255)*8);
		lpDRAMBUFFER[track - MAXMELODY]->Play(0, 0, 0);
	}
}


//2010.08.14 A
void Rxo_PlayKey(unsigned char key,char track,DWORD freq, int Phase)
{
	if(key>96)return;
	if(track < MAXMELODY){
		ChangeOrganFrequency(key%12,track,freq);
		lpORGANBUFFER[track][key/12][Phase]->SetVolume((160-255)*8);
		lpORGANBUFFER[track][key/12][Phase]->Play(0, 0, DSBPLAY_LOOPING);
	}else{
		lpDRAMBUFFER[track - MAXMELODY]->Stop();
		lpDRAMBUFFER[track - MAXMELODY]->SetCurrentPosition(0);
		ChangeDramFrequency(key,track - MAXMELODY);//周波数を設定して
		lpDRAMBUFFER[track - MAXMELODY]->SetVolume((160-255)*8);
		lpDRAMBUFFER[track - MAXMELODY]->Play(0, 0, 0);
	}
}
//2010.08.14 A
void Rxo_StopKey(unsigned char key,char track, int Phase)
{
	if(track < MAXMELODY){
		//lpORGANBUFFER[track][key/12][Phase]->Play(0, 0, 0);	// 2010.08.14 D
		lpORGANBUFFER[track][key/12][Phase]->Stop();	// 2010.08.14 A
	}else{
		lpDRAMBUFFER[track - MAXMELODY]->Stop();
		lpDRAMBUFFER[track - MAXMELODY]->SetCurrentPosition(0);
	}	
}

//デバッグ用。いろんな状態を出力。
void Rxo_ShowDirectSoundObject(HWND hwnd)
{
	
}

//音をすぐに全部止める
void Rxo_StopAllSoundNow(void)
{
	int i,j,k;
	for(i=0;i<SE_MAX;i++) if(lpSECONDARYBUFFER[i]!=NULL)lpSECONDARYBUFFER[i]->Stop();
	
	for(i=0;i<8;i++){
		for(j=0;j<8;j++)for(k=0;k<2;k++)lpORGANBUFFER[i][j][k]->Stop();
		lpDRAMBUFFER[i]->Stop();
	}
	for(i=0;i<MAXTRACK;i++)old_key[i]=255; //2014.05.02 A これで頭が変な音にならなくする。
}
