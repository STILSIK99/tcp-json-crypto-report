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
	if (initDBConnection())
		ShowMessage("Connected");
	else
		ShowMessage("Connection error!");

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Название";
	StringGrid1->Cells[2][0] = "Дата добавления";

	StringGrid1->ColWidths[0] = 50;
	StringGrid1->ColWidths[1] = 200;
	StringGrid1->ColWidths[2] = 200;

	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Название файла";
	StringGrid2->Cells[2][0] = "Имя пользователя";
	StringGrid2->Cells[3][0] = "Время обращения";

	StringGrid2->ColWidths[0] = 50;
	StringGrid2->ColWidths[1] = 200;
	StringGrid2->ColWidths[2] = 200;
	StringGrid2->ColWidths[3] = 200;
	loadData();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
bool TForm1::initDBConnection() {
	String filename = ChangeFileExt(Application->ExeName, ".ini");
	// ShowMessage(AnsiString(filename).c_str());
	TIniFile* iniFile = new TIniFile(AnsiString(filename).c_str());
	// iniFile->WriteString("Database", "connection",
	// Form1->FDConnection1->ConnectionString);
	String ConnectionString = iniFile->ReadString("Database", "connection", "");
	iniFile->Free();
	Form1->FDConnection1->ConnectionString = ConnectionString;
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

// ---------------------------------------------------------------------------

void __fastcall TForm1::ServerExecute(TIdContext *AContext) {
	if (clients.count(AContext) == 0) {
		clients[AContext] = new CryptoSession("watcher");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text += request;
		com req(request);

		Memo1->Lines->Add(AContext->Binding->PeerIP + " ---- " + req.ToJson());
		if (req.table_name == "login" && req.type == "select") {
			answer = log(req.data, AContext->Binding->PeerIP);
		}
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 1).ToJson();
		}

		if (req.table_name == "files") {
			if (req.type == "select") {
				if (req.key == 0)
					answer = get_all_files();
				if (req.key == 1) {
					answer = get_source_file(req.data);
				}
				if (req.key == 2) {
					answer = get_file_by_name(req.data);
				}
			}
			if (req.type == "insert") {
				if (req.key == 0) {
					answer = ins_files(req.data);
				}
			}
			if (req.type == "update") {
				if (req.key == 0) {
					answer = upd_file(req.data);
				}
			}
			if (req.type == "delete") {
				if (req.key == 0) {
					answer = del_files(req.data);
				}
			}

		}

		if (req.table_name == "users") {
			if (req.type == "select") {
				if (req.key == 0)
					answer = get_all_users();
			}
			if (req.type == "insert") {
				answer = ins_users(req.data);
			}
			if (req.type == "update") {
				answer = upd_users(req.data);
			}
			if (req.type == "delete") {
				answer = del_users(req.data);
			}

		}

		if (req.table_name == "history") {
			if (req.type == "select") {
				if (req.key == 0) {
					answer = get_all_history();
				}
				if (req.key == 1) {
					answer = get_history_user(req.data);
				}
				if (req.key == 2) {
					answer = get_filtred(req.data);
				}
			}
			if (req.type == "insert") {
				answer = ins_history(req.data);
			}
		}

		if (req.table_name == "close") {
			active.erase(AContext->Binding->PeerIP);
			loadData();
			answer = simple_ans().ToJson();
		}
		loadData();
		if (answer == "") {
			simple_ans ans;
			ans.result = "Command error!";
			send_mes(ans.ToJson(), AContext);
		}
		else
			send_mes(answer, AContext);
	}
	catch (...) {
		simple_ans ans;
		ans.result = "Decrypt error!";
		AContext->Connection->IOHandler->WriteLn(ans.ToJson(), enUTF8);
		AContext->Connection->Disconnect();
		return;
	}
	AContext->Connection->Disconnect();
	loadData();
}
// ---------------------------------------------------------------------------

