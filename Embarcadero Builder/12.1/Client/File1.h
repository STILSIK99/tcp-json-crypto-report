#pragma once
#include <vector>
#include <cstring>
#include <algorithm>
#include "System.JSON.hpp"
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

struct File {
	String path;
	String size;
	String date;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("path", path);
		req->AddPair("size", size);
		req->AddPair("date", date);
		return req->ToString();
	};

	File(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		path = req->GetValue("path")->Value().c_str();
		size = req->GetValue("size")->Value().c_str();
		date = req->GetValue("date")->Value().c_str();
	};

	File() {
		path = "";
		size = "";
		date = "";
	};

	File(String b, String c, String a) : path(b), size(c), date(a) {
	};

};

struct SSearch {
	String path;
	String reg;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("path", path);
		req->AddPair("reg", reg);
		return req->ToString();
	};

	SSearch(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		path = req->GetValue("path")->Value().c_str();
		reg = req->GetValue("reg")->Value().c_str();
	};

	SSearch() {
		path = "";
		reg = "";
	};

	SSearch(String b, String c) : path(b), reg(c) {
	};

};

struct ans_Files {
	vector<File>arr;
	SSearch zapros;

	String ToJson() {
		auto tree = new TJSONObject;
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto elem = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(elem);
		}
		tree->AddPair("mas", mas);
		tree->AddPair("zap", zapros.ToJson());
		return tree->ToString();
	};

	ans_Files(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(File(mas->Items[i]->ToString()));
		}
		zapros = SSearch(tree->GetValue("zap")->Value());
	};

	ans_Files() {
		arr.clear();
	};

};
