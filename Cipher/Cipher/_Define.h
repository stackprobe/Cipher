#define ENCFILE_EXT "cphr"
#define DECFILE_EXT "dec"

typedef enum CphrMode_et
{
	CPHR_NONE, // as 0
	CPHR_ENC,
	CPHR_DEC,
}
CphrMode_t;

enum
{
	EXITCODE_NORMAL,
	EXITCODE_ERROR,
	EXITCODE_USER_CANCEL,
	EXITCODE_DECRYPTION_ERROR,
};
