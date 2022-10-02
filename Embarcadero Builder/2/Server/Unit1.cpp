// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
#include <utilcls.h>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	curProc = -1;
	curAg = "";

	if (!initDBConnection()) {
		Application->Terminate();
	}
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
		clients[AContext] = new CryptoSession("process");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text += request;
		com req(request);

		Memo1->Text += AContext->Binding->PeerIP + " ---- " + req.ToJson();
		// if (req.table_name == "users" && req.type == "select") {
		// answer = log(req.data);
		// }
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 123).ToJson();
			agents[AContext->Binding->PeerIP] = 0;
			loadAgs();
			del_old(AContext->Binding->PeerIP);
		}
		if (req.table_name == "data") {
			if (req.type == "insert") {
				answer = ins_data(req.data, AContext->Binding->PeerIP);
			}
		}
		if (req.type == "close") {
			disconnect(AContext->Binding->PeerIP);
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

	}
	Memo1->Text += answer;
	AContext->Connection->Disconnect();
}
// ---------------------------------------------------------------------------

String TForm1::ins_data(String data, String ip) {
	ans_Proc ans(data);
	simple_ans reply;
	for (int i = 0; i < ans.arr.size(); i++) {
		FDQuery2->SQL->Text =
			"select id from procList where agent=:a and procName=:p";
		FDQuery2->ParamByName("a")->AsString = ip;
		FDQuery2->ParamByName("p")->AsString = ans.arr[i].name;
		FDQuery2->Active = 1;
		FDQuery2->First();
		int x = -1;
		if (!FDQuery2->Eof) {
			x = StrToInt(FDQuery2->FieldByName("id")->Value);
			FDQuery2->Active = 0;
		}
		else {
			FDQuery2->Active = 0;
			FDQuery2->SQL->Text = "insert into procList values(null,:a,:p)";
			FDQuery2->ParamByName("a")->AsString = ip;
			FDQuery2->ParamByName("p")->AsString = ans.arr[i].name;
			FDQuery2->ExecSQL();
			FDQuery2->SQL->Text =
				"select id from procList where agent=:a and procName=:p";
			FDQuery2->ParamByName("a")->AsString = ip;
			FDQuery2->ParamByName("p")->AsString = ans.arr[i].name;
			FDQuery2->Active = 1;
			FDQuery2->First();
			if (!FDQuery2->Eof) {
				x = StrToInt(FDQuery2->FieldByName("id")->Value);
				FDQuery2->Active = 0;
			}
		}
		// x - id процесса
		try {
			if (ogrs.count(ip + " - " + ans.arr[i].name) != 0) {
				if (ogrs[ip + " - " + ans.arr[i].name] <
					ans.arr[i].memory.ToInt()) {
					Memo2->Lines->Add(Now().TimeString() + ": " + ip + " - " +
						ans.arr[i].name + " превышено ограничение памяти на " +
						String(ans.arr[i].memory.ToInt() -
						ogrs[ip + " - " + ans.arr[i].name]) + " КБайт");
				}
			}
			FDQuery2->SQL->Text = "insert into memoryInfo values (:i,:m,:t)";
			FDQuery2->ParamByName("i")->AsInteger = x;
			FDQuery2->ParamByName("m")->AsString = ans.arr[i].memory;
			FDQuery2->ParamByName("t")->AsString = ans.time;
			FDQuery2->ExecSQL();
		}
		catch (...) {
			reply.key = -1;
		}
	}
	return reply.ToJson();
};

String TForm1::log(String data) {
	login n_w(data);
	simple_ans ans;
	FDQuery1->SQL->Text =
		"select count(*) m from users where pass=:p and login=:u";
	FDQuery1->ParamByName("p")->AsString = n_w.hash;
	FDQuery1->ParamByName("u")->AsString = n_w.log;
	FDQuery1->Active = 1;
	FDQuery1->First();
	if (StrToInt(FDQuery1->FieldByName("m")->Value)) {
		ans.key = 1;
	}
	else
		ans.result = "Login error!";
	FDQuery1->Active = 0;
	return ans.ToJson();
};

