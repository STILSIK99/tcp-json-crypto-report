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
	// FDConnection1->Connected = 1;

	if (initDBConnection()) {
		ShowMessage("Connected!");
	}
	FDQuery6->Active = 1;
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
		clients[AContext] = new CryptoSession("driver");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text += request;
		com req(request);

		Memo1->Lines->Add(AContext->Binding->PeerIP + " ---- " + req.ToJson());
		if (req.table_name == "autoriz" && req.type == "select") {
			answer = log(req.data, AContext->Binding->PeerIP);
		}
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 1).ToJson();
		}

		if (req.table_name == "etalon") {
			if (req.type == "select") {
				if (req.key == 0) {
					answer = get_all_etalon();
				}
			}
			if (req.type == "insert") {
				if (req.key == 0) {
					answer = ins_etalon(req.data);
				}
			}
		}
		if (req.table_name == "driv") {
			if (req.type == "select") {
				if (req.key == 1) {
					answer = get_sr_znach();
				}
				if (req.key == 2) {
					answer = get_max_driv();
				}
			}
			if (req.type == "insert") {
				if (req.key == 0) {
					answer = ins_driv(req.data, AContext->Binding->PeerIP);
				}
			}
		}
		if (req.table_name == "close") {
			del_connection(AContext->Binding->PeerIP);
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

String TForm1::log(String data, String ip) {
	login n_w(data);
	simple_ans ans;
	FDQuery1->SQL->Text =
		"select * from autoriz where name=:log and hash=:pass";
	FDQuery1->ParamByName("log")->AsString = n_w.log;
	FDQuery1->ParamByName("pass")->AsString = n_w.hash;
	FDQuery1->Active = 1;
	FDQuery1->First();
	if (!FDQuery1->Eof) {
		ans.key = 1;
		agents[ip] = 0;
		reload();
	}
	else
		ans.result = "Login error!";
	FDQuery1->Active = 0;
	return ans.ToJson();
};

String TForm1::get_all_etalon() {
	listDriv ans;
	FDQuery2->SQL->Text = "select * from etalon";
	FDQuery2->Active = 1;
	for (FDQuery2->First(); !FDQuery2->Eof; FDQuery2->Next()) {
		driv x;
		x.dname = FDQuery2->FieldByName("name")->Value;
		x.id = StrToInt(FDQuery2->FieldByName("id")->Value);
		ans.arr.push_back(x);
	}
	FDQuery2->Active = 0;
	return ans.ToJson();
};

String TForm1::ins_etalon(String data) {
	simple_ans ans;
	listDriv dr(data);
	try {
		FDQuery4->SQL->Text = "delete from etalon";
		FDQuery4->ExecSQL();
		FDQuery3->SQL->Text = "insert into etalon (name) values (:name)";
		for (int i = 0; i < dr.arr.size(); i++) {
			FDQuery3->ParamByName("name")->AsString = dr.arr[i].dname;
			FDQuery3->ExecSQL();
		}
		ans.result = "Данные успешно внесены!";
	}
	catch (...) {
		ans.result = "Ошибка, данные не внесены!";
	}
	return ans.ToJson();
};

String TForm1::get_sr_znach() {
	simple_ans ans;
	int drivs = 0;
	int pcs = 0;
	FDQuery5->SQL->Text =
		"select count(*) kolvo,compName from driv group by compName";
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		drivs += StrToInt(FDQuery5->FieldByName("kolvo")->Value);
		pcs++;
	}
	if (pcs != 0) {
		ans.result = String(int(drivs / double(pcs) + 0.5));
	}
	else
		ans.result = "0";
	return ans.ToJson();
};

String TForm1::get_max_driv() {
	simple_ans ans;
	int max = 0;
	FDQuery5->SQL->Text = "select count(*) kolvo from driv group by compName";
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		if (StrToInt(FDQuery5->FieldByName("kolvo")->Value) > max)
			max = StrToInt(FDQuery5->FieldByName("kolvo")->Value);
	}
	ans.result = String(max);
	return ans.ToJson();
};

