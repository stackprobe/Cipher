#include <wincrypt.h>
#pragma comment(lib, "ADVAPI32")

void createKeyContainer(void);
void deleteKeyContainer(void);
void getCryptoBlock_MS(uchar *buffer, int size);

int getCryptoByte(void);
uint getCryptoRand(uint modulo);
