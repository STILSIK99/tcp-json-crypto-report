// ---------------------------------------------------------------------------

#pragma hdrstop

#include "CryptoSession.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void CryptoSession::log(const char* text)

{
	if (logFile.is_open()) {
		logFile << TimeToStr(Now()).t_str() << " : " << text;
	}
}

bool CryptoSession::initContext() {
	if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		log("Контекст инициализирован.");
	}
	else {
		log("Ошибка инициализации контекста!");
		return false;
	}

	if (CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash)) {
		log("Пустой хеш-объект создан.");
	}
	else {
		log("Ошибка выполнения функции CryptCreateHash!");
		return false;
	}

	// Хеширование парольной фразы

	if (CryptHashData(hHash, (BYTE*)Password, dwLength, 0)) {
		log("Пароль был хеширован.");
	}
	else {
		log("Ошибка выполнения функции CryptHashData!");
		return false;
	}

	// Создание сессионного ключа на основе хеша парольной фразы

	if (CryptDeriveKey(hCryptProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey)) {
		log("Ключ был выработан. \n");
	}
	else {
		log("cc CryptDeriveKey!");
		return false;
	}
	return true;
}

bool CryptoSession::releaseContext() {
	if (hHash) {
		if (!(CryptDestroyHash(hHash)))
			log("Ошибка выполнения функции CryptDestroyHash");
		else
			hHash = 0;
	}

	// Уничтожение сессионного ключа
	if (hKey) {
		if (!(CryptDestroyKey(hKey)))
			log("Ошибка выполнения функции CryptDestroyKey");
		else
			hKey = 0;
	}

	// Очистка криптографического контекста
	if (hCryptProv) {
		if (!(CryptReleaseContext(hCryptProv, 0)))
			log("Ошибка выполнения функции CryptReleaseContext");
		else
			hCryptProv = 0;
	}

	return true;
}

CryptoSession::CryptoSession(const char* passwd) : TObject() {
	logFile.open("session.log");
	strcpy(Password, passwd);
	dwLength = (DWORD)strlen(Password);
	initContext();
}

__fastcall CryptoSession::~CryptoSession() {
	releaseContext();
	// Очистка шифрованных данных
	if (m_crypted) {
		delete[]m_crypted;
		m_crypted = 0;
	}
	m_size = 0;

}

bool CryptoSession::CryptData(const char* in, char*& out, unsigned long& size) {
	out = new char[size];
	memcpy(out, in, size);
	if (!CryptEncrypt(hKey, 0, true, 0, (BYTE*)out, &size, size)) {
		log("Ошибка выполнения функции CryptEncrypt.");
		return false;
	}
	log("Шифрование завершено.\n");
	return true;

}

bool CryptoSession::DecryptData(const char* in, char*& out, unsigned long& size) {
	out = new char[size];
	memcpy(out, in, size);
	if (!CryptDecrypt(hKey, 0, true, 0, (BYTE*)out, &size)) {
		log("Ошибка выполнения функции CryptDecrypt.");
		return false;
	}
	log("Дешифрование завершено.\n");
	return true;
}
