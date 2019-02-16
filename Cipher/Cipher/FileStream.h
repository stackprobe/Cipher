FILE *rfopen(char *file, char *mode);
FILE *fileOpen(char *file, char *mode);
void fileClose(FILE *fp);

__int64 getFileSize(FILE *fp);
__int64 getFileSize(char *file);
void setFileSize(char *file, __int64 size);

int readChar(FILE *fp);
void writeChar(FILE *fp, int chr);

void fileSeek(FILE *fp, int origin, __int64 offset);
void fileRead(FILE *fp, void *block, int size);
void fileWrite(FILE *fp, void *block, int size);

//void readBlock(char *file, void *block, int size); // not using
void writeBlock(char *file, void *block, int size, int addMode = 0);
