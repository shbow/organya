
#define DIRECTION_UP	0
#define DIRECTION_DOWN	1
#define DIRECTION_LEFT	2
#define DIRECTION_RIGHT	3

typedef struct ScrollData{
	private:
		long hpos;//横スクロール値
		long vpos;//縦スクロール値
		SCROLLINFO scr_info;
		long vScrollMax;
	public:
		void SetIniFile();
		BOOL InitScroll(void);//初期化
		void VertScrollProc(WPARAM wParam);//スクロールさせた時の処理
		void HorzScrollProc(WPARAM wParam);
		void WheelScrollProc(LPARAM lParam, WPARAM wParam); //ホイールを...
		void GetScrollPosition(long *hp,long *vp);
		void SetHorzScroll(long x);
		void AttachScroll(void);	//ウィンドウサイズ変更に応じてスクロールを修正
		void KeyScroll(int iDirection); //キー操作によるスクロール用
		void PrintHorzPosition(void);
		void ChangeVerticalRange(int WindowHeight = -1); //ウィンドウサイズに応じてスクロールバーRangeを変更
}SCROLLDATA;
extern SCROLLDATA scr_data;//スクロールデータ
