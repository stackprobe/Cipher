#include "all.h"

static void AddPadding(char *file)
{
	__int64 fileSize = getFileSize(file);
	int padSzLow = (16 - (int)(fileSize & 0x0f)) & 0x0f;
	int padSize;

	do
	{
		padSize = padSzLow | (getCryptoByte() & 0xf0);
	}
	while(padSize < 1 || fileSize + padSize < 240);

	cout("padSize: %d\n", padSize);
	FILE *fp = fileOpen(file, "ab");

	for(int count = padSize - 1; count; count--)
	{
		writeChar(fp, getCryptoByte());
	}
	writeChar(fp, padSize);
	fileClose(fp);
}
static int UnaddPadding(char *file) // ret: ? successful
{
	__int64 size = getFileSize(file);

	if(size < 1)
	{
		LOGPOS();
		return 0;
	}
	FILE *fp = fileOpen(file, "rb");

	fileSeek(fp, SEEK_SET, size - 1);
	int padSize = readChar(fp);
	cout("padSize: %d\n", padSize);

	fileClose(fp);

	errorCase(padSize < 1 || 255 < padSize);
	
	if(size < padSize)
	{
		LOGPOS();
		return 0;
	}
	size -= padSize;
	setFileSize(file, size);
	return 1;
}

#define RANDPART_SIZE 64
#define IV_SIZE 16 // == BLOCK_SIZE

static void AddRandPart(char *file, int size)
{
	FILE *fp = fileOpen(file, "ab");

	for(int count = size; count; count--)
		writeChar(fp, getCryptoByte());

	fileClose(fp);
}
static int UnaddRandPart(char *file, int size) // ret: ? successful
{
	__int64 fileSize = getFileSize(file);

	if(fileSize < size)
	{
		LOGPOS();
		return 0;
	}
	setFileSize(file, fileSize - size);
	return 1;
}

#define HASH_SIZE 64

static void PrintHash(uchar hash[HASH_SIZE], char *hashName)
{
	cout("%s: ", hashName);

	for(int index = 0; index < HASH_SIZE; index++)
		cout("%02x", hash[index]);

	cout("\n");
}
static void AddHash(char *file)
{
	sha512_t *ctx = sha512_create();
	sha512_update(ctx, file, 0, getFileSize(file));
	sha512_makeHash(ctx);
	sha512_release(ctx);

	PrintHash(sha512_hash, "hash");

	writeBlock(file, sha512_hash, HASH_SIZE, 1);
}
static int UnaddHash(char *file) // ret: ? successful
{
	__int64 size = getFileSize(file);

	if(size < HASH_SIZE)
	{
		LOGPOS();
		return 0;
	}
	sha512_t *ctx = sha512_create();
	sha512_update(ctx, file, 0, size - HASH_SIZE);
	sha512_makeHash(ctx);
	sha512_release(ctx);

	PrintHash(sha512_hash, "mkHash");

	{
		uchar rHash[HASH_SIZE];

		FILE *fp = fileOpen(file, "rb");
		fileSeek(fp, SEEK_SET, size - HASH_SIZE);
		fileRead(fp, rHash, HASH_SIZE);
		fileClose(fp);

		PrintHash(rHash, "rdHash");

		if(memcmp(sha512_hash, rHash, HASH_SIZE) != 0) // ? wrong hash
		{
			LOGPOS();
			return 0;
		}
	}

	setFileSize(file, size - HASH_SIZE);
	return 1;
}

#define RAWKEY_SIZE 32
#define BLOCK_SIZE 16
#define CPART_SIZE (BLOCK_SIZE * 100000)

static camellia_t *Key;
static uchar *CPart;
static uchar *Iv;

