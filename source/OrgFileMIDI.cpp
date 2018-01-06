#include <string.h>
#include "Setting.h"
#include "Gdi.h"
#include "DefOrg.h"
#include "OrgData.h"
#include <stdio.h>
#include <string.h>
#include "Sound.h"
#include "resource.h"
#include "Scroll.h"
#include "rxoFunction.h"

char strMIDI_TITLE[256];
char strMIDI_AUTHOR[256];
unsigned char ucMIDIProgramChangeValue[MAXTRACK];

//MIDIのデルタタイム(TLast - TFirst)を計算し、バッファ*pに格納。 戻り値は書き込んだバイト長
int setDeltaTime(long TFirst, long TLast, unsigned char *p){
	unsigned long r, lDelta;
	unsigned long qbit[32], tbit[32], wbit[32];
	int i,j,k,flg;
	
	if(TLast > TFirst){
		lDelta = TLast - TFirst;
	}else{
		lDelta = TFirst - TLast;
	}

	if(lDelta <= 0x7F){ //1バイトで収まり、後続の面倒な計算はいらないので。
		*p = (unsigned char)(lDelta & 0x7F);
		return 1;
	}


	for(k=1,j=0,i=0; i<32; i++){
		if((i % 8) != 7){
			qbit[j] = k;
			j++;
		}
		tbit[i] = k;
		k = k * 2;
	}
	for(i=0;i<32;i++){
		wbit[i] = lDelta & tbit[i];
	}
	r = 0;
	for(i=0;i<32;i++){
		if(wbit[i] != 0){
			r = r | qbit[i];
		}
	}

	flg = 0;
	if((r & 0xFF000000) > 0){
		flg++;
		*p = (unsigned char)(0x80 | ((r / 0x1000000) & 0x7F)) ;
		p++;
	}
	if((r & 0xFF0000) > 0 || flg != 0){
		flg++;
		*p = (unsigned char)(0x80 | ((r / 0x10000) & 0x7F)) ;
		p++;
	}
	if((r & 0xFF00) > 0 || flg != 0){
		flg++;
		*p = (unsigned char)(0x80 | ((r / 0x100) & 0x7F)) ;
		p++;
	}
	flg++;
	*p = (unsigned char)(r & 0x7F);
	p++;

	return flg;
}


