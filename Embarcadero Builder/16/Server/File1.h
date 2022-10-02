#pragma once
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include "System.JSON.hpp"
using namespace std;

String StrTime(SYSTEMTIME t) {
	String ans;
	if (String(t.wDay).Length() < 2) {
		ans += "0";
	}
	ans += String(t.wDay);
	ans += ".";
	if (String(t.wMonth).Length() < 2) {
		ans += "0";
	}
	ans += String(t.wMonth);
	ans += ".";
	ans += String(t.wYear);
	ans += " ";
	if (String(t.wHour).Length() < 2) {
		ans += "0";
	}
	ans += String(t.wHour);
	ans += ":";
	if (String(t.wMinute).Length() < 2) {
		ans += "0";
	}
	ans += String(t.wMinute);
	ans += ":";
	if (String(t.wSecond).Length() < 2) {
		ans += "0";
	}
	ans += String(t.wSecond);
	ans += ".";
	if (String(t.wMilliseconds).Length() == 2) {
		ans += "0";
	}
	if (String(t.wMilliseconds).Length() == 1) {
		ans += "00";
	}
	ans += String(t.wMilliseconds);
	return ans;
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

struct FFile {
	String name;
	SYSTEMTIME t_A;
	SYSTEMTIME t_C;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("name", name);
		req->AddPair("t_A", StrTime(t_A));
		req->AddPair("t_C", StrTime(t_C));
		return req->ToString();
	};

	FFile(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		name = req->GetValue("name")->Value().c_str();
		t_A = SysTime(req->GetValue("t_A")->Value().c_str());
		t_C = SysTime(req->GetValue("t_C")->Value().c_str());
	};

	FFile() {
		name = "";
	};

	SYSTEMTIME SysTime(String a) {
		SYSTEMTIME t;
		if (a == "") {
			return t;
		}
		t.wDay = a.SubString(1, 2).ToInt();
		t.wMonth = a.SubString(4, 2).ToInt();
		t.wYear = a.SubString(7, 4).ToInt();
		t.wHour = a.SubString(12, 2).ToInt();
		t.wMinute = a.SubString(15, 2).ToInt();
		t.wSecond = a.SubString(18, 2).ToInt();
		t.wMilliseconds = a.SubString(21, 3).ToInt();
		return t;
	};

};

struct monitor {
	int id;
	String timeStart;
	map<String, FFile>files;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id", id);
		auto mas = new TJSONArray;
		int i = 0;
		for (auto x = files.begin(); x != files.end(); x++) {
			auto elem = new TJSONObject;
			elem->AddPair("str", x->second.ToJson());
			elem->AddPair("name", x->first);
			mas->Add(elem);
		}
		req->AddPair("files", mas);
		return req->ToString();
	};

	monitor(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id = StrToInt(req->GetValue("id")->Value().c_str());
		auto mas = (TJSONArray*)req->GetValue("files");
		for (int i = 0; i < mas->Count; i++) {
			TJSONObject *elem = static_cast<TJSONObject*>(mas->Get(i));
			String name = reinterpret_cast<TJSONPair*>(elem->Get("name"))
				->JsonValue->Value();
			files[name] =
				FFile(reinterpret_cast<TJSONPair*>(elem->Get("str"))
				->JsonValue->Value());
		}

	};

	monitor() {
		id = -1;
		timeStart = TDateTime(Now()).FormatString("dd.mm.yyyy hh:mm:zz");
	};

};

struct Act {
	FFile file;
	String act;
	String time;
	int id;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("file", file.ToJson());
		req->AddPair("act", act);
		req->AddPair("time", time);
		req->AddPair("id", id);
		return req->ToString();
	};

	Act(String s) {
		if (s == "") {
			return;
		}
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		file = FFile(req->GetValue("file")->Value().c_str());
		act = req->GetValue("act")->Value().c_str();
		time = req->GetValue("time")->Value().c_str();
		id = StrToInt(req->GetValue("id")->Value());
	};

	Act() {
		act = "";
		time = "";
		id = -1;
	};

	Act(String a, String b, int c) : act(a), time(b), id(c) {
	};
};

struct ans_Act {
	vector<Act>arr;

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

	ans_Act(String s) {
		if (s == "") {
			return;
		}
		auto tree = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		auto mas = (TJSONArray*) tree->GetValue("mas");
		for (int i = 0; i < mas->Count; i++) {
			arr.push_back(Act(mas->Items[i]->ToString()));
		}
	};

	ans_Act() {
		arr.clear();
	};

};