void __fastcall TForm1::ComboBox1Change(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	curProc = procCom[ComboBox1->ItemIndex];
	Series1->Clear();

	FDQuery4->SQL->Text = "select * from memoryInfo where id=:i";
	FDQuery4->ParamByName("i")->AsInteger = curProc;
	FDQuery4->Active = 1;
	int i = 0;
	for (FDQuery4->First(); !FDQuery4->Eof; FDQuery4->Next()) {
		Series1->AddXY(i++, StrToInt(FDQuery4->FieldByName("mem")->Value));
	}
	FDQuery4->Active = 0;

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox2Change(TObject *Sender) {
	if (ComboBox2->ItemIndex == -1) {
		ComboBox1->Clear();
		curProc = -1;
		curAg = "";
		return;
	}
	loadProc(1);
}

// ---------------------------------------------------------------------------

void TForm1::disconnect(String ip) {
	if (agents.count(ip) != 0) {
		agents.erase(ip);
	}
	loadAgs();
}

void TForm1::loadAgs() {
	ListBox1->Clear();
	ListBox2->Clear();
	ComboBox2->Clear();
	curAg = "";
	for (auto x : agents) {
		ComboBox2->Items->Add(x.first);
		if (x.second == 0) {
			ListBox1->Items->Add(x.first + " - сбор данных выключен");
		}
		else
			ListBox1->Items->Add(x.first + " - сбор данных включен");
	}
	ComboBox3->Items = ComboBox2->Items;
};

void TForm1::loadProc(int a) {
	if (a == 0) {
		procList.clear();
		ListBox2->Clear();
	}
	if (a == 1) {
		procCom.clear();
		ComboBox1->Clear();
	}
	if (a == 2) {
		procOgr.clear();
		ComboBox4->Clear();
	}

	try {
		FDQuery3->SQL->Text = "select * from procList where agent=:i";
		if (a == 0 || a == 1) {
			FDQuery3->ParamByName("i")->AsString = curAg;
		}
		if (a == 2) {
			FDQuery3->ParamByName("i")->AsString = ComboBox3->Text;
		}
		FDQuery3->Active = 1;
		for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
			if (a == 0) {
				procList.push_back(FDQuery3->FieldByName("id")->Value);
				ListBox2->Items->Add(FDQuery3->FieldByName("procName")->Value);
			}
			if (a == 1) {
				procCom.push_back(FDQuery3->FieldByName("id")->Value);
				ComboBox1->Items->Add(FDQuery3->FieldByName("procName")->Value);
			}
			if (a == 2) {
				procOgr.push_back(FDQuery3->FieldByName("id")->Value);
				ComboBox4->Items->Add(FDQuery3->FieldByName("procName")->Value);
			}

		}
		FDQuery3->Active = 0;
	}
	catch (...) {
	}
};

void __fastcall TForm1::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		return;
	}
	curAg = ComboBox2->Items->Strings[ListBox1->ItemIndex];
	Label1->Caption = "Агент - " + curAg;
	loadProc(0);
}

void __fastcall TForm1::Button2Click(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		return;
	}
	String ag = ComboBox2->Items->Strings[ListBox1->ItemIndex];
	if (agents[ag] == 0) {
		simple_ans req("turn_on", 0);
		try {
			Client1->Connect(ag, 21212);
			if (!Client1->Connected()) {
				ShowMessage("Агент отключен!");
				disconnect(ag);
			}
			else {
				Client1->IOHandler->WriteLn(req.ToJson());
				String ans;
				simple_ans answer(Client1->IOHandler->ReadLn());
				if (answer.result == "yes") {
					agents[ag] = 1;
				}
				else {
					agents[ag] = 0;
					ShowMessage("Мониторинг отменен агентом");
				}
			}
		}
		catch (...) {
		}
		Client1->Disconnect();
	}
	else {
		simple_ans req("turn_off", 0);
		Client1->Disconnect();
	}

	loadAgs();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
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

	String otch = "";
	for (int i = 0; i < Memo2->Lines->Count; i++) {
		String a = Memo2->Lines->Strings[i];
		if (a == "") {
			continue;
		}
		otch += "\r\n";
		otch += String(i + 1) + ".";
		otch += "\t";
		otch += a;
	}

	vVarParagraphs = vVarDoc.OlePropertyGet("Paragraphs");
	vVarParagraphs.OleProcedure("Add");
	vVarParagraph = vVarParagraphs.OleFunction("Item", 1);
	vVarParagraph.OlePropertyGet("Range").OlePropertySet("Text",
		WideString(otch.c_str()));

	AnsiString vAsCurDir = GetCurrentDir();
	vAsCurDir = "\"" + vAsCurDir;
	AnsiString vAsCurDir1 = vAsCurDir + "\\test.doc\"";
	vVarDoc = vVarDocs.OleFunction("Item", 1);
	vVarDoc.OleProcedure("SaveAs", WideString(vAsCurDir1.c_str()));
	vVarApp.OleProcedure("Quit");
	// ShellExecute(NULL, L"open", vAsCurDir1.c_str(), NULL, NULL, SW_SHOWNORMAL);
	system(AnsiString(vAsCurDir1).c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox3Change(TObject *Sender) {
	if (ComboBox3->ItemIndex == -1) {
		ComboBox4->Clear();
		return;
	}
	loadProc(2);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox4Change(TObject *Sender) {
	if (ogrs.count(ComboBox3->Text + " - " + ComboBox4->Text) == 0) {
		LabeledEdit1->Text = "0";
	}
	else
		LabeledEdit1->Text =
			String(ogrs[ComboBox3->Text + " - " + ComboBox4->Text]);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	if (ComboBox3->ItemIndex == -1) {
		return;
	}
	if (ComboBox4->ItemIndex == -1) {
		return;
	}
	int ogr = 0;
	try {
		ogr = LabeledEdit1->Text.ToInt();
	}
	catch (...) {
		ShowMessage("Некорректные данные");
		return;
	}
	ogrs[ComboBox3->Text + " - " + ComboBox4->Text] = ogr;
	ShowMessage("Ограничение установлено");
	ComboBox4->Clear();
	ComboBox3->ItemIndex = -1;
}

// ---------------------------------------------------------------------------
void TForm1::del_old(String ip) {
	FDQuery6->SQL->Text =
		"delete from memoryInfo where id in (select id from procList where agent=:i)";
	FDQuery6->ParamByName("i")->AsString = ip;
	FDQuery6->ExecSQL();
	FDQuery6->SQL->Text = "delete from procList where agent=:i";
	FDQuery6->ParamByName("i")->AsString = ip;
	FDQuery6->ExecSQL();
}
