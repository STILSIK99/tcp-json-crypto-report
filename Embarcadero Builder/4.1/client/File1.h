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
		data = ".";
		key = 0;
		type = ".";
		table_name = ".";
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

struct driv {
	int id;
	String dname;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("dname", dname);
		req->AddPair("id", id);
		return req->ToString();
	}

	driv(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		dname = req->GetValue("dname")->Value();
		id = StrToInt(req->GetValue("id")->Value());
	}

	driv() {
		id = 0;
		dname = ".";
	}

	driv(int b, String a) : dname(a), id(b) {
	};

};

struct listDriv {
	vector<driv>arr;
	String cname;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("cname", cname);
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto dr = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(dr);
		}
		req->AddPair("mas", mas);
		return req->ToString();
	};

	listDriv(String s) {
		if (s == "") {
			return;
		}
		// ShowMessage(s);
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		cname = req->GetValue("cname")->Value();
		auto mas = (TJSONArray*) req->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(driv(mas->Items[i]->ToString()));
		}

	};

	listDriv() {
		arr.clear();
		cname = ".";
	};

};
