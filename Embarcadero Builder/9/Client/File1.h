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

struct Drive {
	String DriveName;
	String DriveType;
	String NameBuffer;
	String FileSystem;
	String VSNumber;

	Drive(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		DriveName = req->GetValue("DriveName")->Value().c_str();
		DriveType = req->GetValue("DriveType")->Value().c_str();
		NameBuffer = req->GetValue("NameBuffer")->Value().c_str();
		FileSystem = req->GetValue("FileSystem")->Value().c_str();
		VSNumber = req->GetValue("VSNumber")->Value().c_str();

	};

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("DriveName", DriveName);
		req->AddPair("DriveType", DriveType);
		req->AddPair("NameBuffer", NameBuffer);
		req->AddPair("FileSystem", FileSystem);
		req->AddPair("VSNumber", VSNumber);
		return req->ToString();
	};

	bool check() {
		if (DriveName != "")
			return 1;
		if (DriveType != "")
			return 1;
		if (NameBuffer != "")
			return 1;
		if (FileSystem != "")
			return 1;
		if (VSNumber != "")
			return 1;
		return 0;
	}

	Drive() {
	};
};

struct MasDrive {
	vector<Drive>arr;

	String ToJson() {
		auto tree = new TJSONObject;
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto elem = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(elem);
		}
		tree->AddPair("mas", mas);
		return tree->ToString();
	};

	MasDrive(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(Drive(mas->Items[i]->ToString()));
		}
	};

	MasDrive() {
		arr.clear();
	};

};

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
