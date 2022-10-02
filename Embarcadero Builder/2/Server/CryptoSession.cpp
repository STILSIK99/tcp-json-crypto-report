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
		log("�������� ���������������.");
	}
	else {
		log("������ ������������� ���������!");
		return false;
	}

	if (CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash)) {
		log("������ ���-������ ������.");
	}
	else {
		log("������ ���������� ������� CryptCreateHash!");
		return false;
	}

	// ����������� ��������� �����

	if (CryptHashData(hHash, (BYTE*)Password, dwLength, 0)) {
		log("������ ��� ���������.");
	}
	else {
		log("������ ���������� ������� CryptHashData!");
		return false;
	}

	// �������� ����������� ����� �� ������ ���� ��������� �����

	if (CryptDeriveKey(hCryptProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey)) {
		log("���� ��� ���������. \n");
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
			log("������ ���������� ������� CryptDestroyHash");
		else
			hHash = 0;
	}

	// ����������� ����������� �����
	if (hKey) {
		if (!(CryptDestroyKey(hKey)))
			log("������ ���������� ������� CryptDestroyKey");
		else
			hKey = 0;
	}

	// ������� ������������������ ���������
	if (hCryptProv) {
		if (!(CryptReleaseContext(hCryptProv, 0)))
			log("������ ���������� ������� CryptReleaseContext");
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
	// ������� ����������� ������
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
		log("������ ���������� ������� CryptEncrypt.");
		return false;
	}
	log("���������� ���������.\n");
	return true;

}

bool CryptoSession::DecryptData(const char* in, char*& out, unsigned long& size) {
	out = new char[size];
	memcpy(out, in, size);
	if (!CryptDecrypt(hKey, 0, true, 0, (BYTE*)out, &size)) {
		log("������ ���������� ������� CryptDecrypt.");
		return false;
	}
	log("������������ ���������.\n");
	return true;
}
