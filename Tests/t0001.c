#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRRandom.h"

#define CIPHER_EXE "..\\Cipher\\Release\\Cipher.exe"

#define TEST_PLAIN_FILE "C:\\temp\\1.tmp"
#define TEST_CIPHER_FILE "C:\\temp\\2.tmp"
#define TEST_PLAIN_2_FILE "C:\\temp\\3.tmp"

static void Test0001_a(uint lmtSize)
{
	autoBlock_t *plain = makeCryptoRandBlock(getCryptoRand() % lmtSize);

	removeFileIfExist(TEST_PLAIN_FILE);
	removeFileIfExist(TEST_CIPHER_FILE);
	removeFileIfExist(TEST_PLAIN_2_FILE);

	writeBinary(TEST_PLAIN_FILE, plain);

	coExecute(CIPHER_EXE " /E /P 123 " TEST_PLAIN_FILE " " TEST_CIPHER_FILE);

	errorCase(!existFile(TEST_CIPHER_FILE));

	coExecute(CIPHER_EXE " /D /P 123 " TEST_CIPHER_FILE " " TEST_PLAIN_2_FILE);

	errorCase(!existFile(TEST_PLAIN_2_FILE));
	errorCase(!isSameFile(TEST_PLAIN_FILE, TEST_PLAIN_2_FILE));

	releaseAutoBlock(plain);
}
static void Test0001(uint lmtSize, uint testCount)
{
	uint c;

	for(c = 0; c < testCount; c++)
	{
		Test0001_a(lmtSize);
	}
}
int main(int argc, char **argv)
{
	mt19937_initCRnd();

	Test0001(10, 100);
	Test0001(100, 100);
	Test0001(1000, 100);
}
