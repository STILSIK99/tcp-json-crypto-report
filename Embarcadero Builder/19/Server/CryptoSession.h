//---------------------------------------------------------------------------

#ifndef CryptoSessionH
#define CryptoSessionH

#include <windows.h>
#include <Wincrypt.h>
#include <fstream>
#include <vcl.h>

//---------------------------------------------------------------------------

class CryptoSession: public TObject {
	HCRYPTPROV hCryptProv;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;
	char Password[512];
	DWORD dwLength;
	char* m_crypted;
	unsigned long m_size;
	ofstream logFile;
	void log(const char* text);
	bool initContext();
	bool releaseContext();
  public:
	CryptoSession(const char* passwd = "pass");
	virtual __fastcall	~CryptoSession();

	bool CryptData(const char* in, char*& out, unsigned long& size);
	bool DecryptData(const char* in, char*& out, unsigned long& size);

};
#endif
