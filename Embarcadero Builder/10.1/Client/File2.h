#include <cstring>
#include <vector>
#include "File1.h"

using std::vector;
using std::map;
using std::string;

typedef unsigned long long ull;

extern "C" ull* __declspec(dllexport) __stdcall get_disk(string name);
extern "C" string* __declspec(dllexport) __stdcall get_disks_names();

otchet* create_otchet() {
	otchet *ans = new otchet;
	string *s = get_disks_names();
	for (int i = 0; i < 26; i++) {
		if (s[i][0] == 0) {
			continue;
		}
		try {
			diskSpace disk;
			disk.name = String(s[i].c_str());
			ull* sizes = get_disk(s[i]);

			long double f = (sizes[0] * sizes[1] * sizes[2])
				/ 1024.0 / 1024 / 1024;
			disk.total = (sizes[0] * sizes[1] * sizes[3])
				/ 1024.0 / 1024 / 1024;
			disk.occup = disk.total - f;
			ans->arr.push_back(disk);
		}
		catch (...) {
		}
	}
	return ans;
};
