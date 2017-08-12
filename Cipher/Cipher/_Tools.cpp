#include "all.h"

// ---- Progress ----

#define P_MAX 79

static int P_Shown;

static void ProgressBgn(void)
{
	P_Shown = 0;
}
static void Progress(double rate)
{
	int p = m_d2i(rate * P_MAX);

	while(P_Shown < p)
	{
		cout("*");
		P_Shown++;
	}
}
static void ProgressEnd(void)
{
	Progress(1.0);
	cout("\n");
}

// ----

static void ShowBin(uchar *bin, int size, char *nameForUi)
{
	cout("%s: ", nameForUi);

	for(int index = 0; index < size; index++)
		cout("%02x", bin[index]);

	cout("\n");
}
static int IsHex128(char *str)
{
	if(strlen(str) != 128)
		return 0;

	for(char *p = str; *p; p++)
		if(!m_isHex(*p))
			return 0;

	return 1;
}

int XEnabled;
int XChr = 'x';
int XExp = 30;
int XExInfoSz; // out, XEnabled == 0 のときは 0
uchar *XExInfo; // out, bind, XEnabled == 0 のときは NULL

uchar *GetRawKey(char *str, int cphrMode, char *rFile)
{
	errorCase(!str);
	// cphrMode
	errorCase(m_isEmpty(rFile));

	XExInfoSz = 0;
	memFree(XExInfo);
	XExInfo = NULL;

	static uchar rawKey[64];

	cout("pwd: %s\n", str);

	if(IsHex128(str))
	{
		for(int index = 0; index < 64; index++)
			rawKey[index] = m_c2i(str[index * 2]) * 16 + m_c2i(str[index * 2 + 1]);

		if(XEnabled)
			cout("IGNORE X-CHR X-EXT OPTION!\n");
	}
	else
	{
		sha512_t *ctx = sha512_create();
		sha512_update(ctx, str, strlen(str));

		if(XEnabled)
		{
			// rand-byte x 64
			{
				XExInfoSz = 64;
//				XExInfoSz = 64 + 2; // old
				XExInfo = na(uchar, XExInfoSz);

				if(cphrMode == CPHR_ENC)
				{
					for(int c = 0; c < 64; c++)
						XExInfo[c] = getCryptoByte();

//					XExInfo[64] = XChr; // old
//					XExInfo[65] = XExp; // old
				}
				else // CPHR_DEC
				{
					__int64 fileSize = getFileSize(rFile);

					if(XExInfoSz <= fileSize)
					{
						FILE *fp = fileOpen(rFile, "rb");
						fileSeek(fp, SEEK_SET, fileSize - XExInfoSz);
						fileRead(fp, XExInfo, XExInfoSz);
						fileClose(fp);

//						XChr = XExInfo[64]; // old
//						XExp = XExInfo[65]; // old
					}
					else
						cout("NO-X-EX-INFO!\n");
				}
				ShowBin(XExInfo, XExInfoSz, "XExInfo");
				sha512_update(ctx, XExInfo, 64);
			}

			errorCase_m(!m_isRange(XChr, 0x00, 0xff), "パスフレーズの拡張に問題があります。(X-CHR=0x00～0xff)");
			errorCase_m(!m_isRange(XExp, 10, 40), "パスフレーズの拡張に問題があります。(X-EXT=10～40)");

			cout("XChr: 0x%02x\n", XChr);
			cout("XExp: %d\n", XExp);

			// x-chr x 2^x-ext
			{
				uchar *xPtn = (uchar *)memAlloc(1024);

				for(int c = 0; c < 1024; c++)
					xPtn[c] = XChr;

				int xPtnLen = 1024;
				int xNum = 1;

				for(int c = 0; c < XExp - 10; c++)
					xNum *= 2;

				cout("passphrase expanding...\n");
				ProgressBgn();

				uint bgnTm = GetTickCount();
				uint endTm;

				for(int c = 0; c < xNum; c++)
				{
					Progress((double)c / xNum);
					sha512_update(ctx, xPtn, xPtnLen);
				}
				endTm = GetTickCount();

				ProgressEnd();
				cout("time: %u\n", endTm - bgnTm);
				cout("passphrase expanded!\n");

				memFree(xPtn);
			}
		}
		sha512_makeHash(ctx);
		memcpy(rawKey, sha512_hash, 64);
		sha512_release(ctx);
	}
	ShowBin(rawKey, 64, "CIPHER-KEY");
	return rawKey;
}
