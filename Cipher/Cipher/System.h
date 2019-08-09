extern int BatchMode;

void termination(int errorlevel);
void error2(char *source, int lineno, char *function, char *message = NULL);

#define error() \
	error2(__FILE__, __LINE__, __FUNCTION__)

#define error_m(message) \
	error2(__FILE__, __LINE__, __FUNCTION__, (message))

#define errorCase(status) \
	do { \
	if((status)) error(); \
	} while(0)

#define errorCase_m(status, message) \
	do { \
	if((status)) error_m((message)); \
	} while(0)

#define LOGPOS() \
	cout("%s (%d) %s\n", __FILE__, __LINE__, __FUNCTION__)

#define cout cout2

// sync > @ cout_h
void cout(char *format, ...);
char *xcout(char *format, ...);
char *vxcout(char *format, va_list marker);
void coutLongText(char *text);
void coutLongText_x(char *text);
// < sync

#undef cout

extern void (*cout)(char *format, ...);

void SetSilentMode(int flag);

int hasArgs(int count);
int argIs(char *spell);
char *getArg(int index);
char *nextArg(void);
int getArgIndex(void);
void setArgIndex(int index);
