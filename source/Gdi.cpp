#include "Setting.h"
#include "DefOrg.h"
#include "OrgData.h"

#define MAXBITMAP		64

HBITMAP hBmp[MAXBITMAP];
HBITMAP hbWork;//いわゆるバックバッファ
HBITMAP hbMparts;//楽譜用ワーク
HBITMAP hbPan;//パンボリューム用ワーク

extern int gDrawDouble;	//両方のトラックグループを描画する

extern RECT WinRect; //ウィンドウサイズ保存用 A 2010.09.22
extern int NoteWidth;
extern int NoteEnlarge_Until_16px;
//GDIの初期化
BOOL StartGDI(HWND hwnd)
{
	HDC hdc;//デバイスコンテキスト
	BOOL status = FALSE;//この関数の返り値
    int nDesktopWidth = GetSystemMetrics( SM_CXFULLSCREEN );
    int nDesktopHeight = GetSystemMetrics( SM_CYFULLSCREEN );	//タスクバー考慮
    int nScreenWidth = GetSystemMetrics( SM_CXSCREEN );
    int nScreenHeight = GetSystemMetrics( SM_CYSCREEN );


	int nVirtualWidth = WinRect.right - WinRect.left;	//A 2010.09.22
	int nVirtualHeight =WinRect.bottom - WinRect.top;	//A 2010.09.22

	if(nVirtualWidth > nScreenWidth)nScreenWidth = nVirtualWidth;	//A 2010.09.22
	if(nVirtualHeight > nScreenHeight)nScreenHeight = nVirtualHeight;	//A 2010.09.22

	hdc = GetDC(hwnd);//DC取得
	//バックサーフェスを作るにあたる
	if((hbWork = CreateCompatibleBitmap(hdc,nScreenWidth,nScreenHeight)) == NULL){
		status = FALSE;
	}
	if((hbMparts = CreateCompatibleBitmap(hdc,nScreenWidth,144)) == NULL){
		status = FALSE;
	}
	if((hbPan = CreateCompatibleBitmap(hdc,nScreenWidth,144+16)) == NULL){
		status = FALSE;
	}
	ReleaseDC(hwnd,hdc);
	return(status);
}
//リサイズされたとき（失敗した関数）
BOOL ResizeGDI(HWND hwnd)
{
	if(hbWork != NULL)DeleteObject(hbWork);
	HDC hdc;//デバイスコンテキスト
	BOOL status = FALSE;//この関数の返り値

	hdc = GetDC(hwnd);//DC取得
	if((hbWork = CreateCompatibleBitmap(hdc,WWidth,WHeight)) == NULL){
		status = FALSE;
	}
	ReleaseDC(hwnd,hdc);
	return(status);

}
//GDIの開放
void EndGDI(void)
{
	int i;
	for(i = 0; i < MAXBITMAP; i++){
		if(hBmp[i] != NULL)DeleteObject(hBmp[i]);
	}
	if(hbWork != NULL)DeleteObject(hbWork);
	if(hbMparts != NULL)DeleteObject(hbMparts);
	if(hbPan != NULL)DeleteObject(hbPan);
}
//画像のロード(リソースから)
HBITMAP InitBitmap(char *name,int no)
{
	hBmp[no] = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		name,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);
	return hBmp[no];
}

//いわゆるフリップ
void RefleshScreen(HDC hdc)
{

	HDC hdcwork;//バックサーフェスのDC
	HBITMAP hbold;//過去のハンドルを保存
	
	hdcwork = CreateCompatibleDC(hdc);//DCの生成
	hbold = (HBITMAP)SelectObject(hdcwork,hbWork);//バックサーフェスを選択
	//表示(フリップ)
	BitBlt(hdc, 0, 0, WWidth, WHeight,hdcwork,0,0,SRCCOPY);
	SelectObject(hdcwork, hbold);//選択オブジェクトを元に戻す
	DeleteDC(hdcwork);//デバイスコンテキストの削除

}

void PutBitmap(long x,long y, RECT *rect, int bmp_no)
{
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hBmp[bmp_no]);

	BitBlt(toDC,x,y,rect->right - rect->left,
		rect->bottom - rect->top,fromDC,rect->left,rect->top,SRCCOPY);//表示

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}

