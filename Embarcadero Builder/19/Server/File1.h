#pragma once
#include <vector>
#include <cstring>
#include <algorithm>
#include "System.JSON.hpp"
using namespace std;

struct rang {
	int id_rang;
	String name_rang;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_rang", id_rang);
		req->AddPair("name_rang", name_rang);
		return req->ToString();
	};

	rang(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id_rang = StrToInt(req->GetValue("id_rang")->Value());
		name_rang = req->GetValue("name_rang")->Value();
	};

	rang() {
		id_rang = 0;
		name_rang = " ";
	};

};

struct ans_rang {
	String table;
	vector<rang>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "rang");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_rang(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(rang(mas->Items[i]->ToString()));
		}
	}

	ans_rang() {
		table = "rang";
	};
};

struct role {
	int id_role;
	String name_role;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_role", id_role);
		req->AddPair("name_role", name_role);
		return req->ToString();
	};

	role(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id_role = StrToInt(req->GetValue("id_role")->Value());
		name_role = req->GetValue("name_role")->Value();
	};

	role() {
		id_role = 0;
		name_role = " ";
	};

};

struct ans_role {
	String table;
	vector<role>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "role");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_role(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(role(mas->Items[i]->ToString()));
		}
	}

	ans_role() {
		table = "role";
	};
};

struct VG {
	int id_vid;
	String name_vid;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_vid", id_vid);
		req->AddPair("name_vid", name_vid);
		return req->ToString();
	};

	VG(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id_vid = StrToInt(req->GetValue("id_vid")->Value());
		name_vid = req->GetValue("name_vid")->Value();
	};

	VG() {
		id_vid = 0;
		name_vid = " ";
	};

};

struct ans_VG { // vid_gun
	String table;
	vector<VG>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "VG");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_VG(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(VG(mas->Items[i]->ToString()));
		}
	}

	ans_VG() {
		table = "VG";
	};
};

struct person { // personnel
	int id_ls;
	String fio;
	String info;
	role ro;
	rang ra;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_ls", id_ls);
		req->AddPair("fio", fio);
		req->AddPair("info", info);
		auto a = (TJSONObject*)TJSONObject::ParseJSONValue(ro.ToJson());
		req->AddPair("role", a);
		auto b = (TJSONObject*)TJSONObject::ParseJSONValue(ra.ToJson());
		req->AddPair("rang", b);
		return req->ToString();
	};

	person(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id_ls = StrToInt(req->GetValue("id_ls")->Value());
		fio = req->GetValue("fio")->Value();
		info = req->GetValue("info")->Value();
		auto ro_js = req->GetValue("role");
		ro = role(ro_js->ToString());
		auto ra_js = req->GetValue("rang");
		ra = rang(ra_js->ToString());
	};

	person() {
		id_ls = 0;
		fio = " ";
		info = " ";
	};

};

struct ans_person {
	String table;
	vector<person>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "person");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_person(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(person(mas->Items[i]->ToString()));
		}
	}

	ans_person() {
		table = "person";
	};
};

struct GT { // gun_type
	VG vid;
	int id_gt;
	String name_gun;
	String info;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_gt", id_gt);
		req->AddPair("name_gun", name_gun);
		req->AddPair("info", info);
		auto a = (TJSONObject*)TJSONObject::ParseJSONValue(vid.ToJson());
		req->AddPair("VG", a);
		return req->ToString();
	};

	GT(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id_gt = StrToInt(req->GetValue("id_gt")->Value());
		name_gun = req->GetValue("name_gun")->Value();
		info = req->GetValue("info")->Value();
		auto v_js = req->GetValue("VG");
		vid = VG(v_js->ToString());
	};

	GT() {
		id_gt = 0;
		name_gun = " ";
		info = " ";
	};

};

struct ans_GT {
	String table;
	vector<GT>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "GT");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_GT(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(GT(mas->Items[i]->ToString()));
		}
	}

	ans_GT() {
		table = "GT";
	};
};

struct gun {
	GT type;
	int id_gun;
	String num;
	String serial;
	String info;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_gun", id_gun);
		req->AddPair("num", num);
		req->AddPair("serial", serial);
		req->AddPair("info", info);
		auto a = (TJSONObject*)TJSONObject::ParseJSONValue(type.ToJson());
		req->AddPair("GT", a);
		return req->ToString();
	};

	gun(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id_gun = StrToInt(req->GetValue("id_gun")->Value());
		num = req->GetValue("num")->Value();
		serial = req->GetValue("serial")->Value();
		info = req->GetValue("info")->Value();
		auto t_js = req->GetValue("GT");
		type = GT(t_js->ToString());

	};

	gun() {
		id_gun = 0;
		num = "0";
		serial = " ";
		info = " ";
	};

};

