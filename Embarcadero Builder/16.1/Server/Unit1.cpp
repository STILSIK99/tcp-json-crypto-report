// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
#include <utilcls.h>
#include <utility>
#include <iterator>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Агент";
	StringGrid1->Cells[2][0] = "Имя файла";
	StringGrid1->Cells[3][0] = "Действие";
	StringGrid1->Cells[4][0] = "Время события";
	StringGrid1->ColWidths[0] = 100;
	StringGrid1->ColWidths[1] = 100;
	StringGrid1->ColWidths[2] = 350;
	StringGrid1->ColWidths[3] = 200;
	StringGrid1->ColWidths[4] = 200;

	if (!initDBConnection()) {
		Application->Terminate();
	}
	loadDB();
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
void TForm1::loadDB() {
	ComboBox3->Clear();
	FDQuery1->SQL->Text = "select * from agents";
	FDQuery1->Active = 1;
	for (FDQuery1->First(); !FDQuery1->Eof; FDQuery1->Next()) {
		ComboBox3->Items->Add(FDQuery1->FieldByName("agent")->Value);
	}
	FDQuery1->Active = 0;
};

void __fastcall TForm1::ServerExecute(TIdContext *AContext) {
	if (clients.count(AContext) == 0) {
		clients[AContext] = new CryptoSession("dirMon");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text += request;
		com req(request);

		Memo1->Lines->Add(AContext->Binding->PeerIP + " ---- " + req.ToJson());
		if (req.table_name == "test") {
			answer = add_agent(req.data, AContext->Binding->PeerIP);
		}
		if (req.table_name == "data") {
			if (req.type == "insert") {
				answer = ins_data(req.data, AContext->Binding->PeerIP);
				loadDB();
			}
		}

		if (req.table_name == "disconnect") {
			del_agent(AContext->Binding->PeerIP);
		}

		if (answer == "") {
			simple_ans ans;
			ans.result = "Command error!";
			send_mes(ans.ToJson(), AContext);
		}
		else
			send_mes(answer, AContext);
		AContext->Connection->Disconnect();
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
String TForm1::add_agent(String data, String ip) {
	if (simple_ans(data).key != 111) {
		return "";
	}
	agents[ip] = 0;
	loadAgs();
	FDQuery1->SQL->Text = "delete from motions where agent=:a";
	FDQuery1->ParamByName("a")->AsString = ip;
	FDQuery1->ExecSQL();

	FDQuery1->SQL->Text = "delete from agents where agent=:a";
	FDQuery1->ParamByName("a")->AsString = ip;
	FDQuery1->ExecSQL();

	FDQuery1->SQL->Text = "insert into agents values (null,:a)";
	FDQuery1->ParamByName("a")->AsString = ip;
	FDQuery1->ExecSQL();
	loadDB();

	return simple_ans("Есть соединение!", 111).ToJson();
};

void TForm1::del_agent(String ip) {
	agents.erase(ip);
	loadAgs();
};

void TForm1::loadAgs() {
	ListBox1->Clear();
	for (auto x : agents) {
		if (x.second == 0) {
			ListBox1->Items->Add(x.first + " - сбор данных выключен");
		}
		else
			ListBox1->Items->Add(x.first + " - сбор данных включен");
	}
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

String TForm1::ins_data(String data, String ip) {
	simple_ans reply;
	ans_history ans(data);
	try {
		FDQuery2->SQL->Text = "insert into motions values (null,:a,:f,:c,:t)";
		FDQuery2->ParamByName("a")->AsString = ip;
		for (auto x : ans.arr) {
			FDQuery2->ParamByName("f")->AsString = x.filename;
			FDQuery2->ParamByName("c")->AsString = x.change;
			FDQuery2->ParamByName("t")->AsString = x.time;
			FDQuery2->ExecSQL();
		}
	}
	catch (...) {
		reply.key = -1;
	}
	return reply.ToJson();
};

void __fastcall TForm1::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		curAg = "";
		return;
	}
	auto x = agents.begin();
	for (int i = 0; i < ListBox1->ItemIndex; ++i)
		++x;

	if (x->second == 0) {
		simple_ans req("turn_on", 0);
		try {
			Client1->Connect(x->first, 9999);
			if (!Client1->Connected()) {
				ShowMessage("Агент отключен!");
				del_agent(x->first);
			}
			else {
				Client1->IOHandler->WriteLn(req.ToJson());
				String ans;
				simple_ans answer(Client1->IOHandler->ReadLn());
				if (answer.result == "yes") {
					x->second = 1;
				}
				else
					ShowMessage("Сбор данных отклонён");
			}
		}
		catch (...) {
		}
	}
	else {
		simple_ans req("turn_off", 0);
		try {
			Client1->Connect(x->first, 9999);
			if (!Client1->Connected()) {
				ShowMessage("Агент отключен!");
				del_agent(x->first);
			}
			else {
				Client1->IOHandler->WriteLn(req.ToJson());
				x->second = 0;
			}
		}
		catch (...) {
		}

	}
	Client1->Disconnect();
	loadAgs();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox3Change(TObject * Sender) {
	clearGrid();
	if (ComboBox3->ItemIndex == -1) {
		return;
	}
	FDQuery2->SQL->Text = "select * from motions where agent=:a";
	FDQuery2->ParamByName("a")->AsString = ComboBox3->Text;
	if (f1 == 1) {
		FDQuery2->SQL->Text += " and change=:c";
		FDQuery2->ParamByName("c")->AsString = filter1;

	}
	if (f2 == 1) {
		FDQuery2->SQL->Text += " and time > :t";
		FDQuery2->ParamByName("t")->AsString = filter2;
	}
	FDQuery2->Active = 1;
	for (FDQuery2->First(); !FDQuery2->Eof; FDQuery2->Next()) {
		int k = StringGrid1->RowCount++;
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = FDQuery2->FieldByName("agent")->Value;
		StringGrid1->Cells[2][k] = FDQuery2->FieldByName("file")->Value;
		StringGrid1->Cells[3][k] = FDQuery2->FieldByName("change")->Value;
		StringGrid1->Cells[4][k] = FDQuery2->FieldByName("time")->Value;
	}

	FDQuery2->Active = 0;
}

// ---------------------------------------------------------------------------
void TForm1::clearGrid() {
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	StringGrid1->RowCount = 1;
}

void __fastcall TForm1::Button2Click(TObject * Sender) {
	if (CheckBox1->Checked == 1) {
		if (ComboBox1->ItemIndex == -1) {
			ShowMessage("Установите параметры");
			return;
		}
		f1 = 1;
		filter1 = ComboBox1->Text;
	}
	else {
		f1 = 0;
		filter1 = "";
	}
	if (CheckBox2->Checked == 1) {
		f2 = 1;
		TDateTime current(Now());
		current -= StrToTime("03:00:00");
		switch (ComboBox2->ItemIndex) {
		case 0:
			current -= StrToTime("00:05:00");
			break;
		case 1:
			current -= StrToTime("00:15:00");
			break;
		case 2:
			current -= StrToTime("00:30:00");
			break;
		case 3:
			current -= StrToTime("01:00:00");
			break;
		case 4:
			current -= StrToTime("12:00:00");
			break;

		default: ;
		}
		filter2 = current.FormatString("yyyy-mm-dd hh:mm:ss");
	}
	else {
		f2 = 0;
		filter2 = "";
	}
	clearGrid();
	if (ComboBox3->ItemIndex == -1) {
		return;
	}
	FDQuery2->SQL->Text = "select * from motions where agent=:a";
	FDQuery2->ParamByName("a")->AsString = ComboBox3->Text;
	if (f1 == 1) {
		FDQuery2->SQL->Text += " and change=:c";
		FDQuery2->ParamByName("c")->AsString = filter1;

	}
	if (f2 == 1) {
		FDQuery2->SQL->Text += " and time > :t";
		FDQuery2->ParamByName("t")->AsString = filter2;
	}
	Memo1->Text += FDQuery2->SQL->Text;
	FDQuery2->Active = 1;
	for (FDQuery2->First(); !FDQuery2->Eof; FDQuery2->Next()) {
		int k = StringGrid1->RowCount++;
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = FDQuery2->FieldByName("agent")->Value;
		StringGrid1->Cells[2][k] = FDQuery2->FieldByName("file")->Value;
		StringGrid1->Cells[3][k] = FDQuery2->FieldByName("change")->Value;
		StringGrid1->Cells[4][k] = FDQuery2->FieldByName("time")->Value;
	}

	FDQuery2->Active = 0;
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
	int item = 1;
	vVarParagraphs = vVarDoc.OlePropertyGet("Paragraphs");

	for (int i = 1; i < StringGrid1->RowCount; i++) {
		String s = String(i) + ".\t " + StringGrid1->Cells[1][i] + "\\\\" +
			StringGrid1->Cells[2][i] + " - " + StringGrid1->Cells[3][i] + " :" +
			StringGrid1->Cells[4][i];
		vVarParagraphs.OleProcedure("Add");
		vVarParagraph = vVarParagraphs.OleFunction("Item", item++);
		vVarParagraph.OlePropertyGet("Range").OlePropertySet("Text",
			WideString(s.c_str()));
	}

	AnsiString vAsCurDir = GetCurrentDir();
	AnsiString vAsCurDir1 = vAsCurDir + "\\test.doc";
	vVarDoc = vVarDocs.OleFunction("Item", 1);
	vVarDoc.OleProcedure("SaveAs", WideString(vAsCurDir1.c_str()));
	vVarApp.OleProcedure("Quit");
	// ShellExecute(NULL, L"open", vAsCurDir1.c_str(), NULL, NULL, SW_SHOWNORMAL);
	system("start test.doc");
}
// ---------------------------------------------------------------------------
