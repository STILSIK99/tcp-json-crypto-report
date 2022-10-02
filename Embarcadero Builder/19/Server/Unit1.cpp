// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	if (!initDBConnection()) {
		ShowMessage("Подключения нет!");
	}
	else
		ShowMessage("База успешно подключена!");
}

// ---------------------------------------------------------------------------
bool initDBConnection() {
	String filename = ChangeFileExt(Application->ExeName, ".ini");
	// ShowMessage(AnsiString(filename).c_str());
	TIniFile* iniFile = new TIniFile(AnsiString(filename).c_str());
	// iniFile->WriteString("Database", "connection",
	// Form1->FDConnection1->ConnectionString);
	String ConnectionString = iniFile->ReadString("Database", "connection", "");
	iniFile->Free();
	Form1->FDConnection1->ConnectionString = ConnectionString;
	// ShowMessage(ConnectionString);
	try {
		Form1->FDConnection1->Open();
	}
	catch (...) {
		return false;
	}
	return Form1->FDConnection1->Connected;
};

// ---------------------------------------------------------------------------
String TForm1::decrypt(TIdContext *AContext) {
	TMemoryStream * ms = new TMemoryStream;
	AContext->Connection->IOHandler->ReadStream(ms);
	unsigned long size = ms->Size;
	char * crypted = new char[size];
	ms->Seek(0, 0);
	ms->ReadBuffer(crypted, size);
	char * decrypted = 0;
	clients[AContext]->DecryptData(crypted, decrypted, size);
	ms->Free();
	return String((wchar_t*)decrypted);
};

void TForm1::send_mes(String request, TIdContext *AContext) {
	Memo1->Lines->Add(request);
	char* crypted = 0;
	unsigned long size = request.Length() * 2 + 2;
	clients[AContext]->CryptData((const char*)request.data(), crypted, size);
	TMemoryStream * ms = new TMemoryStream;
	ms->Write(crypted, size);
	delete[]crypted;
	ms->Seek(0, 0);
	AContext->Connection->IOHandler->WriteBufferOpen();
	AContext->Connection->IOHandler->Write(ms, -1, true);
	AContext->Connection->IOHandler->WriteBufferClose();
	ms->Free();
	AContext->Connection->Disconnect();
};

void __fastcall TForm1::IdTCPServer1Execute(TIdContext *AContext) {
	if (clients.count(AContext) == 0) {
		clients[AContext] = new CryptoSession("oruzheika");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		com req(request);

		Memo1->Lines->Add(AContext->Binding->PeerIP + " ---- " + req.ToJson());
		if (req.table_name == "users" && req.typ == "select") {
			answer = log(req.data);
		}
		if (req.table_name == "vid_gun") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_all_vid(req.data);
				}
			}
			if (req.typ == "update") {
				answer = change_vid(req.data);
			}
			if (req.typ == "insert") {
				answer = set_vid(req.data);
			}
			if (req.typ == "delete") {
				answer = del_vid(req.data);
			}
		}
		if (req.table_name == "gun_type") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_all_gt(req.data);
				}
			}
			if (req.typ == "update") {
				answer = change_gt(req.data);
			}
			if (req.typ == "insert") {
				answer = set_gt(req.data);
			}
			if (req.typ == "delete") {
				answer = del_gt(req.data);
			}
		}
		if (req.table_name == "personnel") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_all_pers(req.data);
				}
			}
			if (req.typ == "update") {
				answer = change_pers(req.data);
			}
			if (req.typ == "insert") {
				answer = set_pers(req.data);
			}
			if (req.typ == "delete") {
				answer = del_pers(req.data);
			}
		}
		if (req.table_name == "rang" && req.typ == "select") {
			answer = get_all_rang(req.data);
		}
		if (req.table_name == "role" && req.typ == "select") {
			answer = get_all_role(req.data);
		}
		if (req.table_name == "duty") {
			if (req.typ == "select") {
				if (req.key == 1) {
					answer = get_duty(req.data);
				}
			}
			if (req.typ == "update") {
				answer = change_duty(req.data);
			}
			if (req.typ == "insert") {
				answer = set_duty(req.data);
			}
			if (req.typ == "delete") {
				answer = del_duty(req.data);
			}
		}
		if (req.table_name == "zakreplenie") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_zak(req.data);
				}
				if (req.key == 1) {
					answer = get_zak_ls(req.data);
				}
			}
			if (req.typ == "update") {
				answer = change_zak(req.data);
			}
			if (req.typ == "insert") {
				answer = set_zak(req.data);
			}
			if (req.typ == "delete") {
				answer = del_zak(req.data);
			}
		}
		if (req.table_name == "gun") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_gun(req.data);
				}
				if (req.key == 1) {
					answer = get_gun_nozak(req.data);
				}
				if (req.key == 2) {
					answer = get_gun_zak(req.data);
				}
				if (req.key == 3) {
					answer = get_gun_from_pers(req.data);
				}
			}
			if (req.typ == "insert") {
				answer = set_gun(req.data);
			}
		}
		if (req.table_name == "out_personnel") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_outp(req.data);
				}
			}
			if (req.typ == "insert") {
				answer = set_outp(req.data);
			}
		}
		if (req.table_name == "in_personnel") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_inp(req.data);
				}
			}
			if (req.typ == "insert") {
				answer = set_inp(req.data);
			}
		}
		if (req.table_name == "out_sklad") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_gun_for_outs(req.data);
				}
				if (req.key == 1) {
					answer = get_outs(req.data);
				}
			}
			if (req.typ == "insert") {
				answer = set_gun_sklad(req.data);
			}
		}
		if (req.table_name == "in_sklad") {
			if (req.typ == "select") {
				if (req.key == 0) {
					answer = get_ins(req.data);
				}
			}
			if (req.typ == "insert") {
				answer = set_ins(req.data);
			}
		}

		if (answer == "") {
			simple_ans ans;
			ans.result = "Command error!";
			send_mes(ans.ToJson(), AContext);
		}
		else
			send_mes(answer, AContext);
		return;
	}
	catch (...) {
		simple_ans ans;
		ans.result = "Decrypt error!";
		AContext->Connection->IOHandler->WriteLn(ans.ToJson(), enUTF8);
		AContext->Connection->Disconnect();
		return;
	}
	AContext->Connection->Disconnect();
}