String TForm1::log(String data, String ip) {
	login n_w(data);
	UnicodeString tr = "select * from audit where userName = '" + n_w.log +
		"' and pash = '" + n_w.hash + "'";
	simple_ans ans;
	FDQuery1->SQL->Text = tr;
	Memo1->Lines->Add(FDQuery1->SQL->Text);
	FDQuery1->Active = 1;
	FDQuery1->First();
	if (!FDQuery1->Eof) {
		User u;
		u.name = FDQuery1->FieldByName("userName")->Value;
		u.access = FDQuery1->FieldByName("privilegies")->Value;
		ans.result = u.ToJson();
		ans.key = 1;
		active[ip] = FDQuery1->FieldByName("privilegies")->Value;
	}
	else {
		ans.result = "Login error!";
		ans.key = -1;
	}

	FDQuery1->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_files() {
	ans_Files ans;
	FDQuery3->SQL->Text = "select id,fileName,date from files";
	Memo1->Lines->Add(FDQuery3->SQL->Text);
	FDQuery3->Active = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		File f;
		f.name = FDQuery3->FieldByName("fileName")->Value;
		f.id = StrToInt(FDQuery3->FieldByName("id")->Value);
		f.date = FDQuery3->FieldByName("date")->Value;
		ans.arr.push_back(f);
	}
	FDQuery3->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_users() {
	ans_Users ans;
	FDQuery2->SQL->Text = "select * from audit";
	FDQuery2->Active = 1;
	Memo1->Lines->Add(FDQuery2->SQL->Text);
	for (FDQuery2->First(); !FDQuery2->Eof; FDQuery2->Next()) {
		User x;
		x.id = StrToInt(FDQuery2->FieldByName("id")->Value);
		x.name = FDQuery2->FieldByName("userName")->Value;
		x.pash = FDQuery2->FieldByName("pash")->Value;
		x.access = FDQuery2->FieldByName("privilegies")->Value;
		ans.arr.push_back(x);
	}
	FDQuery2->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_history() {
	ans_History ans;
	FDQuery4->SQL->Text = "select * from history";
	FDQuery4->Active = 1;
	Memo1->Lines->Add(FDQuery4->SQL->Text);
	for (FDQuery4->First(); !FDQuery4->Eof; FDQuery4->Next()) {
		History x;
		x.id = StrToInt(FDQuery4->FieldByName("id")->Value);
		x.fileName = FDQuery4->FieldByName("fileName")->Value;
		x.userName = FDQuery4->FieldByName("userName")->Value;
		x.datetime = FDQuery4->FieldByName("date")->Value;
		ans.arr.push_back(x);
	}
	FDQuery4->Active = 0;
	return ans.ToJson();
};

String TForm1::ins_users(String data) {
	simple_ans ans;
	try {
		User user(data);
		FDQuery5->SQL->Text = "insert into audit values (null,'" + user.name +
			"','" + user.pash + "','" + user.access + "')";
		Memo1->Lines->Add(FDQuery5->SQL->Text);
		FDQuery5->ExecSQL();
		ans.result = "Успешно добавлено!";
	}
	catch (...) {
		ans.result = "Произошла ошибка!";
	}
	return ans.ToJson();
};

String TForm1::ins_files(String data) {
	simple_ans ans;
	try {
		File file(data);
		FDQuery6->SQL->Text =
			"insert into files (fileName,source) values (:fN,:s)";
		FDQuery6->ParamByName("fN")->AsString = file.name;
		FDQuery6->ParamByName("s")->AsString = file.source;
		Memo1->Lines->Add(FDQuery6->SQL->Text);
		FDQuery6->ExecSQL();
		ans.result = "Успешно добавлено!";
	}
	catch (...) {
		ans.result = "Произошла ошибка!";
	}
	return ans.ToJson();
};

String TForm1::get_source_file(String data) {
	simple_ans ans;
	try {
		FDQuery7->SQL->Text = "select * from files where id = " + data;
		Memo1->Lines->Add(FDQuery7->SQL->Text);
		FDQuery7->Active = 1;
		for (FDQuery7->First(); !FDQuery7->Eof; FDQuery7->Next()) {
			File file;
			file.source = FDQuery7->FieldByName("source")->Value;
			file.id = StrToInt(FDQuery7->FieldByName("id")->Value);
			file.name = FDQuery7->FieldByName("fileName")->Value;
			file.date = FDQuery7->FieldByName("date")->Value;
			ans.result = file.ToJson();
			break;
		}
		FDQuery7->Active = 0;
	}
	catch (...) {
		ans.result = "Произошла ошибка!";
	}
	return ans.ToJson();
};

String TForm1::upd_file(String data) {
	simple_ans ans;
	try {
		File file(data);
		FDQuery8->SQL->Text = "update files set source = :s where id = " +
			String(file.id);
		FDQuery8->ParamByName("s")->AsString = file.source;
		Memo1->Lines->Add(FDQuery8->SQL->Text);
		FDQuery8->ExecSQL();
		ans.result = "Успешно изменено!";
	}
	catch (...) {
		ans.result = "Произошла ошибка!";
	}
	return ans.ToJson();
};

String TForm1::del_files(String data) {
	simple_ans ans;
	try {
		ans_Files files(data);
		for (int i = 0; i < files.arr.size(); i++) {
			FDQuery9->SQL->Text = "delete from files where id = " +
				String(files.arr[i].id);
			Memo1->Lines->Add(FDQuery9->SQL->Text);
			FDQuery9->ExecSQL();
		}
		ans.result = "Успешно удалено!";
	}
	catch (...) {
		ans.result = "Произошла ошибка!";
	}
	return ans.ToJson();
};

String TForm1::upd_users(String data) {
	simple_ans ans;
	try {
		User user(data);
		FDQuery10->SQL->Text = "update audit set pash = '" + user.pash +
			"', privilegies = '" + user.access + "' where id = " +
			String(user.id);
		Memo1->Lines->Add(FDQuery10->SQL->Text);
		FDQuery10->ExecSQL();
		ans.result = "Успешно изменено!";
	}
	catch (...) {
		ans.result = "Произошла ошибка!";
	}
	return ans.ToJson();
};

String TForm1::del_users(String data) {
	simple_ans ans;
	try {
		User user(data);
		FDQuery11->SQL->Text = "delete from audit where id = " +
			String(user.id);
		Memo1->Lines->Add(FDQuery11->SQL->Text);
		FDQuery11->ExecSQL();
		ans.result = "Успешно удалено!";
	}
	catch (...) {
		ans.result = "Произошла ошибка!";
	}
	return ans.ToJson();
};

String TForm1::get_history_user(String data) {
	ans_History ans;
	FDQuery12->SQL->Text = "select * from history where userName='" +
		data + "'";
	FDQuery12->Active = 1;
	Memo1->Lines->Add(FDQuery12->SQL->Text);
	for (FDQuery12->First(); !FDQuery12->Eof; FDQuery12->Next()) {
		History x;
		x.id = StrToInt(FDQuery12->FieldByName("id")->Value);
		x.fileName = FDQuery12->FieldByName("fileName")->Value;
		x.userName = FDQuery12->FieldByName("userName")->Value;
		x.datetime = FDQuery12->FieldByName("date")->Value;
		ans.arr.push_back(x);
	}
	FDQuery12->Active = 0;
	return ans.ToJson();
};

String TForm1::ins_history(String data) {
	simple_ans ans;
	try {
		History h(data);
		FDQuery13->SQL->Text =
			"insert into history (fileName,userName) values ('" + h.fileName +
			"','" + h.userName + "')";
		Memo1->Lines->Add(FDQuery13->SQL->Text);
		FDQuery13->ExecSQL();
		ans.result = "Успешно добавлено!";
	}
	catch (...) {
		ans.result = "Произошла ошибка!";
	}
	return ans.ToJson();
};

String TForm1::get_file_by_name(String data) {
	simple_ans ans;
	FDQuery14->SQL->Text = "select * from files where fileName='" + data + "'";
	FDQuery14->Active = 1;
	Memo1->Lines->Add(FDQuery14->SQL->Text);
	for (FDQuery14->First(); !FDQuery14->Eof; FDQuery14->Next()) {
		File x;
		x.id = StrToInt(FDQuery14->FieldByName("id")->Value);
		x.source = FDQuery14->FieldByName("source")->Value;
		x.name = FDQuery14->FieldByName("fileName")->Value;
		x.date = FDQuery14->FieldByName("date")->Value;
		ans.result = x.ToJson();
		break;
	}
	FDQuery14->Active = 0;
	return ans.ToJson();
};

String TForm1::get_filtred(String data) {
	sorting so(data);
	ans_History ans;
	int fl = 0;
	FDQuery15->SQL->Text = "select * from history ";
	if (so.date == 1) {
		fl = 1;
		FDQuery15->SQL->Text += "where date between '" + formatDate(so.date1) +
			" 00:00:00' and '" + formatDate(so.date2) + " 23:59:00' ";
	}
	if (so.data == 1) {
		if (fl == 0) {
			fl = 1;
			FDQuery15->SQL->Text += "where ";
		}
		else {
			FDQuery15->SQL->Text += " and ";
		}
		FDQuery15->SQL->Text += " fileName = '" + so.ff + "' ";
	}
	if (so.data1 == 1) {
		if (fl == 0) {
			fl = 1;
			FDQuery15->SQL->Text += "where ";
		}
		else {
			FDQuery15->SQL->Text += " and ";
		}
		FDQuery15->SQL->Text += " userName = '" + so.ss + "' ";

	}
	FDQuery15->Active = 1;
	Memo1->Lines->Add(FDQuery15->SQL->Text);
	for (FDQuery15->First(); !FDQuery15->Eof; FDQuery15->Next()) {
		History x;
		x.id = StrToInt(FDQuery15->FieldByName("id")->Value);
		x.fileName = FDQuery15->FieldByName("fileName")->Value;
		x.userName = FDQuery15->FieldByName("userName")->Value;
		x.datetime = FDQuery15->FieldByName("date")->Value;
		ans.arr.push_back(x);
	}
	FDQuery15->Active = 0;
	return ans.ToJson();

};

String TForm1::formatDate(String date) {
	return date.SubString(7, 4) + "-" + date.SubString(1, 2) + "-" +
		date.SubString(4, 2);
};

void TForm1::loadData() {
	clear();
	loadFiles();
	loadHistory();
	loadAgs();
}

void TForm1::clear() {
	ListBox1->Clear();
	for (int i = 1; i < StringGrid1->RowCount; i++) {
		for (int j = 0; j < StringGrid1->ColCount; j++) {
			StringGrid1->Cells[j][i] = "";
		}
	}
	StringGrid1->RowCount = 1;
	for (int i = 1; i < StringGrid2->RowCount; i++) {
		for (int j = 0; j < StringGrid2->ColCount; j++) {
			StringGrid2->Cells[j][i] = "";
		}
	}
	StringGrid2->RowCount = 1;
}

void TForm1::loadFiles() {
	FDQuery16->SQL->Text = "select fileName,date from files";
	try {
		FDQuery16->Active = 1;
		for (FDQuery16->First(); !FDQuery16->Eof; FDQuery16->Next()) {
			int k = StringGrid1->RowCount++;
			StringGrid1->Cells[0][k] = String(k);
			StringGrid1->Cells[1][k] =
				FDQuery16->FieldByName("fileName")->Value;
			StringGrid1->Cells[2][k] = FDQuery16->FieldByName("date")->Value;
		}
		FDQuery16->Active = 0;
	}
	catch (...) {
	}
}

void TForm1::loadHistory() {
	FDQuery16->SQL->Text = "select * from history";
	try {
		FDQuery16->Active = 1;
		for (FDQuery16->First(); !FDQuery16->Eof; FDQuery16->Next()) {
			int k = StringGrid2->RowCount++;
			StringGrid2->Cells[0][k] = String(k);
			StringGrid2->Cells[1][k] =
				FDQuery16->FieldByName("fileName")->Value;
			StringGrid2->Cells[2][k] =
				FDQuery16->FieldByName("userName")->Value;
			StringGrid2->Cells[3][k] = FDQuery16->FieldByName("date")->Value;
		}
		FDQuery16->Active = 0;
	}
	catch (...) {
	}
}

void TForm1::loadAgs() {
	for (auto x = active.begin(); x != active.end(); ++x) {
		ListBox1->Items->Add(x->second + " - " + x->first);
	}
}