//標準MIDI形式で出力する
BOOL OrgData::ExportMIDIData(char *strMidiFileName, int iRepeat){
	long iDeltaTime = 60; //iDeltaTime < 127であるべし。
	unsigned char tmpuc;
	unsigned char *ucbuf,*p, lastVol = 100, *pBufLen;
	NOTELIST *np, *npStart;
	long CurrentX, LastX = 0, lenbuf;
	long  iUse,  iLeft;
	long  iUseTmp,  iLeftTmp;
	int rp, j;

	unsigned char flgNoteOff[MAXTRACK], flg89;
	//unsigned char NoteY[MAXTRACK] = {0,0,0,0, 0,0,0,0, 36, 38, 42, 46,  49, 50, 59, 66};
	unsigned char Convert_Wave_no_to_MIDI_no[] = { //タムはわかりやすいように99。
        36,36,38,38,99,
        42,46,57,76,77,
        35,99,35,36,40,
        38,42,46,42,46,
        57,55,51,99,99,
        35,81,58,36,36,
        40,69,38,99,46,
        42,39,75,70,36,
        60,42,
        
        36,36,36,36,36,36,36,36, //←今後ドラム音色を追加するときはここも修正すること。
	};
	unsigned char Tom_DrumMap[] = { //99にしたTomの味付け音色
		41, //0: Low Tom 2
		43, //1: Low Tom 1
		45, //2: Mid Tom 2
		47, //3: Mid Tom 1
		48, //4: High Tom 2
		50, //5: High Tom 1
	};
	unsigned char Tom_Threshold[] = { //Tomの味付け閾値（この値以上のとき、そのTomになる）
		0,  //0: Low Tom 2
		19, //1: Low Tom 1
		27, //2: Mid Tom 2
		34, //3: Mid Tom 1
		40, //4: High Tom 2
		47, //5: High Tom 1
	};
	unsigned char ucCurrentDrumKey[MAXTRACK];

	FILE *fp;
	if((fp=fopen(strMidiFileName,"wb"))==NULL){
		//MessageBox(hWnd,"ファイルにアクセスできません","Error (Export)",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_WARNING_ACCESS_FILE,IDS_ERROR_EXPORT,MB_OK);	// 2014.10.19 A
		return(FALSE);
	}
	//                                                                   ↓Tr数 + ConductorTrack + Dummy + Drum
	unsigned char strMIDIHeader[] = {'M', 'T', 'h', 'd', 0,0,0,6,  0,1,  0,  9+1+1  ,0,0};
	//４分音符の分解能
	strMIDIHeader[12] = (unsigned char)((info.dot * iDeltaTime / 0x100) & 0x7F);
	strMIDIHeader[13] = (unsigned char)((info.dot * iDeltaTime) & 0xFF);
	fwrite(strMIDIHeader, 14, 1, fp);

	unsigned char strConductorTrack[] = {'M','T','r','k',0,0,0,0x0B, 0x00, 0xFF, 0x51, 0x03,  0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
	//テンポ
	//long ltempo, invtempo;
	long invtempo, LastPan;
	int i, iUseCode90, isdl;
	unsigned char *pLast;

	//ltempo = 60 * 1000 / (info.wait * info.dot * iDeltaTime);
	//invtempo = 60 * 1000000 / (60 * 1000 / (info.wait * info.dot ));
	
	p = &strConductorTrack[8]; lenbuf = 8;

	//初期テンポ=999
	*p++=0x00; lenbuf++;
	*p++=0xFF; lenbuf++;
	*p++=0x51; lenbuf++;
	*p++=0x03; lenbuf++;
	invtempo = 25 * info.wait * info.dot ;

	tmpuc = (unsigned char)((invtempo / 0x10000) % 0x0100); *p++ = tmpuc; lenbuf++;
	tmpuc = (unsigned char)((invtempo / 0x100) % 0x0100);   *p++ = tmpuc; lenbuf++;
	tmpuc = (unsigned char)((invtempo / 0x1) % 0x0100);     *p++ = tmpuc; lenbuf++;

	//曲のタイトル表示
	tmpuc = (unsigned char)strlen(strMIDI_TITLE);
	if(tmpuc>0){
		*p++=0x00; lenbuf++;
		*p++=0xFF; lenbuf++;
		*p++=0x03; lenbuf++;
		*p++=0x00; pBufLen = p-1; lenbuf++;
		strcpy((char *)p, strMIDI_TITLE);
		lenbuf += tmpuc;
		*pBufLen = tmpuc;
		p+=tmpuc;
	}

	//著作権表示
	tmpuc = (unsigned char)strlen(strMIDI_AUTHOR);
	if(tmpuc>0){
		*p++=0x00; lenbuf++;
		*p++=0xFF; lenbuf++;
		*p++=0x02; lenbuf++;
		*p++=0x00; pBufLen = p-1; lenbuf++;
		//strcpy((char *)p, "(C)COMPOSER                "); lenbuf+=27; *pBufLen=27; p+=27;
		strcpy((char *)p, strMIDI_AUTHOR);
		lenbuf += tmpuc;
		*pBufLen = tmpuc;
		p+=tmpuc;
	}
	
	//任意テキスト
	*p++=0x00; lenbuf++;
	*p++=0xFF; lenbuf++;
	*p++=0x01; lenbuf++;
	*p++=0x00; pBufLen = p-1; lenbuf++;
	strcpy((char *)p, "                        "); //あらかじめ空白でクリア
	strcpy((char *)p, MessageString[IDS_STRING114]); //ここは "Exported by Organya Twei" しか入らない前提。
	lenbuf+=24; *pBufLen=24; p+=24;	// 2014.10.19 D


	//拍子
	*p++=0x00; lenbuf++;
	*p++=0xFF; lenbuf++;
	*p++=0x58; lenbuf++;
	*p++=0x04; lenbuf++;
	*p++=info.line; lenbuf++;
	*p++=0x02; lenbuf++;
	*p++=0x18; lenbuf++;
	*p++=0x08; lenbuf++;

/*	
	//GM System ON
	*p++=0x00; lenbuf++;
	*p++=0xF0; lenbuf++;
	*p++=0x05; lenbuf++;
	*p++=0x7E; lenbuf++;
	*p++=0x7F; lenbuf++;
	*p++=0x09; lenbuf++;
	*p++=0x01; lenbuf++;
	*p++=0xF7; lenbuf++;
*/
	//XG System ON
	/*
	*p++=0x00; lenbuf++;
	*p++=0xF0; lenbuf++;
	*p++=0x08; lenbuf++;
	*p++=0x43; lenbuf++;
	*p++=0x10; lenbuf++;
	*p++=0x4C; lenbuf++;
	*p++=0x00; lenbuf++;
	*p++=0x00; lenbuf++;
	*p++=0x7E; lenbuf++;
	*p++=0x00; lenbuf++;
	*p++=0xF7; lenbuf++;
*/

	//テンポ
	isdl = setDeltaTime((info.line - 1) * info.dot * iDeltaTime , 0, p); p+=isdl;  lenbuf+=isdl;
	*p++=0xFF; lenbuf++;
	*p++=0x51; lenbuf++;
	*p++=0x03; lenbuf++;
	//invtempo = 60 * 1000000 / (60 * 1000 / (info.wait * info.dot ));
	invtempo = 1000 * info.wait * info.dot ;
	tmpuc = (unsigned char)((invtempo / 0x10000) % 0x0100); *p++ = tmpuc; lenbuf++;
	tmpuc = (unsigned char)((invtempo / 0x100) % 0x0100);   *p++ = tmpuc; lenbuf++;
	tmpuc = (unsigned char)((invtempo / 0x1) % 0x0100);     *p++ = tmpuc; lenbuf++;

	//終端
	*p++=0x00; lenbuf++;
	*p++=0xFF; lenbuf++;
	*p++=0x2F; lenbuf++;
	*p++=0x00; lenbuf++;


	strConductorTrack[7] = (unsigned char)(lenbuf-8);
	fwrite(strConductorTrack, lenbuf, 1, fp);

	unsigned char strDummyTrack[] = {'M','T','r','k', 0,0,0,0x14,  0x00, 0xff, 0x03, 0x0C, 0x53, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x20, 0x53, 0x65, 0x74, 0x75, 0x70,  0x00,0xff,0x2f,0x00  };
	fwrite(strDummyTrack, 28, 1, fp);

	//unsigned char strDummyTrack[] = {0x4D, 0x54, 0x72, 0x6B, 0x00, 0x00, 0x00, 0x28, 0x00, 0xFF, 0x03, 0x0C, 0x53, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x20, 0x53, 0x65, 0x74, 0x75, 0x70, 0x00, 0xF0, 0x05, 0x7E, 0x7F, 0x09, 0x01, 0xF7, 0x01, 0xF0, 0x08, 0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00, 0xF7, 0x8D, 0x10, 0xFF, 0x2F, 0x00};

	/*
	//MTrk : System Setup
	//Length 172=0xA3
	//FF 21 01 pp : 出力ポート指定 (現在のSMFでは未定義) 0
	//146:GM システムオン,  141:GS Reset
	//マスタＶＯＬ
	//ドラム追加
	unsigned char strDummyTrack[] ={
		0x4D, 0x54, 0x72, 0x6B, 
		0x00, 0x00, 0x00, 0xAC, 
		0x00, 0xFF, 0x03, 0x0C, 0x53, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x20, 0x53, 0x65, 0x74, 0x75, 0x70, 
		0x00, 0xFF, 0x21, 0x01, 0x00,

		0x00, 0xF0, 0x05, 0x7E, 0x7F, 0x09, 0x01, 0xF7, 
		0x01, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7F, 0x00, 0x41, 0xF7,
		0x01, 0xF0, 0x07, 0x7F, 0x7F, 0x04, 0x01, 0x00, 0x7F, 0xF7,

		0x00, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x11, 0x15, 0x00, 0x1A, 0xF7, 
		0x00, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x12, 0x15, 0x00, 0x19, 0xF7, 
		0x00, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x13, 0x15, 0x00, 0x18, 0xF7, 
		0x00, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x14, 0x15, 0x00, 0x17, 0xF7, 
		0x00, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x15, 0x15, 0x00, 0x16, 0xF7, 
		0x01, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x16, 0x15, 0x00, 0x15, 0xF7, 
		0x00, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x17, 0x15, 0x00, 0x14, 0xF7, 
		0x00, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x18, 0x15, 0x00, 0x13, 0xF7, 
		0x00, 0xF0, 0x0A, 0x41, 0x10, 0x42, 0x12, 0x40, 0x19, 0x15, 0x01, 0x11, 0xF7,
		0x01, 0xFF, 0x2F, 0x00,
	};
	fwrite(strDummyTrack, 8+0xAC, 1, fp);
	*/
	for(i=0;i<MAXMELODY ;i++){
		//if(NULL == info.tdata[i].note_list)continue;
		GetNoteUsed(&iUse, &iLeft, i);

		ucbuf = (unsigned char *)calloc(iUse * 2 * 16 * iRepeat + 1024, sizeof(char));
		
		//memset(ucbuf, 0, iUse * 2 * 16 * iRepeat + 1024); //ゼロクリア callocでもいいのでは。
		ucbuf[0]= 'M'; 
		ucbuf[1]= 'T'; 
		ucbuf[2]= 'r'; 
		ucbuf[3]= 'k';
		p = &ucbuf[8];
		lenbuf = 0;

		//シーケンス名(曲タイトル)・トラック名 	
		*p++ = 0x00;
		*p++ = 0xFF;
		*p++ = 0x03; pBufLen = p;
		*p++ = 0x07; //Length = 7
		lenbuf += 4;
		if(i<MAXMELODY){
			strcpy((char *)p, "TRACK_"); p+=6;
			*p++ = '1' + i;
			lenbuf += (6 + 1);
			if(NULL == info.tdata[i].note_list){
				strcpy((char *)p, " empty"); p+=6;
				*pBufLen = (*pBufLen) + 6;
				lenbuf += 6;
			}
		}else{
			strcpy((char *)p, "----"); p+=4;
			*pBufLen = 4;
			lenbuf += 4;
			*p++ = 0x00;
			*p++ = 0xFF;
			*p++ = 0x01;
			*p++ = 21;
			lenbuf += 4;
			strcpy((char *)p, "---- EMPTY TRACK ----"); p+=21;
			lenbuf += 21;
		}


		//reset All Control
		*p++=0x00;
		*p++=0xB0 + i;
		*p++=0x79;
		*p++=0x00;
		lenbuf += 4;

		//音量(VOL)
		*p++=0x04;
		*p++=0xB0 + i;
		*p++=0x07;
		*p++=0x64;
		lenbuf += 4;

		//リバーブ 0 
		*p++=0x00;
		*p++=0xB0 + i;
		*p++=0x5B;
		*p++=0x00;
		lenbuf += 4;

		//コーラス 0 
		*p++=0x00;
		*p++=0xB0 + i;
		*p++=0x5D;
		*p++=0x00;
		lenbuf += 4;

		//音量(Exp)
		*p++=0x04;
		*p++=0xB0 + i;
		*p++=0x0B;
		*p++=0x7F;
		lenbuf += 4;

		//音色(強引 ^^; ...)
		*p++=0x04;
		*p++=0xC0 + i;
		//ダイアログでucMIDIProgramChangeValue[i]が適切に設定されているはず。
		if(ucMIDIProgramChangeValue[i]<128){
			*p++ = ucMIDIProgramChangeValue[i];
		}else{
			*p++ = info.tdata[i].wave_no;
		}
		lenbuf += 3;

		//ピッチベンド En mm ll (3byte)
        //チャンネルnに対し、ピッチベンド値llmmを送信する (さりげなくlittle endianなので注意)。 ピッチベンドが-8192,0,8191に対応する値llmmはそれぞれ 0x0000, 0x4000, 0x7F7F となる。 
		long lPvnd = (info.tdata[i].freq - 1000) + 0x4000;
		int iFlgOfOverRepeatPoint;
		*p++=0x04;
		*p++=0xE0 + i;
		*p++=(unsigned char)(lPvnd & 0xFF);
		*p++=(unsigned char)((lPvnd >> 8) & 0xFF);
		lenbuf += 4;

		LastX = -(info.line * info.dot * iDeltaTime - 4 - 4 - 4 - 4); lastVol = 100; iUseCode90 = 0;
		LastPan = -99999;
		
		npStart = info.tdata[i].note_list;
		rp = 0;
		iFlgOfOverRepeatPoint = 0;

		for(np = info.tdata[i].note_list; rp < iRepeat ; ){
			if(np == NULL || np->x >= info.end_x){
				if(rp < iRepeat - 1){
					rp++;
					np = npStart;
					LastX -=(info.end_x - info.repeat_x)* iDeltaTime;
					//np = npStart->to;
					continue;
				}else{
					break;
				}
			}
			
			if(np->x >= info.repeat_x && iFlgOfOverRepeatPoint == 0){ //リピート開始点を通過した最初のX
				npStart = np;
				iFlgOfOverRepeatPoint = 1;
			}
			if(np->y != KEYDUMMY ){
				pLast = p;
				if(LastPan != (long)np->pan && np->pan != PANDUMMY){ //PAN変更
					CurrentX = np->x * iDeltaTime; 
					isdl = setDeltaTime(CurrentX, LastX, p);
					p+=isdl;
					LastX = CurrentX;
					iUseCode90 = 0;
					*p++ = (unsigned char)(0xB0 + i);
					*p++ = 0x0A; // Pan
					long ipan = (np->pan - 6) * 63 ; ipan = ipan / 6; ipan += 64;
					*p++ = (unsigned char)(ipan);
					LastPan = np->pan;

				}
				//xx = np->x - LastX;
				CurrentX = np->x * iDeltaTime;
				isdl = setDeltaTime(CurrentX, LastX, p);
				LastX = CurrentX;

				p+=isdl;
				//*p++ = (unsigned char)xx;
				if(iUseCode90 == 0){
					*p++ = (unsigned char)(0x90 + i);
					iUseCode90 = 1;
				}
				*p++ = 12 + np->y;
				if(np->volume != VOLDUMMY){
					*p++ = (unsigned char)(np->volume / 2);
					lastVol = (np->volume / 2);
				}else{
					*p++ = lastVol;
				}
				//xx = np->length;
				//if(xx>=128){ //雑。デルタタイムはメンドイ。
					//*p++ = (unsigned char)(xx/128)|0x80;
					//*p++ = (unsigned char)xx%128;
				//}else{
				//	*p++ = (unsigned char)xx;
				//}
				long Amari;
				Amari = np->x + np->length - info.end_x;
				if(Amari < 0)Amari = 0;

				CurrentX = (np->x + np->length - Amari ) * iDeltaTime - (info.tdata[i].pipi == 0 ? 0 : iDeltaTime / 3 );
				isdl = setDeltaTime(CurrentX, LastX, p);
				p+=isdl;
				LastX = CurrentX;

				if(iUseCode90 == 0){
					*p++ = (unsigned char)(0x90 + i);
					iUseCode90 = 1;
				}
				*p++ = 12 + np->y;
				*p++ = 0; // 音を消す 
				lenbuf += (p - pLast);
			}
			np = np->to;
		}
		
		*p++=0x00;
		*p++=0xFF;
		*p++=0x2F;
		*p++=0x00;
		lenbuf += 4;

		//ブロック長
		tmpuc = (unsigned char)((lenbuf / 0x1000000) & 0xFF);
		ucbuf[4] = tmpuc;
		tmpuc = (unsigned char)((lenbuf / 0x10000) & 0xFF);
		ucbuf[5] = tmpuc;
		tmpuc = (unsigned char)((lenbuf / 0x100) & 0xFF);
		ucbuf[6] = tmpuc;
		tmpuc = (unsigned char)((lenbuf / 0x1) & 0xFF);
		ucbuf[7] = tmpuc;

		fwrite(ucbuf, lenbuf + 8, 1, fp);

		free(ucbuf);
	}

	unsigned char *bufv;
	long x, FinalNoteX;
	int bufsize = 256 * info.line * info.dot * 8 * 2, w, t;
	
	bufv = (unsigned char *)calloc(bufsize, sizeof(char));
	//memset(bufv, 0, bufsize);
	//FinalNoteX = 0;
	
	
	iUse = 0;
	for(t=0, i=MAXMELODY; i<MAXTRACK; t++, i++){
		GetNoteUsed(&iUseTmp, &iLeftTmp, i);
		if(iUseTmp > iUse)iUse = iUseTmp;
		for(np = info.tdata[i].note_list; np != NULL ; np = np->to){
			if(np->y != KEYDUMMY && np->volume>0 && np->volume<VOLDUMMY){
				w = t * 256 * info.line * info.dot * 2 + np->x * 2 ;
				*(bufv + w) = np->volume;
				*(bufv + w + 1) = np->y; //Tomの高さが知りたいので
				//if(np->x > FinalNoteX) FinalNoteX = np->x;
			}
		}
	}
	FinalNoteX = info.end_x;

	ucbuf = (unsigned char *)calloc(iUse * 2 * 8 * 16 * iRepeat + 1024, sizeof(char));
	//memset(ucbuf, 0, iUse * 2 * 8 * 16 * iRepeat + 1024); //ゼロクリア callocでもいいのでは。
	ucbuf[0]= 'M'; ucbuf[1]= 'T'; ucbuf[2]= 'r'; ucbuf[3]= 'k';
	p = &ucbuf[8];
	lenbuf = 0;
	//シーケンス名(曲タイトル)・トラック名 	
	*p++ = 0x00;
	*p++ = 0xFF;
	*p++ = 0x03; pBufLen = p;
	*p++ = 0x07; //Length = 7
	lenbuf += 4;
	strcpy((char *)p, "DRUMS"); p+=5;
	*pBufLen = 5;
	lenbuf += 5;

	//出力ポート指定
	*p++=0x00;
	*p++=0xFF;
	*p++=0x21;
	*p++=0x01;
	*p++=0x00;
	lenbuf += 5;

	//reset All Control
	*p++=0x04;
	*p++=0xB9;
	*p++=0x79;
	*p++=0x00;
	lenbuf += 4;

	//音量(VOL)
	*p++=0x04;
	*p++=0xB9; //B0 + 9で Aポート 10チャネル = Drum Set
	*p++=0x07;
	*p++=0x64;
	lenbuf += 4;

	//リバーブ 80 
	*p++=0x00;
	*p++=0xB9;
	*p++=0x5B;
	*p++=0x50;
	lenbuf += 4;

	//音量(Exp)
	*p++=0x04;
	*p++=0xB9;
	*p++=0x0B;
	*p++=0x7F;
	lenbuf += 4;
	LastX = -(info.line * info.dot * iDeltaTime - 4 - 4 - 4);
	
	for(i=0;i<MAXTRACK;i++){
		flgNoteOff[i] = 0;
		ucCurrentDrumKey[i] = 0;
	}
	rp = 0;
	for(x = 0 ; x <= FinalNoteX+1 ; x++){
		if(x == info.end_x){
			x = info.repeat_x;
			rp++;
			if(rp >= iRepeat)break;
			x--; //for処理で x++されるので。
			LastX -=(info.end_x - info.repeat_x)* iDeltaTime;
			continue;
		}
		//break;
		pLast = p;
		flg89 = (unsigned char)(1 * iDeltaTime);
		for(t=0, i=MAXMELODY; i<MAXTRACK; t++, i++){
			if(flgNoteOff[i] > 0){
				*p++ = flg89;
				*p++ = 0x89;
				*p++ = ucCurrentDrumKey[i];
				*p++ = 0x00;
				flg89 = 0;
				flgNoteOff[i] = 0;
			}
		}
		if(flg89 == 0){
			LastX+= 1 * iDeltaTime;
		}

		for(t=0, i=MAXMELODY; i<MAXTRACK; t++, i++){
			w = t * 256 * info.line * info.dot * 2 + x * 2 ;
			if( *(bufv+w) > 0){
				CurrentX = x * iDeltaTime;
				isdl = setDeltaTime(CurrentX, LastX, p);
				p+=isdl;
				LastX = CurrentX;
				ucCurrentDrumKey[i] = Convert_Wave_no_to_MIDI_no[info.tdata[i].wave_no];
				if(ucCurrentDrumKey[i] == 99){ //Tomのとき
					ucCurrentDrumKey[i] = Tom_DrumMap[0];
					for(j = 1; j< 6; j++){
						if( (*(bufv+w+1)) >= Tom_Threshold[j]) ucCurrentDrumKey[i] = Tom_DrumMap[j];
					}
				}else if(ucCurrentDrumKey[i] == 57){ //Crash Symbal 2
					if((*(bufv+w+1)) < 36)ucCurrentDrumKey[i] = 49; //Crash Symbal 1(低いほうにする)
				}
				*p++ = 0x99;
				*p++ = ucCurrentDrumKey[i];
				*p++ = (unsigned char)((*(bufv+w)) / 2); //velo
				//*p++ = 0x64; //velo
				flgNoteOff[i] = 1;
			}
		}
		lenbuf += (p - pLast);
	}
	//終端
	*p++=0x00;
	*p++=0xFF;
	*p++=0x2F;
	*p++=0x00;
	lenbuf += 4;


	//ブロック長
	tmpuc = (unsigned char)((lenbuf / 0x1000000) & 0xFF);
	ucbuf[4] = tmpuc;
	tmpuc = (unsigned char)((lenbuf / 0x10000) & 0xFF);
	ucbuf[5] = tmpuc;
	tmpuc = (unsigned char)((lenbuf / 0x100) & 0xFF);
	ucbuf[6] = tmpuc;
	tmpuc = (unsigned char)((lenbuf / 0x1) & 0xFF);
	ucbuf[7] = tmpuc;

	fwrite(ucbuf, lenbuf + 8, 1, fp);

	free(ucbuf);
	free(bufv);

  

	fclose(fp);
	return TRUE;
}
