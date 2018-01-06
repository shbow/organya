
#define GET_MEAS1	1
#define GET_MEAS2	3
#define GET_BEAT1	2
#define GET_BEAT2	4

#define MESSAGE_STRING_MAX 1024


void TransportNote(int AddNotes , int Track );
void VolumeEdit(int AddNotes , int Track );
void VolumeWariaiEdit(int AddNotes , int Track ); //2014.04.30 A
void VolumeDecayEdit(int AddNotes , int Track , int Function ); //2014.05.01 A

void PanEdit(int AddNotes , int Track );
void MuteTrack(int Track);

extern char *TrackCode[]; //トラック名と数字の置き換え用
extern int Menu_Recent[];
extern char RecentFileName[10][MAX_PATH];	//最近使ったファイル名

extern int ReverseTrackCode(char *strTrack); //その逆函数
extern void ShowMemoryState();
extern void SortMusicNote(void); //音符の整理


extern void ClearVirtualCB(void);
extern void AddIntegerToVirtualCB(int iNum);
extern void AddStartToVirtualCB(void);
extern bool ReadStartFromVirtualCB(void);
extern int ReadIntegerFromVirtualCB(void);
extern void SetClipBoardFromVCB(void);
extern void GetClipBoardToVCB(void);
extern void AddTrackSeparater(void);
extern void AddStartToVirtualCB(void);

extern void ReplaseUndo();
extern void SetUndo();
extern void ClearUndo();
extern void ReplaceRedo();
extern void ResetLastUndo(); //取りけし
int GetSelectMeasBeat(int GetToValue, int addValue=1);
int MinimumGrid(int x);
int MaximumGrid(int x);
int MinimumGridLine(int x);
int MaximumGridLine(int x);
extern void ChangeGridMode(int iValue=-1);
extern void ChangeSelAlwaysCurrent(int iValue=-1);
extern void ChangeDrawDouble(int iValue=-1);
void ChangeDragMode(int iValue=-1);
extern void ChangeEnablePlaying(int iValue=-1);
extern void ChangeFinish(int iValue=-1);
extern void ChangeSlideOverlapNoteMode(int iValue = -1);
extern void ChangePushStratchNOTE(int iValue = -1);

void PutRecentFile(char *FileName);
void LoadRecentFromIniFile();
void SaveRecentFilesToInifile();
void SetLoadRecentFile(int iNum);
void ClearRecentFile();

int msgbox(HWND hWnd , int MessageID, int TitleID, UINT uType);		// 2014.10.18 

//メッセージテーブルの解放と初期化
void FreeMessageStringBuffer(void);	// 2014.10.19 
int AllocMessageStringBuffer(void);	// 2014.10.19 
//メッセージテーブル
extern TCHAR *MessageString[MESSAGE_STRING_MAX];	// 2014.10.19 


extern int iChangeEnablePlaying; //2010.09.23 A
extern int iChangeFinish; //2010.09.23 A

extern void setRecentTrack(int iNewTrack); //最近使ったトラック番号を更新する //2010.09.23 A
extern int getRecentTrack(int iOrder, int isDrumTrack); //最近使ったトラックを返す //2010.09.23 A

extern int iActivatePAN; //2014.05.01 A
extern int iActivateVOL; //2014.05.01 A
extern int iSlideOverlapNotes; //2014.05.06 A

extern bool AutoLoadPVIFile();
extern bool AutoSavePVIFile();
extern int SaveWithInitVolFile;
extern void ChangeAutoLoadMode(int iValue = -1);
extern int tra;

extern int NoteWidth; //音符の幅 2014.05.26
extern int NoteEnlarge_Until_16px; //表示の際、NOTEの頭を最大16ピクセルまで伸ばす。
extern void ChangeNoteEnlarge(int iValue = -1);
extern int iPushStratch;
extern int iLastEditNoteLength;
