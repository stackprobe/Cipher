#include "all.h"

int BatchMode;
int SilentMode;

void termination(int errorlevel)
{
	cout("exitcode: %d\n", errorlevel);
	exit(errorlevel);
}
void error2(char *source, int lineno, char *function, char *message)
{
	{
		static int once;

		if(once)
			exit(2);

		once = 1;
	}

	cout("ERROR %s %d %s\n", source, lineno, function);

	if(message)
		cout("message: %s\n", message);

	if(!BatchMode)
		MessageBox(
			NULL,
			message ? message : xcout("An error occurred @ %s %d %s", source, lineno, function),
			"Cipher Error",
			MB_OK | MB_ICONSTOP | MB_TOPMOST
			);

	termination(EXITCODE_ERROR);
}

// sync > @ cout

void cout(char *format, ...)
{
	va_list marker;

	va_start(marker, format);

	if(!strcmp(format, "%s"))
	{
		coutLongText(va_arg(marker, char *));
	}
	else if(!strcmp(format, "%s\n"))
	{
		coutLongText(va_arg(marker, char *));
		coutLongText("\n");
	}
	else
		coutLongText_x(vxcout(format, marker));

	va_end(marker);
}
char *xcout(char *format, ...)
{
	char *ret;
	va_list marker;

	va_start(marker, format);
	ret = vxcout(format, marker);
	va_end(marker);

	return ret;
}
char *vxcout(char *format, va_list marker)
{
	char *buffer;

	for(int size = strlen(format) + 100; ; size *= 2)
	{
		int ret;

#define MARGIN 10

		buffer = (char *)memAlloc(size + MARGIN * 2);
		ret = _vsnprintf(buffer, size + MARGIN, format, marker);

#undef MARGIN

		if(0 <= ret && ret <= size)
			break;

		memFree(buffer);
		errorCase(128 * 1024 * 1024 < size); // ANTI OVER-FLOW
	}
	return strr(buffer);
}
void coutLongText(char *text)
{
	coutLongText_x(strx(text));
}
void coutLongText_x(char *text)
{
	char *p;
	char *q;

	for(p = text; *p; p = q)
	{
		int bkc;

#define FPUTS_TEXT_LMT 100

		for(q = p; *q && ((uint)q - (uint)p) < FPUTS_TEXT_LMT; q += _ismbblead(*q) ? 2 : 1)
		{}

#undef FPUTS_TEXT_LMT

		bkc = *q;
		*q = '\0';
		errorCase(fputs(p, stdout) < 0);
		*q = bkc;
	}
	memFree(text);
}

// < sync

static int ArgIndex = 1;

int hasArgs(int count)
{
	return count <= __argc - ArgIndex;
}
int argIs(char *spell)
{
	if(ArgIndex < __argc)
	{
		if(!_stricmp(__argv[ArgIndex], spell))
		{
			ArgIndex++;
			return 1;
		}
	}
	return 0;
}
char *getArg(int index)
{
	errorCase(index < 0 || __argc - ArgIndex <= index);
	return __argv[ArgIndex + index];
}
char *nextArg(void)
{
	char *arg = getArg(0);

	ArgIndex++;
	return arg;
}
int getArgIndex(void)
{
	return ArgIndex;
}
void setArgIndex(int index)
{
	errorCase(index < 0 || __argc < index); // index == __argc は全部読み終わった状態
	ArgIndex = index;
}