String TForm1::ins_driv(String data, String ip) {
	simple_ans ans;
	listDriv dr(data);
	if (dr.arr.size() == 0) {
		return simple_ans("Нет данных", 0).ToJson();
	}
	try {
		FDQuery4->SQL->Text = "delete from driv where compName=:comp";
		FDQuery4->ParamByName("comp")->AsString = ip;
		FDQuery4->ExecSQL();
		FDQuery3->SQL->Text =
			"insert into driv (name,compName) values (:name,:comp)";
		for (int i = 0; i < dr.arr.size(); i++) {
			FDQuery3->ParamByName("name")->AsString = dr.arr[i].dname;
			FDQuery3->ParamByName("comp")->AsString = ip;
			FDQuery3->ExecSQL();
		}
		ans.result = "Данные успешно внесены!";
	}
	catch (...) {
		ans.result = "Ошибка, данные не внесены!";
	}
	FDQuery6->Active = 0;
	FDQuery6->Active = 1;

	int sr = StrToInt(simple_ans(get_sr_znach()).result);
	int m = StrToInt(simple_ans(get_max_driv()).result);

	StatusBar1->Panels->Items[0]->Text =
		"Среднее кол-во установленных драйверов на ПК: " + String(sr);
	StatusBar1->Panels->Items[1]->Text =
		"Максимальное кол-во установленных драйверов на ПК: " + String(m);

	return ans.ToJson();
};

void TForm1::reload() {
	ComboBox1->Clear();
	ListBox1->Clear();
	for (auto it = agents.begin(); it != agents.end(); it++) {
		ComboBox1->Items->Add(it->first);
		if (it->second > 0) {
			ListBox1->Items->Add(it->first + " " + String(it->second));
		}
	}
	ComboBox2->Items = ComboBox1->Items;
};

void TForm1::del_connection(String a) {
	agents.erase(a);
	reload();
};

void __fastcall TForm1::Button2Click(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		ShowMessage("Выберите агента!");
		return;
	}
	int k;
	try {
		k = LabeledEdit1->Text.ToInt();
	}
	catch (...) {
		ShowMessage("Неверно задан период!");
		return;
	}
	simple_ans start("start", k);
	try {
		Client1->Connect(ComboBox1->Text, 4445);
		if (!Client1->Connected()) {
			ShowMessage("Соединение отсутствует!");
			agents.erase(ComboBox1->Text);
		}
		else {
			Client1->IOHandler->WriteLn(start.ToJson());
			String ans;
			simple_ans answer(Client1->IOHandler->ReadLn());
			if (answer.result == "ok") {
				agents[ComboBox1->Text] = k;
			}
			else
				ShowMessage("Агент отклонил команду!");
		}
	}
	catch (...) {
	}
	Client1->Disconnect();
	reload();

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		ShowMessage("Выберите агента!");
		return;
	}
	simple_ans stop("stop", 0);
	try {
		Client1->Connect(ComboBox1->Items->Strings[ListBox1->ItemIndex], 4445);
		if (!Client1->Connected()) {
			ShowMessage("Соединение отсутствует!");
			del_connection(ComboBox1->Items->Strings[ListBox1->ItemIndex]);
		}
		else {
			Client1->IOHandler->WriteLn(stop.ToJson());
			agents[ComboBox1->Items->Strings[ListBox1->ItemIndex]] = 0;
		}

	}
	catch (...) {
	}
	Client1->Disconnect();
	reload();

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender) {
	FDQuery7->Active = 0;
	FDQuery7->SQL->Text =
		"select id,name as 'Имя драйвера' from driv where compName=:comp";
	FDQuery7->ParamByName("comp")->AsString = ComboBox2->Text;
	FDQuery7->Active = 1;
}
// ---------------------------------------------------------------------------
