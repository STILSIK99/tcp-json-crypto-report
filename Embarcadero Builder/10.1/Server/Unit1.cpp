// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <utilcls.h>
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
#include "Unit1.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	if (initDBConnection()) {
		ShowMessage("Connected!");
	}

	curAg = "";
	curDisk = "";
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

// ---------------------------------------------------------------------------

void __fastcall TForm1::ServerExecute(TIdContext *AContext) {
	if (clients.count(AContext) == 0) {
		clients[AContext] = new CryptoSession("monitor");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text = request;
		com req(request);

		Memo1->Lines->Add(AContext->Binding->PeerIP + " ---- " + req.ToJson());
		if (req.table_name == "login" && req.type == "select") {
			answer = log(req.data, AContext->Binding->PeerIP);
		}
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 1).ToJson();
		}

		if (req.table_name == "disk") {
			if (req.type == "insert") {
				answer = ins_data(req.data, AContext->Binding->PeerIP);
			}
		}

		if (req.table_name == "close") {
			disconnect(AContext->Binding->PeerIP);
			AContext->Connection->Disconnect();
			return;
		}

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
	}
	AContext->Connection->Disconnect();
}
// ---------------------------------------------------------------------------

String TForm1::log(String data, String ip) {
	login user(data);
	simple_ans ans;
	// FDQuery1->SQL->Text = "INSERT INTO login values (:log,:pasw)";
	FDQuery1->SQL->Text =
		"select * from login where login=:log and pass =:pasw";
	FDQuery1->ParamByName("log")->AsString = user.log;
	FDQuery1->ParamByName("pasw")->AsString = user.hash;
	// FDQuery1->ExecSQL();
	FDQuery1->Active = 1;
	FDQuery1->First();
	if (!FDQuery1->Eof) {
		ans.key = 1;
		agent n;
		agsData[ip] = n;
		FDQuery2->SQL->Text = "delete from disk where ip=:ip";
		FDQuery2->ParamByName("ip")->AsString = ip;
		FDQuery2->ExecSQL();
		FDQuery2->SQL->Text = "delete from otchet where ip=:ip";
		FDQuery2->ParamByName("ip")->AsString = ip;
		FDQuery2->ExecSQL();
		loadData(1);
	}
	else
		ans.result = "Неверный логин или пароль!";

	return ans.ToJson();
};

String TForm1::ins_data(String data, String ip) {
	otchet disks(data);
	if (agsData.count(ip) == 0) {
		agent s;
		agsData[ip] = s;
	}
	for (auto x : disks.arr) {

		try {

			FDQuery3->SQL->Text =
				"insert into disk (ip,total,occup,name) values (:i,:t,:o,:n)";
			FDQuery3->ParamByName("i")->AsString = ip;
			FDQuery3->ParamByName("t")->AsFloat = x.total;
			FDQuery3->ParamByName("o")->AsFloat = x.occup;
			FDQuery3->ParamByName("n")->AsString = x.name;
			FDQuery3->ExecSQL();
			if (agsData[ip].porog.count(x.name) != 0) {
				if (agsData[ip].porog[x.name] < x.occup) {
					try {
						FDQuery8->SQL->Text =
							"delete from otchet where ip=:i and disk=:d";
						FDQuery8->ParamByName("i")->AsString = ip;
						FDQuery8->ParamByName("d")->AsString = x.name;
						FDQuery8->ExecSQL();
						FDQuery5->SQL->Text =
							"insert into otchet values (:i,:d,:o)";
						FDQuery5->ParamByName("i")->AsString = ip;
						FDQuery5->ParamByName("d")->AsString = x.name;
						FDQuery5->ParamByName("o")->AsFloat =
							agsData[ip].porog[x.name];
						FDQuery5->ExecSQL();
						loadOtchet();
					}
					catch (...) {
					}
				}
			}
			if (curAg == ip) {
				get_info(curAg, curDisk);
			}
		}
		catch (...) {
		}
	}
	loadData(0);

};

