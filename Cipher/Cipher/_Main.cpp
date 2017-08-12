#include "all.h"

int main(int argc, char **argv)
{
	int removeRFileFlag = 0;
	CphrMode_t cphrMode = CPHR_NONE;
	char *passphrase = NULL;
	char *rFile = NULL;
	char *wFile = NULL;

readArgs:
	if(argIs("/MP"))
	{
		int pwLen = 22;

		if(hasArgs(1))
			pwLen = atoi(nextArg());

		char *pw = MkPassword(pwLen);
		cout("%s\n", pw);
		memFree(pw);

		SilentMode = 1; // exitcode: 0 ��\�������Ȃ����߁B
		termination(EXITCODE_NORMAL);
	}
	if(argIs("/MK"))
	{
		char *pk = MkPassKey();
		cout("%s\n", pk);
		memFree(pk);

		SilentMode = 1; // exitcode: 0 ��\�������Ȃ����߁B
		termination(EXITCODE_NORMAL);
	}
	if(argIs("/B"))
	{
		BatchMode = 1;
		goto readArgs;
	}
	if(argIs("/S"))
	{
		SilentMode = 1;
		goto readArgs;
	}
	if(argIs("/R"))
	{
		removeRFileFlag = 1;
		goto readArgs;
	}
	if(argIs("/E"))
	{
		cphrMode = CPHR_ENC;
		goto readArgs;
	}
	if(argIs("/D"))
	{
		cphrMode = CPHR_DEC;
		goto readArgs;
	}
	if(argIs("/P"))
	{
		passphrase = nextArg();
		goto readArgs;
	}
	if(argIs("/PE"))
	{
		XEnabled = 1;
		XChr = PrmToChr(nextArg());
		XExp = atoi(nextArg());
		goto readArgs;
	}
	if(hasArgs(1))
	{
		rFile = getFullPath(nextArg());
	}
	if(hasArgs(1))
	{
		wFile = getFullPath(nextArg());
	}

	errorCase_m(hasArgs(1), "�s���ȃR�}���h�������w�肳��܂����B");
//	errorCase_m(hasArgs(1), "�����ł��Ȃ��R�}���h�������w�肳��܂����B");
//	errorCase_m(hasArgs(1), "�����ł��Ȃ��R�}���h�������w�肳��܂����B\n(�R�}���h�����̏��������ւ��邱�Ƃ͂ł��܂���)"); // old
	errorCase_m(!passphrase, "�p�X�t���[�Y(�܂��͌�)���w�肳��܂���ł����B");
	errorCase_m(!existFile(rFile), "���̓t�@�C���ɃA�N�Z�X�ł��܂���B");

	if(cphrMode == CPHR_NONE)
	{
		if(_stricmp(getExt(rFile), ENCFILE_EXT)) // ? rFile != enc-file
			cphrMode = CPHR_ENC;
		else
			cphrMode = CPHR_DEC;
	}
	if(!wFile)
	{
		if(cphrMode == CPHR_ENC)
		{
			wFile = xcout("%s.%s", rFile, ENCFILE_EXT);
		}
		else
		{
			wFile = strx(rFile);

			if(!removeExt(wFile))
			{
				wFile = addToken_x(wFile, xcout(".%s", DECFILE_EXT));
			}
		}
	}

	cout("removeRFileFlag: %d\n", removeRFileFlag);
	cout("cphrMode: %d\n", cphrMode);
	cout("< %s\n", rFile);
	cout("> %s\n", wFile);

	errorCase_m(!mbs_stricmp(rFile, wFile), "���̓t�@�C���Əo�̓t�@�C���������ł��B"); // ? rFile == wFile

	if(existFile(wFile))
	{
		if(!BatchMode)
		{
			if(MessageBox(
				NULL,
				"�o�̓t�@�C���͊��ɑ��݂��܂��B\n�㏑�����܂���?",
				"�㏑���̊m�F",
				MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST
				) != IDOK
				)
			{
				cout("�L�����Z�����܂����B\n");
				termination(EXITCODE_USER_CANCEL);
			}
		}
		cout("�㏑�����܂��B\n");
	}

	if(!Cipher(rFile, removeRFileFlag, wFile, cphrMode, GetRawKey(passphrase, cphrMode, rFile)))
	{
		LOGPOS();
		errorCase(cphrMode == CPHR_ENC); // 2bs

		if(!BatchMode)
			MessageBox(
				NULL,
				"�����ł��܂���ł����B\n���܂��̓p�X�t���[�Y���Ⴄ���t�@�C�������Ă��܂��B",
				"�������s",
				MB_OK | MB_ICONSTOP | MB_TOPMOST
				);

		remove(wFile);
		termination(EXITCODE_DECRYPTION_ERROR);
	}
	termination(EXITCODE_NORMAL);
	return 0; // dummy
}
