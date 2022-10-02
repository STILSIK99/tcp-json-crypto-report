#pragma once
#include <vector>
#include <cstring>
#include <algorithm>
#include "System.JSON.hpp"
#include <windows.h>
#include <vcl.FileCtrl.hpp>

#include <stdio.h>
#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH
typedef unsigned long long ull;
using std::string;
using std::vector;
using std::swap;

string str(unsigned int a) {
	string res = "";
	while (a / 10 > 0) {
		res += char(a % 10 + 48);
		a /= 10;
	}
	res += char(a % 10 + 48);
	int k = res.length() / 2;
	int last = res.length() - 1;
	for (int i = 0; i < k; i++) {
		swap(res[i], res[last - i]);
	}
	return res;
};

string* GetInfo() {
	string *mas = new string[26 * 5];
	char vol_name[MAX_PATH + 1];
	HANDLE find_handle;
	find_handle = FindFirstVolume(vol_name, ARRAYSIZE(vol_name));
	int k = 0;
	while (FindNextVolume(find_handle, vol_name, ARRAYSIZE(vol_name))) {
		char path[MAX_PATH + 1];
		DWORD lp;
		if (GetVolumePathNamesForVolumeName(vol_name, path,
			ARRAYSIZE(vol_name), &lp)) {
			char NameBuffer[MAX_PATH];
			char SysNameBuffer[MAX_PATH];
			char num[10];
			DWORD VSNumber;
			DWORD MCLength;
			DWORD FileSF;
			mas[5 * k] = path;

			if (GetVolumeInformation(path, NameBuffer, sizeof(NameBuffer),
				&VSNumber, &MCLength, &FileSF, SysNameBuffer,
				sizeof(SysNameBuffer))) {
				(mas[5 * k + 2] = NameBuffer, mas[5 * k + 3] = SysNameBuffer);
				itoa(VSNumber, num, 10);
				string numb = num;
				if (num[0] == '-') {
					numb = numb.substr(1, numb.length() - 2);
				}
				mas[5 * k + 4] = numb;
			}

			switch (GetDriveType(path)) {
			case DRIVE_NO_ROOT_DIR:
				mas[5 * k + 1] = "не найден";
				break;
			case DRIVE_REMOVABLE:
				mas[5 * k + 1] = "извлекаемый диск";
				break;
			case DRIVE_FIXED:
				mas[5 * k + 1] = "жесткий диск";
				break;
			case DRIVE_REMOTE:
				mas[5 * k + 1] = "сетевой диск";
				break;
			case DRIVE_CDROM:
				mas[5 * k + 1] = "СD ROM";
				break;
			case DRIVE_RAMDISK:
				mas[5 * k + 1] = "в оперативной памяти";
				break;
			default:
				mas[5 * k + 1] = "неизвестный тип";
				break;
			}

			k++;
		}
	}
	FindVolumeClose(find_handle);
	for (int f = 5 * k; f < 26 * 5; ++f) {
		mas[f] = "";
	}
	return mas;
}
