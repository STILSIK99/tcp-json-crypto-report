// Important note about DLL memory management when your DLL uses the
// static version of the RunTime Library:
//
// If your DLL exports any functions that pass String objects (or structs/
// classes containing nested Strings) as parameter or function results,
// you will need to add the library MEMMGR.LIB to both the DLL project and
// any other projects that use the DLL.  You will also need to use MEMMGR.LIB
// if any other projects which use the DLL will be performing new or delete
// operations on any non-TObject-derived classes which are exported from the
// DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
// EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
// the file BORLNDMM.DLL should be deployed along with your DLL.
//
// To avoid using BORLNDMM.DLL, pass string information using "char *" or
// ShortString parameters.
//
// If your DLL uses the dynamic version of the RTL, you do not need to
// explicitly add MEMMGR.LIB as this will be done implicitly for you

#pragma hdrstop
#pragma argsused
#include <windows.h>
#include <iostream>
#include <cstring>
#include <vector>
using namespace std;
typedef unsigned long long ull;

extern "C" int _libmain(unsigned long reason) {
	return 1;
}

extern "C" ull* __declspec(dllexport) __stdcall get_disk(string name) {
	DWORD lpSectorsPerCluster;
	DWORD lpBytesPerSector;
	DWORD lpNumberOfFreeClusters;
	DWORD lpTotalNumberOfClusters;
	ull *mas = new ull[4];
	if (GetDiskFreeSpace(name.c_str(), &lpSectorsPerCluster, &lpBytesPerSector,
		&lpNumberOfFreeClusters, &lpTotalNumberOfClusters)) {
		mas[0] = lpSectorsPerCluster;
		mas[1] = lpBytesPerSector;
		mas[2] = lpNumberOfFreeClusters;
		mas[3] = lpTotalNumberOfClusters;
		return mas;
	}
};

extern "C" string* __declspec(dllexport) __stdcall get_disks_names() {
	string* f = new string[26];
	char vol_name[MAX_PATH + 1];
	HANDLE find_handle;
	find_handle = FindFirstVolume(vol_name, ARRAYSIZE(vol_name));
	int k = 0;
	while (FindNextVolume(find_handle, vol_name, ARRAYSIZE(vol_name))) {
		// cout << vol_name << endl;
		char path[MAX_PATH + 1];
		DWORD lp;
		if (GetVolumePathNamesForVolumeName(vol_name, path,
			ARRAYSIZE(vol_name), &lp)) {
			// cout << path << endl;
			f[k++] = path;
		}
	}
	return f;
}
