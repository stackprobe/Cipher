#include "all.h"

void createKeyContainer(void)
{
	HCRYPTPROV hp;

	if(CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET)) // エラー無視
		CryptReleaseContext(hp, 0);
}
void deleteKeyContainer(void)
{
	HCRYPTPROV hp;

	CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_DELETEKEYSET); // エラー無視
}
void getCryptoBlock_MS(uchar *buffer, int size)
{
	HCRYPTPROV hp;

	if(!CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		cout("Failed to open CSP. Last error: %08x\n", GetLastError());
		error();
	}
	if(!CryptGenRandom(hp, size, buffer))
	{
		CryptReleaseContext(hp, 0);
		error();
	}
	CryptReleaseContext(hp, 0);
#if 0
// test
cout("mscr: ");
for(int c = 0; c < size; c++)
	cout("%02x", buffer[c]);
cout("\n");
#endif
}

#if 0

#define BUFFERSIZE 1000000
//#define BUFFERSIZE 65 // old

int getCryptoByte(void)
{
	static uchar buffer[BUFFERSIZE];
	static uint index = BUFFERSIZE;

	if(index == BUFFERSIZE)
	{
		getCryptoBlock_MS(buffer, BUFFERSIZE);
		index = 0;
	}
	return buffer[index++];
}

#else

#define SEEDSIZE 65536 // 超長くしてみる。
//#define SEEDSIZE 4096
#define BUFFERSIZE 64 // == sha512 hash size

static void GetCryptoBlock(uchar *buffer)
{
	static sha512_t *ctx;
//	static uchar counter[16]; // こっちも長くしてみる。<- 意味ないだろ。一周しないだろうし、乱数の質が上がる訳でもないだろうし、
	static uchar counter[8];

	sha512_localize();

	if(!ctx)
	{
		uchar seed[SEEDSIZE];

		getCryptoBlock_MS(seed, SEEDSIZE);
		getCryptoBlock_MS(counter, sizeof(counter));
//memset(counter, 0xff, sizeof(counter)); // test
//counter[0] = 0xf0; // test

		ctx = sha512_create();
		sha512_update(ctx, seed, SEEDSIZE);
		sha512_makeHash(ctx);
	}
	else
	{
		sha512_t *ictx = sha512_copy(ctx);

		sha512_update(ictx, &counter, sizeof(counter));
		sha512_makeHash(ictx);
		sha512_release(ictx);

		// increment counter
		for(uchar *p = counter; p < counter + lengthof(counter); p++)
		{
			if(*p < 0xff)
			{
				p[0]++;
				break;
			}
			*p = 0x00;
		}
#if 0 // test
		cout("counter: ");
		for(int index = 0; index < lengthof(counter); index++)
		{
			cout("%02x", counter[index]);
		}
		cout("\n");
#endif
	}
	memcpy(buffer, sha512_hash, BUFFERSIZE);
	sha512_unlocalize();
}
int getCryptoByte(void)
{
	static uchar buffer[BUFFERSIZE];
	static uint index = BUFFERSIZE;

	if(BUFFERSIZE <= index)
	{
		GetCryptoBlock(buffer);
		index = 0;
	}
	return buffer[index++];
}

#endif

uint getCryptoRand(uint modulo) // ret: 0 ～ (modulo - 1), modulo: 1 ～ UINTMAX
{
	uint64 value;
	uint64 r = (0x8000000000000000ui64 / modulo) * modulo;

	do
	{
		for(int index = 0; index < sizeof(uint64); index++)
		{
			((uchar *)&value)[index] = getCryptoByte();
		}
		value &= 0x7fffffffffffffffui64;
	}
	while(r <= value);

	return (uint)(value % (uint64)modulo);
}
