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
	int id;
	String source;
	String name;
	String date;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("source", source);
		req->AddPair("name", name);
		req->AddPair("id", id);
		req->AddPair("date", date);
		return req->ToString();
	};

	File(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		source = req->GetValue("source")->Value().c_str();
		name = req->GetValue("name")->Value().c_str();
		id = StrToInt(req->GetValue("id")->Value());
		date = req->GetValue("date")->Value().c_str();

	};

	File() {
		source = "";
		name = "";
		date = "";
		id = 0;
	};

	File(int a, String b, String c, String d) : id(a), source(b), name(c),
		date(d) {
	};

};

struct ans_Files {
	vector<File>arr;

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

	ans_Files(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(File(mas->Items[i]->ToString()));
		}
	};

	ans_Files() {
		arr.clear();
	};

};

struct User {
	int id;
	String name;
	String pash;
	String access;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("access", access);
		req->AddPair("pash", pash);
		req->AddPair("name", name);
		req->AddPair("id", id);
		return req->ToString();
	};

	User(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id = StrToInt(req->GetValue("id")->Value());
		pash = req->GetValue("pash")->Value().c_str();
		name = req->GetValue("name")->Value().c_str();
		access = req->GetValue("access")->Value().c_str();
	};

	User() {
		access = "";
		name = "";
		pash = "";
		id = 0;
	};

	User(int a, String b, String c, String d) : id(a), name(b), pash(c),
		access(d) {
	};

};

struct ans_Users {
	vector<User>arr;

	String ToJson() {
		auto req = new TJSONObject;
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto elem = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(elem);
		}
		req->AddPair("mas", mas);
		return req->ToString();
	};

	ans_Users(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) req->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(User(mas->Items[i]->ToString()));
		}
	};

	ans_Users() {
		arr.clear();
	};

};

struct History {
	int id;
	String userName;
	String fileName;
	String datetime;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("datetime", datetime);
		req->AddPair("fileName", fileName);
		req->AddPair("userName", userName);
		req->AddPair("id", id);
		return req->ToString();
	};

	History(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id = StrToInt(req->GetValue("id")->Value());
		fileName = req->GetValue("fileName")->Value().c_str();
		userName = req->GetValue("userName")->Value().c_str();
		datetime = req->GetValue("datetime")->Value().c_str();

	};

	History() {
		fileName = "";
		userName = "";
		datetime = "";
		id = 0;
	};

	History(int a, String b, String c, String d)
		: id(a), userName(b), fileName(c), datetime(d) {
	};

};

struct ans_History {
	vector<History>arr;

	String ToJson() {
		auto req = new TJSONObject;
		auto mas = new TJSONArray;
		for (auto x : arr) {
			auto elem = (TJSONObject*)TJSONObject::ParseJSONValue(x.ToJson());
			mas->Add(elem);
		}
		req->AddPair("mas", mas);
		return req->ToString();
	};

	ans_History(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) req->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(History(mas->Items[i]->ToString()));
		}
	};

	ans_History() {
		arr.clear();
	};

};

struct sorting {
	int date;
	String date1, date2;
	int data;
	String ff;
	int data1;
	String ss;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("date", date);
		req->AddPair("date1", date1);
		req->AddPair("date2", date2);
		req->AddPair("data", data);
		req->AddPair("data1", data1);
		req->AddPair("ff", ff);
		req->AddPair("ss", ss);
		return req->ToString();
	};

	sorting(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		date = StrToInt(req->GetValue("date")->Value());
		date1 = req->GetValue("date1")->Value().c_str();
		date2 = req->GetValue("date2")->Value().c_str();
		data = StrToInt(req->GetValue("data")->Value());
		data1 = StrToInt(req->GetValue("data1")->Value());
		ff = req->GetValue("ff")->Value().c_str();
		ss = req->GetValue("ss")->Value().c_str();
	};

	sorting() {
		data = 0;
		date = 0;
		data1 = 0;
	};

	sorting(int a, String b, String c, int e, String d, int g, String f)
		: date(a), date1(b), date2(c), data(e), ff(d), data1(g), ss(f) {
	};

};
