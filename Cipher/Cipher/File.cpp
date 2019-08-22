#include "all.h"

int existPath(char *path)
{
	return !m_isEmpty(path) && !_access(path, 0);
}
int existFile(char *file)
{
	return existPath(file) && !(GetFileAttributes(file) & FILE_ATTRIBUTE_DIRECTORY);
}

void setCwd(char *dir)
{
	errorCase(m_isEmpty(dir));
	errorCase(_chdir(dir)); // ? Ž¸”s
}
void setCwd_x(char *dir)
{
	setCwd(dir);
	memFree(dir);
}
char *getCwd(void)
{
	char *dirBuff = _getcwd(NULL, 0);
	errorCase(m_isEmpty(dirBuff));
	char *dir = strx(dirBuff);
	free(dirBuff);
	return dir;
}

static char *MemorizedDir = NULL;

void addCwd(char *dir)
{
	errorCase(MemorizedDir);
	MemorizedDir = getCwd();
	setCwd(dir);
}
void unaddCwd(void)
{
	errorCase(!MemorizedDir);
	setCwd_x(MemorizedDir);
	MemorizedDir = NULL;
}

void fileMove(char *rFile, char *wFile)
{
	errorCase(rename(rFile, wFile)); // ? Ž¸”s
}
void fileCopy(char *rFile, char *wFile)
{
	remove(wFile); // anti rFile == wFile

	__int64 rCount = getFileSize(rFile);

	FILE *rfp = fileOpen(rFile, "rb");
	FILE *wfp = fileOpen(wFile, "wb");

	const int buffSize = 1000000;
	uchar *buff = (uchar *)memAlloc(buffSize);

	while(0 < rCount)
	{
		int rSize = (int)m_min(rCount, buffSize);

		fileRead(rfp, buff, rSize);
		fileWrite(wfp, buff, rSize);

		rCount -= rSize;
	}
	memFree(buff);

	fileClose(rfp);
	fileClose(wfp);
}

char *getFullPath(char *path, char *baseDir)
{
	errorCase(m_isEmpty(path));
	errorCase(m_isEmpty(baseDir));

	addCwd(baseDir);
	path = _fullpath(NULL, path, 0);
	unaddCwd();

	char *retPath = strx(path);
	free(path);
	return retPath;
}
static char *GetExtPeriod(char *path)
{
	char *p = strrchr(path, '.');

	if(!p || findMbsChar(p + 1, '\\'))
		return NULL;

	return p;
}
char *getExt(char *path)
{
	char *p = GetExtPeriod(path);

	if(p)
		p++;
	else
		p = strchr(path, '\0');

	return p;
}
int removeExt(char *path) // ret: ? removed
{
	char *p = GetExtPeriod(path);

	if(p)
	{
		*p = '\0';
		return 1;
	}
	return 0;
}
