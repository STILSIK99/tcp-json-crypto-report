#pragma once
#include <vector>
#include <map>
#include <cstring>
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

struct history {
	String filename;
	String change;
	String time;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("filename", filename);
		req->AddPair("change", change);
		req->AddPair("time", time);
		return req->ToString();
	};

	history(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		filename = req->GetValue("filename")->Value().c_str();
		change = req->GetValue("change")->Value().c_str();
		time = req->GetValue("time")->Value().c_str();
	};

	history() {
		filename = "";
		change = "";
		time = "";
	};

	history(String a, String b, String c) : filename(a), change(b), time(c) {
	};
};

struct ans_history {
	vector<history>arr;

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

	ans_history(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(history(mas->Items[i]->ToString()));
		}
	};

	ans_history() {
		arr.clear();
	};

};
