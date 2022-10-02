#pragma once
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include "System.JSON.hpp"
#include <windows.h>
#include <iostream>

#include <stdio.h>
#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH
typedef unsigned long long ull;

using std::vector;
using std::map;
using std::string;

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

struct agent {
	vector<String>disks;
	map<String, double>porog;
};

struct diskSpace {
	String name;
	double total;
	double occup;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("name", name);
		req->AddPair("total", total);
		req->AddPair("occup", occup);
		String str = req->ToString();
		req->Free();
		return str;
	};

	diskSpace(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		name = req->GetValue("name")->Value().c_str();
		total = req->GetValue("total")->Value().ToDouble();
		occup = req->GetValue("occup")->Value().ToDouble();
	};

	diskSpace() {
	};

};

struct otchet {
	vector<diskSpace>arr;

	String ToJson() {
		auto req = new TJSONObject;
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto disk = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(disk);
		}
		req->AddPair("mas", mas);
		return req->ToString();
	};

	otchet(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) req->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(diskSpace(mas->Items[i]->ToString()));
		}
	};

	otchet() {
		arr.clear();
	};

};