void PutBitmapCenter16(long x,long y, RECT *rect, int bmp_no) //中心に描画する 2014.05.26
{
	if(rect->right - rect->left != 16 || NoteWidth == 16){
		PutBitmap(x, y, rect, bmp_no);
		return;
	}
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hBmp[bmp_no]);

	int ww = NoteWidth - 4;

	BitBlt(toDC,x     ,y,2   , rect->bottom - rect->top,  fromDC,  rect->left   ,rect->top,SRCCOPY);//表示
	BitBlt(toDC,x+2   ,y,ww  , rect->bottom - rect->top,  fromDC,  rect->left+2 ,rect->top,SRCCOPY);//表示
	BitBlt(toDC,x+2+ww,y,2   , rect->bottom - rect->top,  fromDC,  rect->left+14,rect->top,SRCCOPY);//表示

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}
///////////////////////////////////////////////
////以降はユニークば関数////////////////////////
///////////////////////////////////////////////
//楽譜のパーツ生成
bool MakeMusicParts(unsigned char line,unsigned char dot)
{
	if(line*dot==0)return false;
//	RECT m_rect[] = {
//		{  0,  0, 64,144},//鍵盤
//		{ 64,  0, 80,144},//小節ライン
//		{ 80,  0, 96,144},//一拍ライン
//		{ 96,  0,112,144},//1/16ライン
//	};
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbMparts);
	fromold = (HBITMAP)SelectObject(fromDC,hBmp[BMPMUSIC]);

	int x;
	if(org_data.track>=8)x=0;
	else x=0;

	for(int i = 0; i < (WWidth/NoteWidth)+15; i++){
		if(i%(line*dot) == 0)//線
//			BitBlt(toDC,i*16,0,16,192+WDWHEIGHTPLUS,fromDC,x+64,0,SRCCOPY);//表示
			BitBlt(toDC,i*NoteWidth,0,NoteWidth,WHeight+192-WHNM,fromDC,x+64,0,SRCCOPY);//表示
		else if(i%dot == 0)//破線
			BitBlt(toDC,i*NoteWidth,0,NoteWidth,WHeight+192-WHNM,fromDC,x+64+16,0,SRCCOPY);//表示
		else{
			if(NoteWidth>=8){
				BitBlt(toDC,i*NoteWidth,0,NoteWidth,WHeight+192-WHNM,fromDC,x+64+32,0,SRCCOPY);//表示
			}else{
				BitBlt(toDC,i*NoteWidth,0,NoteWidth,WHeight+192-WHNM,fromDC,x+64+32+1,0,SRCCOPY);//表示
			}
		}
	}

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
	return true;
}

void PutMusicParts(long x,long y)
{
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hbMparts);

	BitBlt(toDC,x,y,WWidth,WHeight+192-WHNM,fromDC,0,0,SRCCOPY);//表示

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}
//パン・ボリュームライン表示
void PutPanParts(void)
{
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hbPan);

	BitBlt(toDC,64,WHeight+288-WHNM,WWidth,WHeight+192-WHNM,fromDC,0,0,SRCCOPY);//表示

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}

void MakePanParts(unsigned char line,unsigned char dot)
{
//	RECT m_rect[] = {
//		{  0,  0, 64,144},//鍵盤
//		{ 64,  0, 80,144},//小節ライン
//		{ 80,  0, 96,144},//一拍ライン
//		{ 96,  0,112,144},//1/16ライン
//	};
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbPan);
	fromold = (HBITMAP)SelectObject(fromDC,hBmp[BMPPAN]);


//	for(int i = 0; i < 40; i++){
	for(int i = 0; i < (WWidth/NoteWidth)+15; i++){
		if(i%(line*dot) == 0)//線
			BitBlt(toDC,i*NoteWidth,0,NoteWidth,WHeight+192-WHNM+16,fromDC,64,0,SRCCOPY);//表示
		else if(i%dot == 0)//破線
			BitBlt(toDC,i*NoteWidth,0,NoteWidth,WHeight+192-WHNM+16,fromDC,64+16,0,SRCCOPY);//表示
		else {
			if(NoteWidth>=8){
				BitBlt(toDC,i*NoteWidth,0,NoteWidth,WHeight+192-WHNM+16,fromDC,64+32,0,SRCCOPY);//表示
			}else{
				BitBlt(toDC,i*NoteWidth,0,NoteWidth,WHeight+192-WHNM+16,fromDC,64+32+1,0,SRCCOPY);//表示
			}
		}
	}

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}

void PutSelectParts(void)
{
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hbPan);

	BitBlt(toDC,64,WHeight-16,WWidth,WHeight,fromDC,0,144,SRCCOPY);//表示

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);

}

//以下はチト特殊。音符を描くときのみに用いることとする。
HDC		Dw_hdc, Dw_toDC, Dw_fromDC;
HBITMAP Dw_toold, Dw_fromold;

