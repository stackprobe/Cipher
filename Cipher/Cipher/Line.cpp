#include "all.h"

char *strx(char *line)
{
	return strcpy((char *)memAlloc(strlen(line) + 1), line);
}
char *strr(char *line)
{
	return (char *)memRealloc(line, strlen(line) + 1);
}

char *findMbsChar(char *line, int chr)
{
	for(char *p = line; *p; mbcInc(p))
		if(*p == chr)
			return p;

	return NULL; // not found
}

int toUpper(int chr)
{
	if('a' <= chr && chr <= 'z')
	{
		chr += 'A' - 'a';
	}
	return chr;
}
int toLower(int chr)
{
	if('A' <= chr && chr <= 'Z')
	{
		chr += 'a' - 'A';
	}
	return chr;
}
int mbs_stricmp(char *str1, char *str2)
{
	int onMbc = 0;

	for(int index = 0; ; index++)
	{
		int chr1 = str1[index];
		int chr2 = str2[index];

		if(!chr1 && !chr2)
			return 0;

		if(onMbc)
		{
			onMbc = 0;
		}
		else if(isMbc1(chr1))
		{
			onMbc = 1;
		}
		else
		{
			chr1 = toLower(chr1);
			chr2 = toLower(chr2);
		}

		if(chr1 != chr2)
		{
			return chr1 - chr2;
		}
	}
	error();
	return 0;
}

char *addToken(char *buffer, char *token)
{
	char *newBuffer = xcout("%s%s", buffer, token);
	memFree(buffer);
	return newBuffer;
}
char *addToken_x(char *buffer, char *token)
{
	char *newBuffer = addToken(buffer, token);
	memFree(token);
	return newBuffer;
}

int ToLowerChar(int chr)
{
	if('A' <= chr && chr <= 'Z')
		chr += 'a' - 'A';

	return chr;
}
int IndexOfChar(char *str, int chr)
{
	int index;

	for(index = 0; str[index]; index++)
		if(str[index] == chr)
			break;

	return index;
}
int PrmToChr(char *prm)
{
	if(
		strchr(hexadecimal, ToLowerChar(prm[0])) &&
		strchr(hexadecimal, ToLowerChar(prm[1])) &&
		!prm[2]
		)
		return
			IndexOfChar(hexadecimal, ToLowerChar(prm[0])) * 16 +
			IndexOfChar(hexadecimal, ToLowerChar(prm[1]));

	return prm[0];
}
