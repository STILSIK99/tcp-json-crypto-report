#include <iostream>
#include <cstring>
#include <windows.h>

using std::cout;
using std::endl;

int main() {
	char vol_name[MAX_PATH + 1];
	HANDLE find_handle;
	find_handle = FindFirstVolume(vol_name, ARRAYSIZE(vol_name));
	// cout << vol_name;
	while (FindNextVolume(find_handle, vol_name, ARRAYSIZE(vol_name))) {
		cout << vol_name << endl;
		char path[MAX_PATH + 1];
		DWORD lp;
		if (GetVolumePathNamesForVolumeName(vol_name, path,
			ARRAYSIZE(vol_name), &lp)) {
			cout << path << endl;
		}
	}
	return 0;
}

// int __cdecl main() {
// HANDLE find_handle;
// char vol_name[MAX_PATH + 1];
// find_handle = FindFirstVolume(vol_name, ARRAYSIZE(vol_name));
// if (find_handle == INVALID_HANDLE_VALUE)
// return 0;
// while (1) {
// if (!FindNextVolume(find_handle, vol_name, ARRAYSIZE(vol_name))) {
// if (GetLastError() == ERROR_NO_MORE_FILES)
// break;
// }
// }
// FindVolumeClose(find_handle);
//
// return 0;
// }
