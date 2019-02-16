#define _CRT_SECURE_NO_WARNINGS

// íËî‘ {
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <mbstring.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h> // sys/stat.h ÇÊÇËêÊÇ≈Ç†ÇÈÇ±Ç∆ÅB
#include <sys\stat.h>
#include <time.h>
// }

#include <windows.h>

typedef unsigned char uchar;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint;
typedef unsigned __int64 uint64;

#define m_isEmpty(str) \
	(!(str) || !*(str))

#define m_min(value1, value2) \
	((value1) < (value2) ? (value1) : (value2))

// not using
/*
#define m_max(value1, value2) \
	((value1) < (value2) ? (value2) : (value1))
	*/

#define m_isRange(value, minval, maxval) \
	((minval) <= (value) && (value) <= (maxval))

#define m_isHex(chr) \
	(m_isRange((chr), '0', '9') || m_isRange((chr), 'A', 'F') || m_isRange((chr), 'a', 'f'))

#define m_c2i(chr) \
	((chr) & 0x40 ? ((chr) & ~0x20) - 'A' + 10 : (chr) - '0')

#define m_d2i(value) \
	((int)((value) + 0.5))