// ---------------------------------------------------------------------------

String TForm1::log(String data) {
	login n_w(data);
	UnicodeString tr = "select * from users where login = '" + n_w.log +
		"' and hash = '" + n_w.hash + "'";
	simple_ans ans;
	FDQuery1->SQL->Text = tr;
	FDQuery1->Active = 1;
	FDQuery1->First();
	if (!FDQuery1->Eof) {
		ans.result = FDQuery1->FieldByName("info")->Value;
		ans.key = 1;
	}
	else
		ans.result = "Login error!";
	FDQuery1->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_vid(String data) {
	ans_VG ans;
	FDTable1->Active = 1;
	for (FDTable1->First(); !FDTable1->Eof; FDTable1->Next()) {
		VG a;
		a.id_vid = StrToInt(FDTable1->FieldByName("id_vid")->Value);
		a.name_vid = FDTable1->FieldByName("name_vid")->Value;
		ans.data.push_back(a);
	}
	FDTable1->Active = 0;
	return ans.ToJson();
};

String TForm1::change_vid(String data) {
	VG a(data);
	simple_ans ans;
	FDTable1->Active = 1;
	for (FDTable1->First(); !FDTable1->Eof; FDTable1->Next()) {
		if (StrToInt(FDTable1->FieldByName("id_vid")->Value) != a.id_vid) {
			continue;
		}
		try {
			FDTable1->Edit();
			FDTable1->FieldByName("name_vid")->Value = a.name_vid;
			FDTable1->Post();
			FDTable1->Refresh();
			ans.result = "Успешно изменено!";
		}
		catch (...) {
			ans.result = "Ошибка изменения!";
		}
		break;
	}
	FDTable1->Active = 0;
	return ans.ToJson();
};

String TForm1::del_vid(String data) {
	VG a(data);
	simple_ans ans;
	FDTable1->Active = 1;
	for (FDTable1->First(); !FDTable1->Eof; FDTable1->Next()) {
		if (StrToInt(FDTable1->FieldByName("id_vid")->Value) != a.id_vid) {
			continue;
		}
		try {
			FDTable1->Delete();
			FDTable1->Refresh();
			ans.result = "Успешно удалено!";
		}
		catch (...) {
			ans.result = "Ошибка удаления!";
		}
		break;
	}
	FDTable1->Active = 0;
	return ans.ToJson();
};

String TForm1::set_vid(String data) {
	VG a(data);
	simple_ans ans;
	FDTable1->Active = 1;
	try {
		FDTable1->Insert();
		FDTable1->FieldByName("name_vid")->Value = a.name_vid;
		FDTable1->Post();
		FDTable1->Refresh();
		ans.result = "Успешно добавлено!";
	}
	catch (...) {
		ans.result = "Ошибка добавления!";
	}
	FDTable1->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_gt(String data) {
	ans_GT ans;
	UnicodeString tr = "";
	tr += "select gt.*, vg.name_vid from vid_gun vg, gun_type gt where vg.id_vid = gt.id_vid";
	Memo1->Lines->Add(tr);
	FDQuery2->SQL->Text = tr;
	FDQuery2->Active = 1;
	for (FDQuery2->First(); !FDQuery2->Eof; FDQuery2->Next()) {
		GT a;
		a.id_gt = StrToInt(FDQuery2->FieldByName("id_gt")->Value);
		a.vid.id_vid = StrToInt(FDQuery2->FieldByName("id_vid")->Value);
		a.vid.name_vid = FDQuery2->FieldByName("name_vid")->Value;
		a.name_gun = FDQuery2->FieldByName("name_gun")->Value;
		a.info = FDQuery2->FieldByName("info")->Value;
		ans.data.push_back(a);
	}
	FDQuery2->Active = 0;
	return ans.ToJson();
};

String TForm1::change_gt(String data) {
	GT a(data);
	simple_ans ans;
	FDTable2->Active = 1;
	for (FDTable2->First(); !FDTable2->Eof; FDTable2->Next()) {
		if (StrToInt(FDTable2->FieldByName("id_gt")->Value) != a.id_gt) {
			continue;
		}
		try {
			FDTable2->Edit();
			FDTable2->FieldByName("id_vid")->Value = a.vid.id_vid;
			FDTable2->FieldByName("name_gun")->Value = a.name_gun;
			FDTable2->FieldByName("info")->Value = a.info;
			FDTable2->Post();
			FDTable2->Refresh();
			ans.result = "Успешно изменено!";
		}
		catch (...) {
			ans.result = "Ошибка изменения!";
		}
		break;
	}
	FDTable2->Active = 0;
	return ans.ToJson();
};

String TForm1::set_gt(String data) {
	GT a(data);
	simple_ans ans;
	FDTable2->Active = 1;
	try {
		FDTable2->Insert();
		FDTable2->FieldByName("id_vid")->Value = a.vid.id_vid;
		FDTable2->FieldByName("name_gun")->Value = a.name_gun;
		FDTable2->FieldByName("info")->Value = a.info;
		FDTable2->Post();
		FDTable2->Refresh();
		ans.result = "Успешно добавлено!";
	}
	catch (...) {
		ans.result = "Ошибка добавления!";
	}
	FDTable2->Active = 0;
	return ans.ToJson();
};

String TForm1::del_gt(String data) {
	GT a(data);
	simple_ans ans;
	FDTable2->Active = 1;
	for (FDTable2->First(); !FDTable2->Eof; FDTable2->Next()) {
		if (StrToInt(FDTable2->FieldByName("id_gt")->Value) != a.id_gt) {
			continue;
		}
		try {
			FDTable2->Delete();
			FDTable2->Refresh();
			ans.result = "Успешно удалено!";
		}
		catch (...) {
			ans.result = "Ошибка удаления!";
		}
		break;
	}
	FDTable2->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_pers(String data) {
	ans_person ans;
	UnicodeString tr = "";
	tr += "select p.*, ro.*, ra.* from personnel p, roles ro, rang ra where ";
	tr += "p.id_role = ro.id_role and p.id_rang = ra.id_rang ";
	FDQuery3->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery3->Active = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		person a;
		a.id_ls = StrToInt(FDQuery3->FieldByName("id_ls")->Value);
		a.ra.id_rang = StrToInt(FDQuery3->FieldByName("id_rang")->Value);
		a.ro.id_role = StrToInt(FDQuery3->FieldByName("id_role")->Value);
		a.ra.name_rang = FDQuery3->FieldByName("name_rang")->Value;
		a.ro.name_role = FDQuery3->FieldByName("name_role")->Value;
		a.fio = FDQuery3->FieldByName("fio")->Value;
		a.info = FDQuery3->FieldByName("info")->Value;
		ans.data.push_back(a);
	}
	FDQuery3->Active = 0;
	return ans.ToJson();
};

String TForm1::change_pers(String data) {
	person a(data);
	simple_ans ans;
	FDTable3->Active = 1;
	for (FDTable3->First(); !FDTable3->Eof; FDTable3->Next()) {
		if (StrToInt(FDTable3->FieldByName("id_ls")->Value) != a.id_ls) {
			continue;
		}
		try {
			FDTable3->Edit();
			FDTable3->FieldByName("id_rang")->Value = a.ra.id_rang;
			FDTable3->FieldByName("fio")->Value = a.fio;
			FDTable3->FieldByName("info")->Value = a.info;
			FDTable3->FieldByName("id_role")->Value = a.ro.id_role;
			FDTable3->Post();
			ans.result = "Успешно изменено!";
		}
		catch (...) {
			ans.result = "Ошибка изменения!";
		}
		FDTable3->Active = 0;
		break;

	}
	return ans.ToJson();
};

String TForm1::set_pers(String data) {
	person a(data);
	simple_ans ans;
	FDTable3->Active = 1;
	try {
		FDTable3->Insert();
		FDTable3->FieldByName("id_role")->Value = a.ro.id_role;
		FDTable3->FieldByName("id_rang")->Value = a.ra.id_rang;
		FDTable3->FieldByName("info")->Value = a.info;
		FDTable3->FieldByName("fio")->Value = a.fio;
		FDTable3->Post();
		FDTable3->Refresh();
		ans.result = "Успешно добавлено!";
	}
	catch (...) {
		ans.result = "Ошибка добавления!";
	}
	FDTable3->Active = 0;
	return ans.ToJson();
};

String TForm1::del_pers(String data) {
	person a(data);
	simple_ans ans;
	FDTable3->Active = 1;
	for (FDTable3->First(); !FDTable3->Eof; FDTable3->Next()) {
		if (StrToInt(FDTable3->FieldByName("id_ls")->Value) != a.id_ls) {
			continue;
		}
		try {
			FDTable3->Delete();
			FDTable3->Refresh();
			ans.result = "Успешно удалено!";
		}
		catch (...) {
			ans.result = "Ошибка удаления!";
		}
		break;
	}
	FDTable3->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_rang(String data) {
	ans_rang ans;
	FDTable4->Active = 1;
	for (FDTable4->First(); !FDTable4->Eof; FDTable4->Next()) {
		rang a;
		a.id_rang = StrToInt(FDTable4->FieldByName("id_rang")->Value);
		a.name_rang = FDTable4->FieldByName("name_rang")->Value;
		ans.data.push_back(a);
	}
	FDTable4->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_role(String data) {
	ans_role ans;
	FDTable5->Active = 1;
	for (FDTable5->First(); !FDTable5->Eof; FDTable5->Next()) {
		role a;
		a.id_role = StrToInt(FDTable5->FieldByName("id_role")->Value);
		a.name_role = FDTable5->FieldByName("name_role")->Value;
		ans.data.push_back(a);
	}
	FDTable5->Active = 0;
	return ans.ToJson();
};

String TForm1::get_duty(String data) {
	ans_duty ans;
	two_date b(data);
	UnicodeString tr = "";
	tr += "select p.*, r.name_rang, d.date_d from personnel p,duty d, rang r where r.id_rang = p.id_rang and p.id_ls = d.id_ls and d.date_d between '";
	tr += b.date1 + "' and '" + b.date2 + "'";
	Memo1->Lines->Add(tr);
	FDQuery4->SQL->Text = tr;
	FDQuery4->Active = 1;
	for (FDQuery4->First(); !FDQuery4->Eof; FDQuery4->Next()) {
		duty a;
		a.dej.id_ls = StrToInt(FDQuery4->FieldByName("id_ls")->Value);
		a.dej.fio = FDQuery4->FieldByName("fio")->Value; ;
		a.dej.ra.name_rang = FDQuery4->FieldByName("name_rang")->Value;
		a.date_d = FDQuery4->FieldByName("date_d")->Value;
		ans.data.push_back(a);
	}
	FDQuery4->Active = 0;
	return ans.ToJson();
};

String TForm1::change_duty(String data) {
	duty a(data);
	simple_ans ans;
	FDTable6->Active = 1;
	for (FDTable6->First(); !FDTable6->Eof; FDTable6->Next()) {
		String d = FDTable6->FieldByName("date_d")->Value;
		if (d != a.date_d) {
			continue;
		}
		try {
			FDTable6->Edit();
			FDTable6->FieldByName("id_ls")->Value = a.dej.id_ls;
			FDTable6->Post();
			FDTable6->Refresh();
			ans.result = "Успешно изменено!";
		}
		catch (...) {
			ans.result = "Ошибка изменения!";
		}
		break;
	}
	FDTable6->Active = 0;
	return ans.ToJson();
};

String TForm1::del_duty(String data) {
	duty a(data);
	simple_ans ans;
	FDTable6->Active = 1;
	for (FDTable6->First(); !FDTable6->Eof; FDTable6->Next()) {
		String d = FDTable6->FieldByName("date_d")->Value;
		if (d != a.date_d) {
			continue;
		}
		try {
			FDTable6->Delete();
			FDTable6->Refresh();
			ans.result = "Успешно удалено!";
		}
		catch (...) {
			ans.result = "Ошибка удаления!";
		}
		break;
	}
	FDTable6->Active = 0;
	return ans.ToJson();
};

String TForm1::set_duty(String data) {
	duty a(data);
	simple_ans ans;
	FDTable6->Active = 1;
	try {
		FDTable6->Insert();
		FDTable6->FieldByName("id_ls")->Value = a.dej.id_ls;
		FDTable6->FieldByName("date_d")->Value = a.date_d;
		FDTable6->Post();
		FDTable6->Refresh();
		ans.result = "Успешно добавлено!";
	}
	catch (...) {
		ans.result = "Ошибка добавления!";
	}
	FDTable6->Active = 0;
	return ans.ToJson();
};

String TForm1::get_zak(String data) {
	ans_zak ans;
	UnicodeString tr = "";
	tr += "select p.*,g.*, gt.name_gun, z.id_zak, r.name_rang ";
	tr += "from zakreplenie z, personnel p, gun g, gun_type gt, rang r ";
	tr += "where z.id_ls = p.id_ls and z.id_gun = g.id_gun and g.id_gt = gt.id_gt and r.id_rang = p.id_rang";
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		zakrep a;
		a.id_zak = StrToInt(FDQuery5->FieldByName("id_zak")->Value);
		a.ls.id_ls = StrToInt(FDQuery5->FieldByName("id_ls")->Value);
		a.ls.fio = FDQuery5->FieldByName("fio")->Value; ;
		a.ls.ra.name_rang = FDQuery5->FieldByName("name_rang")->Value;
		a.date_zak = FDQuery5->FieldByName("date_zak")->Value;
		a.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.g.num = FDQuery5->FieldByName("num")->Value;
		a.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::change_zak(String data) {
	zakrep a(data);
	simple_ans ans;
	FDTable7->Active = 1;
	for (FDTable7->First(); !FDTable6->Eof; FDTable6->Next()) {
		int d = StrToInt(FDTable7->FieldByName("id_zak")->Value);
		if (d != a.id_zak) {
			continue;
		}
		try {
			FDTable7->Edit();
			FDTable7->FieldByName("id_ls")->Value = a.ls.id_ls;
			FDTable7->FieldByName("id_gun")->Value = a.g.id_gun;
			FDTable7->Post();
			FDTable7->Refresh();
			ans.result = "Успешно изменено!";
		}
		catch (...) {
			ans.result = "Ошибка изменения!";
		}
		break;
	}
	FDTable7->Active = 0;
	return ans.ToJson();
};

String TForm1::del_zak(String data) {
	zakrep a(data);
	simple_ans ans;
	FDTable7->Active = 1;
	for (FDTable7->First(); !FDTable7->Eof; FDTable7->Next()) {
		int d = StrToInt(FDTable7->FieldByName("id_zak")->Value);
		if (d != a.id_zak) {
			continue;
		}
		try {
			FDTable7->Delete();
			FDTable7->Refresh();
			ans.result = "Успешно удалено!";
		}
		catch (...) {
			ans.result = "Ошибка удаления!";
		}
		break;
	}
	FDTable7->Active = 0;
	return ans.ToJson();
};

String TForm1::set_zak(String data) {
	zakrep a(data);
	simple_ans ans;
	FDTable7->Active = 1;
	try {
		FDTable7->Insert();
		FDTable7->FieldByName("id_ls")->Value = a.ls.id_ls;
		FDTable7->FieldByName("id_gun")->Value = a.g.id_gun;
		FDTable7->Post();
		FDTable7->Refresh();
		ans.result = "Успешно добавлено!";
	}
	catch (...) {
		ans.result = "Ошибка добавления!";
	}
	FDTable7->Active = 0;
	return ans.ToJson();
};

String TForm1::get_gun(String data) {
	ans_zak ans;
	UnicodeString tr = "";
	tr += "select p.*,g.*, gt.name_gun, z.id_zak, r.name_rang ";
	tr += "from zakreplenie z, personnel p, gun g, gun_type gt, rang r ";
	tr += "where z.id_ls = p.id_ls and z.id_gun = g.id_gun and g.id_gt = gt.id_gt and r.id_rang = p.id_rang";
	tr += " ";
	Memo1->Lines->Add(tr);
	FDQuery5->SQL->Text = tr;
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		zakrep a;
		a.id_zak = StrToInt(FDQuery5->FieldByName("id_zak")->Value);
		a.ls.id_ls = StrToInt(FDQuery5->FieldByName("id_ls")->Value);
		a.ls.fio = FDQuery5->FieldByName("fio")->Value; ;
		a.ls.ra.name_rang = FDQuery5->FieldByName("name_rang")->Value;
		a.date_zak = FDQuery5->FieldByName("date_zak")->Value;
		a.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.g.num = FDQuery5->FieldByName("num")->Value;
		a.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::set_gun(String data) {
	ans_gun a(data);
	simple_ans ans;
	FDTable12->Active = 1;

	for (auto x : a.data) {
		try {
			UnicodeString tr = "select * from gun where id_gt = ";
			tr += String(x.type.id_gt) + " and num = '" + x.num;
			tr += "' and serial = '" + x.serial;
			tr += "'";
			FDQuery5->SQL->Text = tr;
			FDQuery5->Active = 1;
			FDQuery5->First();
			if (!FDQuery5->Eof) {
				x.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
				FDQuery5->Active = 0;
			}
			else {
				FDTable12->Insert();
				FDTable12->FieldByName("id_gt")->Value = x.type.id_gt;
				FDTable12->FieldByName("serial")->Value = x.serial;
				FDTable12->FieldByName("num")->Value = x.num;
				FDTable12->FieldByName("info")->Value = x.info;
				FDTable12->Post();
				FDTable12->Refresh();
				UnicodeString tr = " ";
				tr += " select id_gun from gun where serial = '" + x.serial +
					"' and num = '" + x.num + "'";
				Memo1->Lines->Add(tr);
				FDQuery8->SQL->Text = tr;
				FDQuery8->Active = 1;
				FDQuery8->First();
				if (!FDQuery8->Eof) {
					x.id_gun = StrToInt(FDQuery8->FieldByName("id_gun")->Value);
				}
				FDQuery8->Active = 0;
			}
			FDTable10->Active = 1;
			FDTable10->Insert();
			FDTable10->FieldByName("id_gun")->Value = x.id_gun;
			FDTable10->FieldByName("date_ins")->Value = DateToStr(Now());
			FDTable10->FieldByName("id_ls")->Value = get_dej(DateToStr(Now()));
			FDTable10->Post();
			FDTable10->Refresh();
			FDTable10->Active = 0;
		}
		catch (...) {
			ans.key++;
		}
	}
	ans.result = " При обработке произошло " + String(ans.key) + " ошибок !";
	FDTable12->Active = 0;
	return ans.ToJson();
};

int TForm1::get_dej(String data) {
	UnicodeString tr = " select id_ls from duty where date_d = '";
	tr += data + "'";
	FDQuery6->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery6->Active = 1;
	FDQuery6->First();
	if (!FDQuery6->Eof) {
		return StrToInt(FDQuery6->FieldByName("id_ls")->Value);
	}
	return 0;
};

String TForm1::get_gun_zak(String data) {
	ans_zak ans;
	UnicodeString tr = " ";
	tr += " select gt.name_gun, g.*, p.*, z.id_zak, r.name_rang from KXO, gun_type gt, gun g, zakreplenie z, personnel p, rang r ";
	tr += " where KXO.id_gun = z.id_gun and z.id_ls = p.id_ls and r.id_rang = p.id_rang ";
	tr += " and g.id_gun = KXO.id_gun and g.id_gt = gt.id_gt and gt.id_vid = 1 ";
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		zakrep a;
		a.id_zak = StrToInt(FDQuery5->FieldByName("id_zak")->Value);
		a.ls.id_ls = StrToInt(FDQuery5->FieldByName("id_ls")->Value);
		a.ls.fio = FDQuery5->FieldByName("fio")->Value; ;
		a.ls.ra.name_rang = FDQuery5->FieldByName("name_rang")->Value;
		a.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.g.info = FDQuery5->FieldByName("info")->Value;
		a.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.g.num = FDQuery5->FieldByName("num")->Value;
		a.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::get_gun_nozak(String data) {
	ans_gun ans;
	UnicodeString tr = " ";
	tr += " select gt.name_gun, g.*from KXO, gun_type gt, gun g ";
	tr += " where KXO.id_gun<>all(select id_gun from zakreplenie)";
	tr += " and g.id_gun = KXO.id_gun and g.id_gt = gt.id_gt ";
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		gun a;
		a.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.serial = FDQuery5->FieldByName("serial")->Value;
		a.num = FDQuery5->FieldByName("num")->Value;
		a.info = FDQuery5->FieldByName("info")->Value;
		a.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::get_zak_ls(String data) {
	ans_zak ans;
	UnicodeString tr = " ";
	tr += " select g.*, gt.name_gun, z.id_zak from zakreplenie z, gun g, ";
	tr += " gun_type gt where z.id_gun = g.id_gun and gt.id_gt = g.id_gt ";
	tr += " and z.id_ls = " + data;
	tr += " and z.id_gun = any(select id_gun from KXO)";
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		zakrep a;
		a.id_zak = StrToInt(FDQuery5->FieldByName("id_zak")->Value);
		a.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.g.num = FDQuery5->FieldByName("num")->Value;
		a.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::get_gun_from_pers(String data) {
	ans_outp ans;
	UnicodeString tr = " ";
	tr += " select gt.name_gun, g.*, p.*, z.id_zak, r.name_rang, op.id_outp ";
	tr += " from out_personnel op, zakreplenie z, gun g, ";
	tr += " rang r, gun_type gt, personnel p ";
	tr += " where op.id_outp<>all(select id_outp from in_personnel) and z.id_ls = p.id_ls ";
	tr += " and r.id_rang = p.id_rang ";
	tr += " and op.id_zak = z.id_zak and z.id_gun = g.id_gun and g.id_gt = gt.id_gt ";
	tr += " and z.id_ls = " + data;
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		out_p a;
		a.id_outp = StrToInt(FDQuery5->FieldByName("id_outp")->Value);
		a.zak.id_zak = StrToInt(FDQuery5->FieldByName("id_zak")->Value);
		a.zak.ls.id_ls = StrToInt(FDQuery5->FieldByName("id_ls")->Value);
		a.zak.ls.fio = FDQuery5->FieldByName("fio")->Value; ;
		a.zak.ls.ra.name_rang = FDQuery5->FieldByName("name_rang")->Value;
		a.zak.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.zak.g.info = FDQuery5->FieldByName("info")->Value;
		a.zak.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.zak.g.num = FDQuery5->FieldByName("num")->Value;
		a.zak.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::get_outp(String data) {
	two_date a(data);
	ans_outp ans;
	UnicodeString tr = " ";
	tr += " select op.date_outp, p.*, r.name_rang, g.*, gt.name_gun, z.id_zak ";
	tr += " from out_personnel op, zakreplenie z, personnel p, gun g, gun_type gt, rang r ";
	tr += " where op.id_zak = z.id_zak and z.id_ls = p.id_ls and p.id_rang = r.id_rang and ";
	tr += " z.id_gun = g.id_gun and gt.id_gt = g.id_gt and op.date_outp between '";
	tr += a.date1 + "' and '" + a.date2 + "'";
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		out_p a;
		a.date_outp = FDQuery5->FieldByName("date_outp")->Value;
		a.zak.id_zak = StrToInt(FDQuery5->FieldByName("id_zak")->Value);
		a.zak.ls.id_ls = StrToInt(FDQuery5->FieldByName("id_ls")->Value);
		a.zak.ls.fio = FDQuery5->FieldByName("fio")->Value; ;
		a.zak.ls.ra.name_rang = FDQuery5->FieldByName("name_rang")->Value;
		a.zak.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.zak.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.zak.g.num = FDQuery5->FieldByName("num")->Value;
		a.zak.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		// получение данных дежурного
		{
			UnicodeString tr = " ";
			tr += " select p.*, r.name_rang from personnel p, rang r ";
			tr += " where r.id_rang = p.id_rang and p.id_ls = ";
			tr += String(get_dej(DateToStr(a.date_outp)));
			FDQuery7->SQL->Text = tr;
			Memo1->Lines->Add(tr);
			FDQuery7->Active = 1;
			FDQuery7->First();
			if (!FDQuery7->Eof) {
				a.dej.ra.name_rang = FDQuery7->FieldByName("name_rang")->Value;
				a.dej.fio = FDQuery7->FieldByName("fio")->Value;
			}
			FDQuery7->Active = 0;
			//
		} ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::set_outp(String data) {
	out_p a(data);
	simple_ans ans;
	FDTable8->Active = 1;
	try {
		FDTable8->Insert();
		FDTable8->FieldByName("id_zak")->Value = a.zak.id_zak;
		FDTable8->FieldByName("date_outp")->Value = DateToStr(Now());
		FDTable8->FieldByName("id_ls")->Value = get_dej(DateToStr(Now()));
		FDTable8->Post();
		FDTable8->Refresh();
		ans.result = " Успешно добавлено !";
	}
	catch (...) {
		ans.result = " Ошибка добавления !";
	}
	FDTable8->Active = 0;
	return ans.ToJson();
};

String TForm1::get_inp(String data) {
	two_date a(data);
	ans_inp ans;
	UnicodeString tr = " ";
	tr += " select ip.date_inp, p.*, r.name_rang, g.*, gt.name_gun, z.id_zak ";
	tr += " from out_personnel op, zakreplenie z, personnel p, gun g, gun_type gt, rang r, in_personnel ip ";
	tr += " where op.id_zak = z.id_zak and z.id_ls = p.id_ls and p.id_rang = r.id_rang and ";
	tr += " z.id_gun = g.id_gun and gt.id_gt = g.id_gt and ip.id_outp = op.id_outp and ip.date_inp ";
	tr += " between '" + a.date1 + "' and '" + a.date2 + "'";
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		in_p a;
		a.date_inp = FDQuery5->FieldByName("date_inp")->Value;
		a.outp.zak.id_zak = StrToInt(FDQuery5->FieldByName("id_zak")->Value);
		a.outp.zak.ls.id_ls = StrToInt(FDQuery5->FieldByName("id_ls")->Value);
		a.outp.zak.ls.fio = FDQuery5->FieldByName("fio")->Value; ;
		a.outp.zak.ls.ra.name_rang = FDQuery5->FieldByName("name_rang")->Value;
		a.outp.zak.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.outp.zak.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.outp.zak.g.num = FDQuery5->FieldByName("num")->Value;
		a.outp.zak.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		// получение данных дежурного
		{
			UnicodeString tr = " ";
			tr += " select p.*, r.name_rang from personnel p, rang r where r.id_rang = ";
			tr += " p.id_rang and p.id_ls = ";
			tr += String(get_dej(a.date_inp));
			FDQuery7->SQL->Text = tr;
			Memo1->Lines->Add(tr);
			FDQuery7->Active = 1;
			FDQuery7->First();
			if (!FDQuery7->Eof) {
				a.dej.ra.name_rang = FDQuery7->FieldByName("name_rang")->Value;
				a.dej.fio = FDQuery7->FieldByName("fio")->Value;
			}
			FDQuery7->Active = 0;
			//
		} ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::set_inp(String data) {
	in_p a(data);
	simple_ans ans;
	FDTable9->Active = 1;
	try {
		FDTable9->Insert();
		FDTable9->FieldByName("id_outp")->Value = a.outp.id_outp;
		FDTable9->FieldByName("date_inp")->Value = DateToStr(Now());
		FDTable9->FieldByName("id_ls")->Value = get_dej(DateToStr(Now()));
		FDTable9->Post();
		FDTable9->Refresh();
		ans.result = " Успешно добавлено !";
	}
	catch (...) {
		ans.result = " Ошибка добавления !";
	}
	FDTable9->Active = 0;
	return ans.ToJson();
};

String TForm1::get_ins(String data) {
	two_date a(data);
	ans_ins ans;
	UnicodeString tr = " ";
	tr += " select gt.name_gun, g.*, p.*, r.name_rang, v.name_vid, ins.date_ins ";
	tr += " from in_sklad ins, gun g, gun_type gt, personnel p, rang r, vid_gun v ";
	tr += " where ins.id_gun = g.id_gun and ins.id_ls = p.id_ls and v.id_vid = gt.id_vid and ";
	tr += " gt.id_gt = g.id_gt and p.id_rang = r.id_rang and ins.date_ins ";
	tr += " between '" + a.date1 + "' and '" + a.date2 + "'";
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		in_sklad a;
		a.date_ins = FDQuery5->FieldByName("date_ins")->Value;
		a.dej.id_ls = StrToInt(FDQuery5->FieldByName("id_ls")->Value);
		a.dej.fio = FDQuery5->FieldByName("fio")->Value; ;
		a.dej.ra.name_rang = FDQuery5->FieldByName("name_rang")->Value;
		a.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.g.num = FDQuery5->FieldByName("num")->Value;
		a.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		a.g.type.vid.name_vid = FDQuery5->FieldByName("name_vid")->Value;
		ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::set_ins(String data) {
	in_sklad a(data);
	simple_ans ans;
	FDTable10->Active = 1;
	try {
		FDTable10->Insert();
		FDTable10->FieldByName("id_gun")->Value = a.g.id_gun;
		FDTable10->FieldByName("date_ins")->Value = DateToStr(Now());
		FDTable10->FieldByName("id_ls")->Value = get_dej(DateToStr(Now()));
		FDTable10->Post();
		FDTable10->Refresh();
		ans.result = " Успешно добавлено !";
	}
	catch (...) {
		ans.result = " Ошибка добавления !";
	}
	FDTable10->Active = 0;
	return ans.ToJson();
};

String TForm1::get_outs(String data) {
	two_date a(data);
	ans_outs ans;
	UnicodeString tr = " ";
	tr += " select gt.name_gun, g.*, p.*, r.name_rang, v.name_vid, outs.date_outs ";
	tr += " from in_sklad ins, gun g, gun_type gt, personnel p, rang r, vid_gun v, out_sklad outs ";
	tr += " where ins.id_gun = g.id_gun and outs.id_ls = p.id_ls and v.id_vid = ";
	tr += " gt.id_vid and outs.id_ins = ins.id_ins and ";
	tr += " gt.id_gt = g.id_gt and p.id_rang = r.id_rang and ins.date_ins ";
	tr += " between '" + a.date1 + "' and '" + a.date2 + "'";
	FDQuery5->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		out_sklad a;
		a.date_outs = FDQuery5->FieldByName("date_outs")->Value;
		a.dej.id_ls = StrToInt(FDQuery5->FieldByName("id_ls")->Value);
		a.dej.fio = FDQuery5->FieldByName("fio")->Value; ;
		a.dej.ra.name_rang = FDQuery5->FieldByName("name_rang")->Value;
		a.ins.g.id_gun = StrToInt(FDQuery5->FieldByName("id_gun")->Value);
		a.ins.g.serial = FDQuery5->FieldByName("serial")->Value;
		a.ins.g.num = FDQuery5->FieldByName("num")->Value;
		a.ins.g.type.name_gun = FDQuery5->FieldByName("name_gun")->Value;
		a.ins.g.type.vid.name_vid = FDQuery5->FieldByName("name_vid")->Value;
		ans.data.push_back(a);
	}
	FDQuery5->Active = 0;
	return ans.ToJson();
};

String TForm1::set_outs(String data) {
	out_sklad a(data);
	simple_ans ans;
	FDTable11->Active = 1;
	try {
		FDTable11->Insert();
		FDTable11->FieldByName("id_ins")->Value = a.ins.id_ins;
		FDTable11->FieldByName("date_outs")->Value = DateToStr(Now());
		FDTable11->FieldByName("id_ls")->Value = get_dej(DateToStr(Now()));
		FDTable11->Post();
		FDTable11->Refresh();
		ans.result = " Успешно добавлено !";
	}
	catch (...) {
		ans.result = " Ошибка добавления !";
	}
	FDTable11->Active = 0;
	return ans.ToJson();
};

String TForm1::get_gun_for_outs(String data) {
	ans_gun ans;
	UnicodeString tr = " ";
	tr += " select g.*, gt.name_gun, vg.name_vid ";
	tr += " from gun g, gun_type gt, vid_gun vg ";
	tr += " where g.id_gun = any(select id_gun from KXO) and ";
	tr += " g.id_gt = gt.id_gt and vg.id_vid = gt.id_vid ";
	FDQuery8->SQL->Text = tr;
	Memo1->Lines->Add(tr);
	FDQuery8->Active = 1;
	for (FDQuery8->First(); !FDQuery8->Eof; FDQuery8->Next()) {
		gun a;
		a.id_gun = StrToInt(FDQuery8->FieldByName("id_gun")->Value);
		a.serial = FDQuery8->FieldByName("serial")->Value;
		a.num = FDQuery8->FieldByName("num")->Value;
		a.type.name_gun = FDQuery8->FieldByName("name_gun")->Value;
		a.type.vid.name_vid = FDQuery8->FieldByName("name_vid")->Value;
		a.info = FDQuery8->FieldByName("name_vid")->Value;
		ans.data.push_back(a);
	}
	FDQuery8->Active = 0;
	return ans.ToJson();
};

String TForm1::set_gun_sklad(String data) {
	ans_gun a(data);
	simple_ans ans;
	FDTable13->Active = 1;
	for (auto x : a.data) {
		try {
			FDTable13->Insert();
			//
			int f; {
				UnicodeString tr = " ";
				tr += " select id_ins from in_sklad where id_ins <> all (select id_ins ";
				tr += " from out_sklad) and id_gun = ";
				tr += String(x.id_gun);
				FDQuery8->SQL->Text = tr;
				Memo1->Lines->Add(tr);
				FDQuery8->Active = 1;
				FDQuery8->First();
				if (FDQuery8->Eof) {
					FDQuery8->Active = 0;
					continue;
				}
				f = StrToInt(FDQuery8->FieldByName("id_ins")->Value);
				FDQuery8->Active = 0;
			}
			//
			FDTable13->FieldByName("id_ins")->Value = f;
			FDTable13->FieldByName("date_outs")->Value = DateToStr(Now());
			FDTable13->FieldByName("id_ls")->Value = get_dej(DateToStr(Now()));
			FDTable13->Post();
			FDTable13->Refresh();
			// ans.result = " Успешно добавлено !";
		}
		catch (...) {
			ans.key++;
		}

	}
	ans.result = " При обработке произошло " + String(ans.key) + " ошибок !";
	FDTable13->Active = 0;
	return ans.ToJson();
};
