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

//音符リスト構造体
typedef struct NOTELIST{
	NOTELIST *from;//前のアドレス
	NOTELIST *to;//次のアドレス
	long x;//位置
	unsigned char length;//音の長さ
	unsigned char y;//音の高さ
	unsigned char volume;//ヴォリューム
	unsigned char pan;//パン
}NOTELIST;
//トラックデータ＊８
typedef struct{
	unsigned short freq;//+α周波数(1000がDefault)
//	unsigned short note_num;//音符の数
	unsigned char wave_no;//波形No
	char pipi;
	NOTELIST *note_p;//NoteData領域の先頭アドレス
	NOTELIST *note_list;//リストの先頭アドレス1
}TRACKDATA;
//曲単位で持つ固有の情報
typedef struct MUSICINFO{
	unsigned short wait;
	unsigned char line;//１小節のライン数
	unsigned char dot;//一泊のドット数
	unsigned short alloc_note;//確保した音符の数
	long repeat_x;//リピート
	long end_x;//曲の終わり(リピートに戻る)
	TRACKDATA tdata[16];
}MUSICINFO;
//メインクラス。このアプリケーションの中心。（クラスってやつを初めて使う）
typedef struct OrgData{
	OrgData();//コンストラクタ
	~OrgData();//デストラクタ
	private:
		MUSICINFO info;
	public:
		//unsigned char *noteon;
		BOOL SetNote_afterSetLength(long x);
		BOOL SetNote_onlyLength(long x, long Length);

		void RedrawSelectArea(void);
		//アンドゥ用
		int ReplaceFromRedoData();
		int ReplaceFromUndoData();
		void ClearUndoData();
		int SetUndoData();
		int ResetLastUndo(void); //最後のアンドゥはなかったことに！（空クリックだったとか）
		MUSICINFO ud_tdata[32]; //とりあえず、アンドゥは32回
		NOTELIST  ud_note[32][16][4096];
		bool UndoEnable;
		bool RedoEnable;
		int CurrentUndoCursor; //この番号に書き込む
		int MinimumUndoCursor; //この番号までならさかのぼれる
		int MaximumUndoCursor; //この番号までならやり直せる（リドゥ可能）

		char track;
		char mute[MAXTRACK];
		unsigned char def_pan[MAXTRACK];
		unsigned char def_volume[MAXTRACK];
		void InitOrgData(void);
		void GetMusicInfo(MUSICINFO *mi);//曲情報を取得
		//曲情報を設定。flagは設定アイテムを指定
		BOOL SetMusicInfo(MUSICINFO *mi,unsigned long flag);
		BOOL NoteAlloc(unsigned short note_num);//指定の数だけNoteDataの領域を確保
		void ReleaseNote(void);//NoteDataを開放
		NOTELIST *SearchNote(NOTELIST *np);//未使用NOTEの検索
		void OrgData::TouchKeyboard(unsigned char y);//鍵盤に触れる
		BOOL SetNote(long x,unsigned char y, int DragMode = 0);//音符を配置
		BOOL CutNote(long x,unsigned char y);//音符のカット
		BOOL SetPan(long x,unsigned char y);//パン音符を配置
		BOOL SetPan2(long x,unsigned char y);//パン音符を配置
		BOOL CutPan(long x,unsigned char y);//パン音符のカット
		BOOL SetVolume(long x,unsigned char y);//ボリューム音符を配置
		BOOL SetVolume2(long x,unsigned char y,long fade); //フェードアウト	// 2010.08.17 A
		BOOL CutVolume(long x,unsigned char y);//ボリューム音符のカット
		BOOL EnlargeAllNotes(int Power);//[新]音符をPower倍に引き伸ばす
		BOOL ShortenAllNotes(int Power);//[新]音符を1/Power倍に縮こめる

		void SortNotes(void);	//音符の整理
		int SearchNote(long x,unsigned char y, int YuusenTrack);//どのトラックにある音符か？の検索
		int SearchNoteB(long x,unsigned char y, long *lStartx, long *lLastx, int YuusenTrack);//どのトラックにある音符か？の検索 2014.05.02
		int SearchNoteC(long x,unsigned char y, long xWidth, long xMod);//縮小表示時の検索。
		

		void GetNoteUsed(long *use,long *left,char track);//音譜残量等
		//以下は表示
		void PutNotes(int TPCY=0);
		void PutNotes2(int TPCY=0);
		void PutMusic(void);
		void PutNumber(void);
		void PutRepeat(void);
		bool PutBackGround(void);	//背景を再描画
		void PutSelectArea(void);
		//以下は再生
		void PlayData(void);
		void SetPlayPointer(long x);//再生ポインターを指定の位置に設定
		//以下はファイル関係
		unsigned short GetNoteNumber(char track,NOTECOPY *nc);
		BOOL OrgData::SaveMusicData(void);
		BOOL OrgData::LoadMusicData(void);
		int FileCheckBeforeLoad(char *checkfile); //ファイルがロード可能であれば0異常であれば1を返す。ﾃﾞｰﾀロードはされない。 2014.05.22
		//以下は編集関係
		BOOL DelateNoteData(PARCHANGE *pc);
		BOOL CopyNoteData(NOTECOPY *nc);
		BOOL ChangePanData(PARCHANGE *pc);
		BOOL ChangeTransData(PARCHANGE *pc);
		BOOL ChangeVolumeData(PARCHANGE *pc);
		BOOL CheckNoteTail(char track);

		BOOL EnsureEmptyArea(PARCHANGE *pc, int Function); //[新]空白を埋める 2014.05.01
		BOOL SwapTrack(NOTECOPY *pc); //[新]トラックを入れ替える 2014.05.10
		BOOL ExportMIDIData(char *strMidiFileName, int iRepeat); //MIDIｴｸｽﾎﾟｰﾄ,繰り返し回数 2014.05.11



	bool CopyNoteDataToCB(NOTECOPY *nc, int iTrack, int iFullTrack);
}ORGDATA;
extern ORGDATA org_data;