void TForm1::loadAgs(int a) {
	ListBox1->Clear();
	if (a) {
		ComboBox1->Clear();
	}
	for (auto x = agsData.begin(); x != agsData.end(); x++) {
		if (x->second.turn)
			ListBox1->Items->Add(x->first + " - мониторинг включен");
		else
			ListBox1->Items->Add(x->first + " - мониторинг выключен");
		if (a) {
			ComboBox1->Items->Add(x->first);
		}

	}
};

void __fastcall TForm1::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		return;
	}
	int k = ListBox1->ItemIndex;
	auto x = agsData.begin();
	for (; x != agsData.end() && k-- > 0; x++) {

	}
	if (x->second.turn == 0) {

		if (Application->MessageBox(_T("Включить сбор данных?"), _T(""),
			MB_YESNO + MB_TOPMOST + MB_ICONQUESTION) == IDYES) {
			simple_ans a("turn_on", 0);
			try {

				Client1->Connect(x->first, 48888);
				if (!Client1->Connected()) {
					ShowMessage("Соединение отсутствует!");
					agsData.erase(x->first);
					curAg = "";
					ComboBox2->Clear();
					loadData(1);
				}
				else {
					Client1->IOHandler->WriteLn(a.ToJson());
					simple_ans answer(Client1->IOHandler->ReadLn());
					if (answer.result == "yes") {
						x->second.turn = 1;
						loadData(1);
					}
					else
						ShowMessage("Агент отклонил команду!");
				}

			}
			catch (...) {
			}
			Client1->Disconnect();
		}
	}
	else {
		if (Application->MessageBox(_T("Выключить сбор данных?"), _T(""),
			MB_YESNO + MB_TOPMOST + MB_ICONQUESTION) == IDYES) {
			simple_ans a("turn_off", 0);
			try {
				Client1->Connect(x->first, 48888);
				if (!Client1->Connected()) {
					ShowMessage("Соединение отсутствует!");
					agsData.erase(x->first);
					curAg = "";
					ComboBox2->Clear();
					loadData(1);
				}
				else {
					Client1->IOHandler->WriteLn(a.ToJson());
					x->second.turn = 0;
					loadData(1);
				}

			}
			catch (...) {
			}
			Client1->Disconnect();
		}

	}
}

// ---------------------------------------------------------------------------
void TForm1::loadData(int a) {
	ListBox1->Clear();
	loadAgs(a);
	if (curAg != "" && curDisk != "") {
		get_info(curAg, curDisk);
	}

};

void __fastcall TForm1::ComboBox1Change(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	curAg = ComboBox1->Text;
	ComboBox2->Clear();
	auto x = agsData[ComboBox1->Text];
	FDQuery7->SQL->Text = "SELECT DISTINCT name from disk where ip=:i";
	FDQuery7->ParamByName("i")->AsString = curAg;
	FDQuery7->Active = 1;
	for (FDQuery7->First(); !FDQuery7->Eof; FDQuery7->Next()) {
		ComboBox2->Items->Add(FDQuery7->FieldByName("name")->Value);
	}
	FDQuery7->Active = 0;
}

// ---------------------------------------------------------------------------
void TForm1::disconnect(String ip) {
	if (agsData.count(ip) == 0) {
		return;
	}
	agsData.erase(ip);
	if (curAg == ip) {
		curAg = "";
		ComboBox2->Clear();
	}
	loadData(1);

};

