#include "all.h"

char *MkPassword(int pwLen)
{
	errorCase_m(!m_isRange(pwLen, 3, 1000), "要求されたパスフレーズの長さに問題があります。");

	char *pw = (char *)memAlloc(pwLen + 1);

	for(; ; )
	{
		int dig = 0;
		int upr = 0;
		int lwr = 0;

		for(int index = 0; index < pwLen; index++)
		{
			int value = (int)getCryptoRand64_Mod(10 + 26 + 26);
			int chr;

			if(value < 10)
			{
				chr = '0' + value;
				dig = 1;
				goto chrSt;
			}
			value -= 10;
			if(value < 26)
			{
				chr = 'A' + value;
				upr = 1;
				goto chrSt;
			}
			value -= 26;
			{
				chr = 'a' + value;
				lwr = 1;
			}
chrSt:
			pw[index] = chr;
		}
		if(dig && upr && lwr)
			break;
	}
	pw[pwLen] = '\0';
	return pw;
}
char *MkPassKey(void)
{
	char *pk = (char *)memAlloc(128 + 1);

	for(int index = 0; index < 128; index += 2)
	{
		sprintf(pk + index, "%02x", getCryptoByte());
	}
	return pk;
}
