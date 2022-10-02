#pragma once
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include "System.JSON.hpp"
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
		data = ".";
		key = 0;
		type = ".";
		table_name = ".a";
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

struct Sign {
	String name;
	String data;
	String date;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("name", name);
		req->AddPair("data", data);
		req->AddPair("date", date);
		return req->ToString();
	};

	Sign(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		data = req->GetValue("data")->Value().c_str();
		name = req->GetValue("name")->Value().c_str();
		date = req->GetValue("date")->Value().c_str();
	};

	Sign() {
		name = "";
		data = "";
		date = "";
	};

	Sign(String a, String c, String d) : name(a), data(c), date(d) {
	};
};

struct ans_Sign {
	vector<Sign>arr;

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

	ans_Sign(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(Sign(mas->Items[i]->ToString()));
		}

	};

	ans_Sign() {
		arr.clear();
	};

};

struct Scan {
	String file;
	String sign;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("file", file);
		req->AddPair("sign", sign);
		return req->ToString();
	};

	Scan(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		file = req->GetValue("file")->Value().c_str();
		sign = req->GetValue("sign")->Value().c_str();
	};

	Scan() {
		file = "";
		sign = "";
	};

	Scan(String a, String b) : file(a), sign(b) {
	};
};

struct ans_Scan {
	vector<Scan>arr;

	String ToJson() {
		auto tree = new TJSONObject;
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto elem = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(elem);
		}
		tree->AddPair("mas", mas);
		String js = tree->ToString();
		return js;
	};

	ans_Scan(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(Scan(mas->Items[i]->ToString()));
		}

	};

	ans_Scan() {
		arr.clear();
	};

};
