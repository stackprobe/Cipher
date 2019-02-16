int existPath(char *path);
int existFile(char *file);

void setCwd(char *dir);
void setCwd_x(char *dir);
char *getCwd(void);
void addCwd(char *dir);
void unaddCwd(void);

void fileMove(char *rFile, char *wFile);
void fileCopy(char *rFile, char *wFile);

char *getFullPath(char *path, char *baseDir = ".");
char *getExt(char *path);
int removeExt(char *path);
