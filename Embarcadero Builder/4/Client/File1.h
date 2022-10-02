#pragma once
#include <vector>
#include <cstring>
#include <algorithm>
#include "System.JSON.hpp"
#include <regex>
#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#include <stdio.h>

#define ARRAY_SIZE 1024
using namespace std;

String reg_s(String reg, String str) {
	string subject = AnsiString(str).c_str();
	string result;
	try {
		regex re(AnsiString(reg).c_str());
		smatch match;
		if (regex_search(subject, match, re)) {
			return str;
		}
	}
	catch (regex_error& e) {
	}
	return "";
};

vector<String>get_drivers() {
	vector<String>arr;
	LPVOID drivers[ARRAY_SIZE];
	DWORD cbNeeded;
	int cDrivers, i;

	if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded <
		sizeof(drivers)) {
		TCHAR szDriver[ARRAY_SIZE];

		cDrivers = cbNeeded / sizeof(drivers[0]);

		_tprintf(TEXT("There are %d drivers:\n"), cDrivers);
		for (i = 0; i < cDrivers; i++) {
			if (GetDeviceDriverBaseName(drivers[i], szDriver,
				sizeof(szDriver) / sizeof(szDriver[0]))) {
				// _tprintf(TEXT("%d: %s\n"), i + 1, szDriver);
				arr.push_back(szDriver);
			}
		}
	}
	else {
	}

	return arr;
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
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		log = req->GetValue("login")->Value().c_str();
		hash = req->GetValue("hash")->Value().c_str();
	};

	login() {
	};
};

struct Drivers {
	vector<String>arr;

	String ToJson() {
		auto tree = new TJSONObject;
		auto mas = new TJSONArray;
		for (auto x : arr) {
			mas->Add(x);
		}
		tree->AddPair("mas", mas);
		return tree->ToString();
	};

	Drivers(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(mas->Items[i]->Value().c_str());
		}
	};

	Drivers() {
		arr.clear();
	};

};


struct twoStr {
	String ff;
	String ss;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("ff", ff);
		req->AddPair("ss", ss);
		return req->ToString();
	};

	twoStr(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		ff = req->GetValue("ff")->Value().c_str();
		ss = req->GetValue("ss")->Value().c_str();
	};

	twoStr() {
		ff = "0";
        ss = "0";
	};
};
