#pragma once
#include <vector>
#include <cstring>
#include <algorithm>
#include "System.JSON.hpp"
#include <windows.h>
#include <iostream>
#include <vcl.FileCtrl.hpp>

#include <stdio.h>
#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH
typedef unsigned long long ull;
using std::string;
using std::vector;

extern "C" ull* __declspec(dllexport) __stdcall get_disk(std::string name);

bool dirExists(const std::string& dirName_in) {
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false; // something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true; // this is a directory!

	return false; // this is not a directory!
}

struct diskInfo {
	String name;
	String fr;
	String busy;

	diskInfo() {
	};

	diskInfo(String a, String b, String c) : name(a), fr(b), busy(c) {
	};

	diskInfo(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		name = req->GetValue("name")->Value().c_str();
		fr = req->GetValue("fr")->Value().c_str();
		busy = req->GetValue("busy")->Value().c_str();

	};

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("name", name);
		req->AddPair("fr", fr);
		req->AddPair("busy", busy);
		return req->ToString();
	};

	diskInfo(string n) {
		ull *mas = get_disk(n);
		long double f = (mas[0] * mas[1] * mas[2]) / 1024.0 / 1024 / 1024;
		long double b = (mas[0] * mas[1] * (mas[3] - mas[2]))
			/ 1024.0 / 1024 / 1024;

		fr = String(ull(f)) + "," + String(int(f -int(f)) * 100);
		busy = String(ull(b)) + "," + String(int(b -int(b)) * 100);
		name = n.c_str();
	}

};

vector<string> *get_disk_names() {
	vector<string> *disks = new vector<string>;
	disks->clear();
	HANDLE hdf;
	HMODULE hmod;
	DWORD name;
	int i, n, j;
	char str[26][255], str1[26][255];
	char str0[26][4];
	/////////////////////////////////
	hdf = FindFirstVolume((LPTSTR)str[(n = 0)], 255);
	if (hdf != INVALID_HANDLE_VALUE) {
		do
			1; // cout<<str[n]<<endl;
		while (FindNextVolume(hdf, (LPTSTR)str[++n], 255) != 0);
		FindVolumeClose(hdf);
	}
	else {
		return disks;
	}
	/////////////////////////////////
	name = GetLogicalDrives();
	for (i = j = 0; i < 26; i++) {
		if (((name >> i) & 1)) {
			str0[j][0] = 65 + i;
			str0[j][1] = ':';
			str0[j][2] = '\\';
			str0[j][3] = 0;
			GetVolumeNameForVolumeMountPoint((LPTSTR)str0[j],
				(LPTSTR)str1[j], 255);
			// cout<<str0[j][0]<<" "<<str1[j]<<endl;
			j++;
		}
	}
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (!lstrcmpA((LPCSTR)str[i], (LPCSTR)str1[j])) {
				disks->push_back(str0[j]);
				ShowMessage(str0[j]);
			}
	ShowMessage(disks->size());
	return disks;
}

struct testConnect {
	String str;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("str", str);
		return req->ToString();
	};

	testConnect(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		str = req->GetValue("str")->Value();
	};

	testConnect() {
		str = "Test connection";
	};

};

struct com {
	String type;
	String table_name;
	int key;
	String data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("type", this->type);
		req->AddPair("table", this->table_name);
		req->AddPair("key", this->key);
		req->AddPair("data", this->data);
		return req->ToString();
	};

	com(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		type = req->GetValue("type")->Value().c_str();
		key = StrToInt(req->GetValue("key")->Value());
		table_name = req->GetValue("table")->Value().c_str();
		data = req->GetValue("data")->Value().c_str();
	};

	com() {
		data = "";
		key = 0;
		type = "";
		table_name = "";
	};

	com(String a, String b, int k, String d) : table_name(a), type(b), key(k),
		data(d) {
	};
};

struct simple_ans {
	String result;
	int key;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("result", this->result);
		req->AddPair("key", this->key);
		return req->ToString();
	}

	simple_ans(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		result = req->GetValue("result")->Value();
		key = StrToInt(req->GetValue("key")->Value());
	}

	simple_ans() {
		result = ".";
		key = 0;
	}

	simple_ans(String a, int b) : result(a), key(b) {
	};
};

struct login {
	String log;
	String hash;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("login", this->log);
		req->AddPair("hash", this->hash);
		String str = req->ToString();
		req->Free();
		return str;
	};

	login(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		log = req->GetValue("login")->Value().c_str();
		hash = req->GetValue("hash")->Value().c_str();
	};

	login() {
	};

	login(String a, String b) : log(a), hash(b) {
	};
};

struct listDisk {
	std::vector<diskInfo>arr;

	String time;
	String cname;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("cname", cname);
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto disk = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(disk);
		}
		req->AddPair("mas", mas);
		req->AddPair("time", time);
		return req->ToString();
	};

	listDisk(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		cname = req->GetValue("cname")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(diskInfo(mas->Items[i]->ToString()));
		}
		time = req->GetValue("time")->Value().c_str();
	};

	listDisk() {
		arr.clear();
	};
};

listDisk* get_disks_info() {
	listDisk* ans = new listDisk();
	// vector<string> *disks = get_disk_names();
	string d = "C://";
	for (int i = 0; i < 23; i++) {
		d[0] += i;
		if (!dirExists(d)) {
			continue;
		}
		diskInfo disk(d);
		if (disk.fr == "0") {
			continue;
		}
		ans->arr.push_back(disk);
	}
	return ans;
}