void TForm1::get_info(String ag, String disk) {
	Chart1->Title->Text->Clear();
	Series1->Clear();
	Series2->Clear();
	Series4->Clear();
	try {
		FDQuery4->SQL->Text =
			"select * from disk where ip=:a and name=:d order by id desc limit 30";
		FDQuery4->ParamByName("a")->AsString = ag;
		FDQuery4->ParamByName("d")->AsString = disk;
		FDQuery4->Active = 1;
		int i = 0;
		for (FDQuery4->First(); !FDQuery4->Eof; FDQuery4->Next()) {
			double t = FDQuery4->FieldByName("total")->Value;
			double o = FDQuery4->FieldByName("occup")->Value;
			Series1->AddXY(i++, t);
			Series2->AddXY(i++, o);
			if (agsData.count(ag) != 0) {
				if (agsData[ag].porog.count(disk) != 0) {
					Series4->AddXY(i, agsData[ag].porog[disk]);
				}
			}
		}
		if (i > 0) {
			String tit = ag + " - " + disk;
			Chart1->Title->Text->Add(tit);
		}
		FDQuery4->Active = 0;
	}
	catch (...) {
	}
};

void TForm1::loadOtchet() {
	Memo2->Clear();
	try {
		FDQuery6->SQL->Text = "select * from otchet";
		FDQuery6->Active = 1;
		for (FDQuery6->First(); !FDQuery6->Eof; FDQuery6->Next()) {
			String line = FDQuery6->FieldByName("ip")->Value;
			line += " - ";
			line += FDQuery6->FieldByName("disk")->Value;
			line += " - превышено ограничение: ";
			line += FDQuery6->FieldByName("ogr")->Value;
			Memo2->Lines->Add(line);
		}
		FDQuery6->Active = 0;
	}
	catch (...) {
	}
};

void __fastcall TForm1::ComboBox2Change(TObject *Sender) {
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	if (curAg == "") {
		ComboBox2->Clear();
		return;
	}
	curDisk = ComboBox2->Text;
	get_info(curAg, curDisk);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {

	if (ComboBox1->ItemIndex == -1)
		return;
	if (ComboBox2->ItemIndex == -1)
		return;
	double x;
	try {
		x = LabeledEdit1->Text.ToDouble();
		if (agsData.count(curAg) == 0) {
			agent s;
			agsData[curAg] = s;
		}
		agsData[curAg].porog[curDisk] = x;
		ShowMessage("Ограничение добавлено");
		LabeledEdit1->Text = "";
	}
	catch (...) {
		ShowMessage("Неверные данные");
		return;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender) {
	Variant v, v1, vVarCell, vVarTable, vVarRange, vVarParagraph, vVarApp,
		vVarDocs, vVarDoc, vVarParagraphs;
	try {
		vVarApp = CreateOleObject("Word.Application");
	}
	catch (...) {
		ShowMessage("Ошибка");
		return;
	}

	vVarApp.OlePropertySet("Visible", false);
	vVarDocs = vVarApp.OlePropertyGet("Documents");
	vVarDocs.OleProcedure("Add");
	if (vVarDocs.OlePropertyGet("Count") != 1) {
		ShowMessage("Ошибка");
		return;
	}

	vVarDoc = vVarDocs.OleFunction("Item", 1);
	vVarDoc.OleProcedure("Activate");

	vVarParagraphs = vVarDoc.OlePropertyGet("Paragraphs");
	vVarParagraphs.OleProcedure("Add");
	vVarParagraph = vVarParagraphs.OleFunction("Item", 1);
	vVarParagraph.OlePropertyGet("Range").OlePropertySet("Text",
		WideString(Memo2->Text.c_str()));
	vVarParagraph.OlePropertySet("Alignment", 3);
	vVarParagraphs.OleProcedure("Add");

	AnsiString vAsCurDir = GetCurrentDir();
	AnsiString vAsCurDir1 = vAsCurDir + "\\test.doc";
	vVarDoc = vVarDocs.OleFunction("Item", 1);
	vVarDoc.OleProcedure("SaveAs", WideString(vAsCurDir1.c_str()));
	vVarApp.OleProcedure("Quit");
	// ShellExecute(NULL, L"open", vAsCurDir1.c_str(), NULL, NULL, SW_SHOWNORMAL);
	system("start test.doc");
}
// ---------------------------------------------------------------------------
