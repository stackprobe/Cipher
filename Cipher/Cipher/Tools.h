int getZero(void);
int s2i(char *line);

int compInt(int a, int b);
int compI2D(i2D_t a, i2D_t b);

int isHitKey(int key);

void addValue(autoList<uchar> *block, uint64 value);
uint64 unaddValue64(autoList<uchar> *block);

char *MkPassword(int pwLen);
char *MkPassKey(void);