void Dw_BeginToDraw(void)
{
	Dw_hdc = GetDC(hWnd);
	Dw_toDC   = CreateCompatibleDC(Dw_hdc);
	Dw_fromDC = CreateCompatibleDC(Dw_hdc);
	Dw_toold   = (HBITMAP)SelectObject(Dw_toDC,hbWork);
	Dw_fromold = (HBITMAP)SelectObject(Dw_fromDC,hBmp[BMPNOTE]);
	
}

void Dw_FinishToDraw(void)
{
	SelectObject(Dw_toDC,Dw_toold);
	SelectObject(Dw_fromDC,Dw_fromold);
	DeleteDC(Dw_toDC);
	DeleteDC(Dw_fromDC);
	ReleaseDC(hWnd,Dw_hdc);

}

void Dw_PutBitmap(long x,long y, RECT *rect, int bmp_no) //最後の引数は最早意味なし.
{
	if(NoteWidth == 16){
		BitBlt(Dw_toDC,x,y,rect->right - rect->left,
			rect->bottom - rect->top,Dw_fromDC,rect->left,rect->top,SRCCOPY);//表示
	}else if(NoteWidth >= 4){ //短縮の場合
		int ww = NoteWidth - 4;
		BitBlt(Dw_toDC,x,y,2, rect->bottom - rect->top,
			Dw_fromDC,rect->left,rect->top,SRCCOPY);//表示
		
		if(ww>0){
			BitBlt(Dw_toDC,x+2,y,ww, rect->bottom - rect->top,
				Dw_fromDC,rect->left + 2,rect->top,SRCCOPY);//表示
		}
		BitBlt(Dw_toDC,x+2+ww,y,2, rect->bottom - rect->top,
			Dw_fromDC,rect->left + 14,rect->top,SRCCOPY);//表示

	}
}

int Dw_PutBitmap_Head(long x,long y, RECT *rect, int bmp_no, int iNoteLength) //bmp_noは最早意味なし. iLengthは必ず1以上。 np->lengthをそのまま代入すること。
{
	int iTotalLength = NoteWidth * iNoteLength;
	int bitWidth = iTotalLength; if(bitWidth > 16)bitWidth = 16;
	if(NoteEnlarge_Until_16px == 0){bitWidth = NoteWidth; iTotalLength = bitWidth;}

	if(NoteWidth == 16 || iTotalLength >= 16){
		BitBlt(Dw_toDC,x,y,rect->right - rect->left,
			rect->bottom - rect->top,Dw_fromDC,rect->left,rect->top,SRCCOPY);//表示
		return 16;
	}else if(NoteWidth >= 4){ //短縮の場合
		int ww = bitWidth - 4;

		BitBlt(Dw_toDC,x,y,2, rect->bottom - rect->top,
			Dw_fromDC,rect->left,rect->top,SRCCOPY);//表示
		
		if(ww>0){
			BitBlt(Dw_toDC,x+2,y,ww, rect->bottom - rect->top,
				Dw_fromDC,rect->left + 2,rect->top,SRCCOPY);//表示
		}
		BitBlt(Dw_toDC,x+2+ww,y,2, rect->bottom - rect->top,
			Dw_fromDC,rect->left + 14,rect->top,SRCCOPY);//表示

	}
	return bitWidth;
}

//PAN, VOLに特化しているなー
void Dw_PutBitmap_Center(long x,long y, RECT *rect, int bmp_no) //最後の引数は最早意味なし.
{
	if(NoteWidth == 16){
		BitBlt(Dw_toDC,x,y,rect->right - rect->left,
			rect->bottom - rect->top,Dw_fromDC,rect->left,rect->top,SRCCOPY);//表示
	}else if(NoteWidth >= 4){ //短縮の場合
//		int ww = (16 - NoteWidth) / 2;
//		BitBlt(Dw_toDC,x ,y,rect->right - rect->left - 2 * ww,
//			rect->bottom - rect->top,Dw_fromDC,rect->left + ww,rect->top,SRCCOPY);//表示
		int ww = NoteWidth / 2;
		BitBlt(Dw_toDC,x ,y,
			ww,
			rect->bottom - rect->top,
			Dw_fromDC,rect->left ,rect->top,SRCCOPY);//表示

		//BitBlt(Dw_toDC,x+ww-1 ,y-3,
		//	2,
		//	rect->bottom - rect->top,
		//	Dw_fromDC,rect->left+7 ,rect->top,SRCCOPY);//表示

		BitBlt(Dw_toDC,x+ww ,y,
			ww,
			rect->bottom - rect->top,
			Dw_fromDC,rect->left+16-ww ,rect->top,SRCCOPY);//表示

		BitBlt(Dw_toDC,x+ww-1 ,y,
			2,
			rect->bottom - rect->top,
			Dw_fromDC,rect->left+7 ,rect->top,SRCCOPY);//表示

	}
}