struct ans_gun {
	String table;
	vector<gun>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "gun");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_gun(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(gun(mas->Items[i]->ToString())); ;
		}
	}

	ans_gun() {
		table = "gun";
	};
};

struct zakrep {
	int id_zak;
	person ls;
	gun g;
	String date_zak;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_zak", id_zak);
		auto a = (TJSONObject*)TJSONObject::ParseJSONValue(ls.ToJson());
		req->AddPair("person", a);
		auto b = (TJSONObject*)TJSONObject::ParseJSONValue(g.ToJson());
		req->AddPair("gun", b);
		req->AddPair("date_zak", date_zak);
		return req->ToString();
	};

	zakrep(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		id_zak = StrToInt(req->GetValue("id_zak")->Value());
		date_zak = req->GetValue("date_zak")->Value();
		auto p_js = req->GetValue("person");
		ls = person(p_js->ToString());
		auto g_js = req->GetValue("gun");
		g = gun(g_js->ToString());
	};

	zakrep() {
		id_zak = 0;
		date_zak = "01.01.0001";
	};

};

struct ans_zak {
	String table;
	vector<zakrep>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "zakrep");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_zak(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(zakrep(mas->Items[i]->ToString())); ;
		}
	}

	ans_zak() {
		table = "zakrep";
	};
};

struct in_sklad {
	int id_ins;
	gun g;
	String date_ins;
	person dej;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_ins", id_ins);
		auto a = (TJSONObject*)TJSONObject::ParseJSONValue(g.ToJson());
		req->AddPair("gun", a);
		auto b = (TJSONObject*)TJSONObject::ParseJSONValue(dej.ToJson());
		req->AddPair("person", b);
		req->AddPair("date_ins", date_ins);
		return req->ToString();
	};

	in_sklad(String f) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(f);
		id_ins = StrToInt(req->GetValue("id_ins")->Value());
		date_ins = req->GetValue("date_ins")->Value();
		auto g_js = req->GetValue("gun");
		g = gun(g_js->ToString());
		auto p_js = req->GetValue("person");
		dej = person(p_js->ToString());
	};

	in_sklad() {
		id_ins = 0;
		date_ins = "01.01.0001";
	};

};

struct ans_ins {
	String table;
	vector<in_sklad>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "in_sklad");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_ins(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(in_sklad(mas->Items[i]->ToString())); ;
		}
	}

	ans_ins() {
		table = "in_sklad";
	};
};

struct out_sklad {
	int id_outs;
	in_sklad ins;
	String date_outs;
	person dej;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_outs", id_outs);
		auto a = (TJSONObject*)TJSONObject::ParseJSONValue(ins.ToJson());
		req->AddPair("in_sklad", a);
		auto b = (TJSONObject*)TJSONObject::ParseJSONValue(dej.ToJson());
		req->AddPair("person", b);
		req->AddPair("date_outs", date_outs);
		return req->ToString();
	};

	out_sklad(String f) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(f);
		id_outs = StrToInt(req->GetValue("id_outs")->Value());
		date_outs = req->GetValue("date_outs")->Value();
		auto i_js = req->GetValue("in_sklad");
		ins = in_sklad(i_js->ToString());
		auto p_js = req->GetValue("person");
		dej = person(p_js->ToString());
	};

	out_sklad() {
		id_outs = 0;
		date_outs = "01.01.0001";
	};

};

struct ans_outs {
	String table;
	vector<out_sklad>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "out_sklad");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_outs(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(out_sklad(mas->Items[i]->ToString())); ;
		}
	}

	ans_outs() {
		table = "out_sklad";
	};
};

struct out_p {
	int id_outp;
	zakrep zak;
	String date_outp;
	person dej;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_outp", id_outp);
		auto a = (TJSONObject*)TJSONObject::ParseJSONValue(zak.ToJson());
		req->AddPair("zakrep", a);
		auto b = (TJSONObject*)TJSONObject::ParseJSONValue(dej.ToJson());
		req->AddPair("person", b);
		req->AddPair("date_outp", date_outp);
		return req->ToString();
	};

	out_p(String f) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(f);
		id_outp = StrToInt(req->GetValue("id_outp")->Value());
		date_outp = req->GetValue("date_outp")->Value();
		auto z_js = req->GetValue("zakrep");
		zak = zakrep(z_js->ToString());
		auto p_js = req->GetValue("person");
		dej = person(p_js->ToString());
	};

	out_p() {
		id_outp = 0;
		date_outp = "01.01.0001";
	};

};

