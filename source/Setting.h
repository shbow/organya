#include <windows.h>
//ウインドウの広さ(枠は省く)
#define WINDOWWIDTH		1180
#define WDWHEIGHTPLUS	0
#define WINDOWHEIGHT	(432+WDWHEIGHTPLUS)
#define WHNM			(432 + 16)

#define KEYWIDTH		64

//ビットマップNO
#define BMPMUSIC		0
#define BMPNOTE			1
#define BMPNUMBER		2
#define BMPPAN			3


#define MSGCANCEL		1
#define MSGEXISFILE		2	//既存ファイルあり
#define MSGSAVEOK		3	//保存完了
#define MSGLOADOK		3	//読み込み完了

extern char music_file[];
extern HINSTANCE hInst;//インスタンスハンドル
extern HWND hWnd;//メインウィンドウハンドル
extern BOOL actApp;//ウインドウのアクティブ

extern int  WWidth, WHeight;	//ウィンドウのサイズ