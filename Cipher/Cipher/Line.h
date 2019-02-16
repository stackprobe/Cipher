char *strx(char *line);

#define isMbc1(chr) \
	(_ismbblead((chr)))

#define isMbc2(chr) \
	(_ismbbtrail((chr)))

#define mbcNext(p) \
	((p) + (isMbc1(*(p)) && (p)[1] ? 2 : 1))

#define mbcInc(p) \
	((p) = mbcNext((p)))

char *findMbsChar(char *line, int chr);

int toUpper(int chr);
int toLower(int chr);
int mbs_stricmp(char *str1, char *str2);

char *addToken(char *buffer, char *token);
char *addToken_x(char *buffer, char *token);

#define hexadecimal \
	"0123456789abcdef"

int ToLowerChar(int chr);
int IndexOfChar(char *str, int chr);
int PrmToChr(char *prm);
