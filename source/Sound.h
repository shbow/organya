/*****							  *****/
/*****   DirectSoundサポート関数   *****/
/*****							  *****/

//#include <dsound.h>

// DirectSoundの開始
BOOL InitDirectSound(HWND hwnd);
BOOL LoadSoundObject( char *file_name, int no);
// DirectSoundの終了
void EndDirectSound(void);
void ReleaseSoundObject(int no);//バッファの開放
BOOL InitSoundObject(LPCSTR resname, int no);
// サウンドの再生
void PlaySoundObject(int no, int mode);
void ChangeSoundVolume(int no, long volume);//9999がMAX2195?がﾉｰﾏﾙ
void ChangeSoundFrequency(int no, DWORD rate);//300がMAX300がﾉｰﾏﾙ
void ChangeSoundPan(int no, long pan);

BOOL MakeSoundObject8(char *wavep,char track, char pipi);

void ChangeOrganFrequency(unsigned char key,char track,DWORD a);//9999がMAXで2195?がﾉｰﾏﾙ
void ChangeOrganVolume(int no, long volume,char track);//300がMAXで300がﾉｰﾏﾙ
void ChangeOrganPan(unsigned char key, unsigned char pan,char track);//0が←で6がﾉｰﾏﾙ11が→
void PlayOrganObject(unsigned char key, int mode,char track,DWORD freq);
void PlayOrganObject2(unsigned char key, int mode,char track,DWORD freq);
void PlayOrganKey(unsigned char key,char track,DWORD freq, int Nagasa = 80);
BOOL LoadWaveData100(void);
BOOL InitWaveData100(void);
BOOL DeleteWaveData100(void);

BOOL MakeOrganyaWave(char track,char wave_no, char pipi);

//BOOL LoadDramObject(char *file_name, int no);
BOOL InitDramObject(LPCSTR resname, int no);
void ReleaseDramObject(char track);
void ChangeDramFrequency(unsigned char key,char track);
void ChangeDramPan(unsigned char pan,char track);//512がMAXで256がﾉｰﾏﾙ
void ChangeDramVolume(long volume,char track);//300がMAXで300がﾉｰﾏﾙ
void PlayDramObject(unsigned char key, int mode,char track);


void Rxo_ShowDirectSoundObject(HWND hwnd);
void Rxo_StopAllSoundNow(void);

void Rxo_PlayKey(unsigned char key,char track,DWORD freq, int Phase);
void Rxo_StopKey(unsigned char key,char track, int Phase);