static void XorBlock(uchar block[BLOCK_SIZE], uchar subBlock[BLOCK_SIZE])
{
	((uint *)block)[0] ^= ((uint *)subBlock)[0];
	((uint *)block)[1] ^= ((uint *)subBlock)[1];
	((uint *)block)[2] ^= ((uint *)subBlock)[2];
	((uint *)block)[3] ^= ((uint *)subBlock)[3];
}
static void EncryptFileRCbc(char *file, uchar rawKey[RAWKEY_SIZE])
{
	__int64 size = getFileSize(file);

	errorCase(size < 32);
	errorCase(size % 16 != 0);

	FILE *rfp = fileOpen(file, "rb");
	FILE *wfp = fileOpen(file, "r+b");

	fileSeek(rfp, SEEK_SET, size - BLOCK_SIZE);
	fileRead(rfp, Iv, BLOCK_SIZE);
	fileSeek(rfp, SEEK_SET, 0);

	Key = camellia_create(rawKey, RAWKEY_SIZE);

	for(__int64 remain = size; remain; )
	{
		int readSize = (int)m_min(remain, CPART_SIZE);

		fileRead(rfp, CPart, readSize);

		for(int bPos = 0; bPos < readSize; bPos += BLOCK_SIZE)
		{
			XorBlock(CPart + bPos, Iv);
			camellia_crypt(Key, CPart + bPos, 0);
			memcpy(Iv, CPart + bPos, BLOCK_SIZE);
		}
		fileWrite(wfp, CPart, readSize);

		remain -= readSize;
	}
	camellia_release(Key);

	fileClose(rfp);
	fileClose(wfp);
}
static int DecryptFileRCbc(char *file, uchar rawKey[RAWKEY_SIZE]) // ret: ? successful
{
	__int64 size = getFileSize(file);

	if(
		size < 32 ||
		size % 16 != 0
		)
	{
		LOGPOS();
		return 0;
	}

	FILE *rfp = fileOpen(file, "rb");
	FILE *wfp = fileOpen(file, "r+b");

	Key = camellia_create(rawKey, RAWKEY_SIZE);

	int gotFirstIv = 0;
	uchar firstIv[BLOCK_SIZE];

	for(__int64 remain = size; 0i64 < remain; )
	{
		int readSize = remain < size ? (int)m_min(remain, CPART_SIZE) : BLOCK_SIZE;
		remain -= readSize;

		fileSeek(rfp, SEEK_SET, remain);
		fileRead(rfp, CPart, readSize);

		if(0i64 < remain)
		{
			fileSeek(rfp, SEEK_SET, remain - BLOCK_SIZE);
			fileRead(rfp, Iv, BLOCK_SIZE);
		}
		else
			memcpy(Iv, firstIv, BLOCK_SIZE);

		for(int bPos = readSize; bPos; )
		{
			bPos -= BLOCK_SIZE;
			camellia_crypt(Key, CPart + bPos, 1);
			XorBlock(CPart + bPos, bPos ? CPart + bPos - BLOCK_SIZE : Iv);

			if(!gotFirstIv)
			{
				memcpy(firstIv, CPart + bPos, BLOCK_SIZE);
				gotFirstIv = 1;
			}
		}
		fileSeek(wfp, SEEK_SET, remain);
		fileWrite(wfp, CPart, readSize);
	}
	camellia_release(Key);

	fileClose(rfp);
	fileClose(wfp);

	return 1;
}

int Cipher(char *rFile, int removeRFileFlag, char *wFile, CphrMode_t cphrMode, uchar rawKey[64]) // ret: ? successful
{
	if(CPart == NULL)
	{
		CPart = (uchar *)memAlloc(CPART_SIZE + BLOCK_SIZE);
		Iv = CPart + CPART_SIZE;
	}

	LOGPOS();

	if(removeRFileFlag)
	{
		remove(wFile);
		fileMove(rFile, wFile);
	}
	else
		fileCopy(rFile, wFile);

	LOGPOS();

	if(cphrMode == CPHR_ENC)
	{
		AddPadding(wFile);
		AddRandPart(wFile, RANDPART_SIZE);
		AddHash(wFile);
		AddRandPart(wFile, IV_SIZE);
		EncryptFileRCbc(wFile, rawKey);
		EncryptFileRCbc(wFile, rawKey + 32);

		if(XEnabled)
		{
			LOGPOS();

			FILE *fp = fileOpen(wFile, "ab");
			fileWrite(fp, XExInfo, XExInfoSz);
			fileClose(fp);
		}
	}
	else // ? CPHR_DEC
	{
		if(XEnabled)
			setFileSize(wFile, getFileSize(wFile) - XExInfoSz);

		if(!DecryptFileRCbc(wFile, rawKey + 32))
		{
			LOGPOS();
			return 0;
		}
		if(!DecryptFileRCbc(wFile, rawKey))
		{
			LOGPOS();
			return 0;
		}
		if(!UnaddRandPart(wFile, IV_SIZE))
		{
			LOGPOS();
			return 0;
		}
		if(!UnaddHash(wFile))
		{
			LOGPOS();
			return 0;
		}
		if(!UnaddRandPart(wFile, RANDPART_SIZE))
		{
			LOGPOS();
			return 0;
		}
		if(!UnaddPadding(wFile))
		{
			LOGPOS();
			return 0;
		}
	}
	LOGPOS();
	return 1;
}