struct ans_outp {
	String table;
	vector<out_p>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "out_personnel");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_outp(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(out_p(mas->Items[i]->ToString())); ;
		}
	}

	ans_outp() {
		table = "out_personnel";
	};
};

struct in_p {
	int id_inp;
	out_p outp;
	String date_inp;
	person dej;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id_inp", id_inp);
		auto a = (TJSONObject*)TJSONObject::ParseJSONValue(outp.ToJson());
		req->AddPair("out_p", a);
		auto b = (TJSONObject*)TJSONObject::ParseJSONValue(dej.ToJson());
		req->AddPair("person", b);
		req->AddPair("date_inp", date_inp);
		return req->ToString();
	};

	in_p(String f) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(f);
		id_inp = StrToInt(req->GetValue("id_inp")->Value());
		date_inp = req->GetValue("date_inp")->Value();
		auto o_js = req->GetValue("out_p");
		outp = out_p(o_js->ToString());
		auto p_js = req->GetValue("person");
		dej = person(p_js->ToString());
	};

	in_p() {
		id_inp = 0;
		date_inp = "01.01.0001";
	};

};

struct ans_inp {
	String table;
	vector<in_p>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "in_personnel");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_inp(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(in_p(mas->Items[i]->ToString())); ;
		}
	}

	ans_inp() {
		table = "in_personnel";
	};
};

struct duty {
	String date_d;
	person dej;

	String ToJson() {
		auto req = new TJSONObject;
		auto b = (TJSONObject*)TJSONObject::ParseJSONValue(dej.ToJson());
		req->AddPair("person", b);
		req->AddPair("date_d", date_d);
		return req->ToString();
	};

	duty(String f) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(f);
		date_d = req->GetValue("date_d")->Value();
		auto p_js = req->GetValue("person");
		dej = person(p_js->ToString());
	};

	duty() {
		date_d = "01.01.0001";
	};

};

struct ans_duty {
	String table;
	vector<duty>data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("table", "duty");
		auto mas = new TJSONArray;
		for (auto s : data) {
			mas->AddElement((TJSONObject*)TJSONObject::ParseJSONValue
				(s.ToJson()));
		}
		req->AddPair("data", mas);
		return req->ToString();
	};

	ans_duty(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		table = req->GetValue("table")->Value().c_str();
		auto mas = (TJSONArray*) req->GetValue("data");
		for (int i = 0; i < mas->Count; i++) {
			data.push_back(duty(mas->Items[i]->ToString())); ;
		}
	}

	ans_duty() {
		table = "duty";
	};
};

struct com {
	String typ;
	String table_name;
	int key;
	String data;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("type", this->typ);
		req->AddPair("table", this->table_name);
		req->AddPair("key", this->key);
		req->AddPair("data", this->data);
		return req->ToString();
	};

	com(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		typ = req->GetValue("type")->Value().c_str();
		key = StrToInt(req->GetValue("key")->Value());
		table_name = req->GetValue("table")->Value().c_str();
		data = req->GetValue("data")->Value().c_str();
	};

	com() {
		data = "";
	};

	com(String a, String b, int k, String d) : table_name(a), typ(b), key(k),
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
};

struct id_date {
	int id;
	String date;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("id", this->id);
		req->AddPair("date", this->date);
		return req->ToString();
	};

	id_date(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		date = req->GetValue("date")->Value().c_str();
		id = StrToInt(req->GetValue("id")->Value());
	};

	id_date() {
	};

	id_date(int a, String b) : id(a), date(b) {
	};
};

struct two_date {
	String date1;
	String date2;
	int key1;
	int key2;

	String ToJson() {
		auto req = new TJSONObject;
		req->AddPair("date2", this->date2);
		req->AddPair("date1", this->date1);
		req->AddPair("key1", this->key1);
		req->AddPair("key2", this->key2);
		return req->ToString();
	};

	two_date(String s) {
		auto req = (TJSONObject*)TJSONObject::ParseJSONValue(s);
		date2 = req->GetValue("date2")->Value().c_str();
		date1 = req->GetValue("date1")->Value().c_str();
		key1 = StrToInt(req->GetValue("key1")->Value());
		key2 = StrToInt(req->GetValue("key2")->Value());
	};

	two_date() {
		date1 = "01.01.0001";
		date2 = "01.01.0001";
		key1 = 0;
		key2 = 0;
	};
};
