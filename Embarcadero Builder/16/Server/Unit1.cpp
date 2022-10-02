// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Excel_XP_srvr.h"
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
#include <utilcls.h>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	cur_mon = -1;
	cur_ag = "";

	StringGrid1->RowCount = 1;
	StringGrid2->RowCount = 1;

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Агент";
	StringGrid1->Cells[2][0] = "Директория мониторинга";
	StringGrid1->Cells[3][0] = "Время начала";
	StringGrid1->Cells[4][0] = "Время окончания";
	StringGrid1->ColWidths[0] = 40;
	StringGrid1->ColWidths[1] = 260;
	StringGrid1->ColWidths[2] = 300;
	StringGrid1->ColWidths[3] = 200;
	StringGrid1->ColWidths[4] = 200;

	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Имя файла";
	StringGrid2->Cells[2][0] = "Событие";
	StringGrid2->Cells[3][0] = "Время события";
	StringGrid2->ColWidths[0] = 70;
	StringGrid2->ColWidths[1] = 150;
	StringGrid2->ColWidths[2] = 100;
	StringGrid2->ColWidths[3] = 300;
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
		clients[AContext] = new CryptoSession("acts");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text = request;
		com req(request);

		Memo1->Lines->Add(AContext->Binding->PeerIP + " ---- " + req.ToJson());
		if (req.table_name == "users" && req.type == "select") {
			answer = log(req.data);
		}
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 1).ToJson();
		}
		if (req.table_name == "monitor") {
			if (req.type == "insert") {
				answer = ins_monitor(req.data, AContext->Binding->PeerIP);
			}
		}
		if (req.table_name == "acts") {
			if (req.type == "insert") {
				answer = ins_acts(req.data, AContext->Binding->PeerIP);
			}
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

String TForm1::ins_monitor(String data, String ip) {
	if (!agents.count(ip)) {
		agents[ip] = 1;
		FDQuery2->SQL->Text = "delete from monitor where agent=:i";
		FDQuery2->ParamByName("i")->AsString = ip;
		FDQuery2->ExecSQL();
	}
	simple_ans reply;
	try {

		FDQuery2->SQL->Text = "insert into monitor (agent,path) values (:i,:p)";
		FDQuery2->ParamByName("i")->AsString = ip;
		FDQuery2->ParamByName("p")->AsString = data;
		FDQuery2->ExecSQL();
		FDQuery2->SQL->Text =
			"select MAX(id) m from monitor where agent=:i and path=:p";
		FDQuery2->ParamByName("i")->AsString = ip;
		FDQuery2->ParamByName("p")->AsString = data;
		FDQuery2->Active = 1;
		FDQuery2->First();
		if (!FDQuery2->Eof) {
			reply.key = StrToInt(FDQuery2->FieldByName("m")->Value);
			agents[ip] = 1;
		}
		else
			reply.key = -1;
		agents.erase(ip);
		FDQuery2->Active = 0;
	}
	catch (...) {
		reply.key = -1;
	}
	upd_ags();
	return reply.ToJson();
};

void TForm1::upd_ags() {
	FDQuery6->SQL->Text = "select agent from monitor group by agent";
	FDQuery6->Active = 1;
	int i = 1;
	ComboBox1->Clear();
	ListBox1->Clear();
	for (FDQuery6->First(); !FDQuery6->Eof; FDQuery6->Next()) {
		agents[FDQuery6->FieldByName("agent")->Value] = 1;
		ComboBox1->Items->Add(FDQuery6->FieldByName("agent")->Value);
		ListBox1->Items->Add(FDQuery6->FieldByName("agent")->Value);
	}
	FDQuery6->Active = 0;
};

void __fastcall TForm1::ComboBox1Change(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	idMons.clear();
	FDQuery3->SQL->Text = "select * from monitor where agent=:i";
	FDQuery3->ParamByName("i")->AsString = ComboBox1->Text;
	FDQuery3->Active = 1;
	int i = 1;
	StringGrid1->RowCount = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		StringGrid1->RowCount = i + 1;
		StringGrid1->Cells[0][i] = IntToStr(i);
		StringGrid1->Cells[1][i] = FDQuery3->FieldByName("agent")->Value;
		StringGrid1->Cells[2][i] = FDQuery3->FieldByName("path")->Value;
		StringGrid1->Cells[3][i] = FDQuery3->FieldByName("tsart")->Value;
		if (FDQuery3->FieldByName("tsart")->Value != FDQuery3->FieldByName
			("tfinish")->Value) {
			StringGrid1->Cells[4][i] = FDQuery3->FieldByName("tfinish")->Value;
		}
		i++;
		idMons.push_back(StrToInt(FDQuery3->FieldByName("id")->Value));
		ComboBox2->Items->Add(FDQuery3->FieldByName("path")->Value);
	}
	FDQuery3->Active = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox2Change(TObject *Sender) {
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	for (int i = 0; i < StringGrid2->ColCount; i++) {
		for (int j = 1; j < StringGrid2->RowCount; j++) {
			StringGrid2->Cells[i][j] = "";
		}
	}
	cur_mon = idMons[ComboBox2->ItemIndex];
	FDQuery4->SQL->Text = "select * from act where id=:i";
	FDQuery4->ParamByName("i")->AsInteger = idMons[ComboBox2->ItemIndex];
	FDQuery4->Active = 1;
	int i = 1;
	StringGrid2->RowCount = 1;
	for (FDQuery4->First(); !FDQuery4->Eof; FDQuery4->Next()) {
		StringGrid2->RowCount = i + 1;
		StringGrid2->Cells[0][i] = IntToStr(i);
		StringGrid2->Cells[1][i] = FDQuery4->FieldByName("file")->Value;
		StringGrid2->Cells[2][i] = FDQuery4->FieldByName("act")->Value;
		StringGrid2->Cells[3][i++] = FDQuery4->FieldByName("t_act")->Value;
	}
	FDQuery4->Active = 0;
}

// ---------------------------------------------------------------------------
void TForm1::showMons() {
	if (cur_ag == "") {
		return;
	}
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	idMons.clear();
	FDQuery3->SQL->Text = "select * from monitor where agent=:i";
	FDQuery3->ParamByName("i")->AsString = cur_ag;
	FDQuery3->Active = 1;
	int i = 1;
	StringGrid1->RowCount = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		StringGrid1->RowCount = i + 1;
		StringGrid1->Cells[0][i] = IntToStr(i);
		StringGrid1->Cells[1][i] = FDQuery3->FieldByName("agent")->Value;
		StringGrid1->Cells[2][i] = FDQuery3->FieldByName("path")->Value;
		StringGrid1->Cells[3][i] = FDQuery3->FieldByName("tsart")->Value;
		if (FDQuery3->FieldByName("tsart")->Value != FDQuery3->FieldByName
			("tfinish")->Value) {
			StringGrid1->Cells[4][i] = FDQuery3->FieldByName("tfinish")->Value;
		}
		i++;
		idMons.push_back(StrToInt(FDQuery3->FieldByName("id")->Value));
		ComboBox2->Items->Add(FDQuery3->FieldByName("path")->Value);
	}
	FDQuery3->Active = 0;
};

void TForm1::showActs() {
	if (cur_mon == -1) {
		return;
	}
	for (int i = 0; i < StringGrid2->ColCount; i++) {
		for (int j = 1; j < StringGrid2->RowCount; j++) {
			StringGrid2->Cells[i][j] = "";
		}
	}
	FDQuery4->SQL->Text = "select * from act where id=:i";
	FDQuery4->ParamByName("i")->AsInteger = cur_mon;
	FDQuery4->Active = 1;
	int i = 1;
	StringGrid2->RowCount = 1;
	for (FDQuery4->First(); !FDQuery4->Eof; FDQuery4->Next()) {
		StringGrid2->RowCount = i + 1;
		StringGrid2->Cells[0][i] = IntToStr(i);
		StringGrid2->Cells[1][i] = FDQuery4->FieldByName("file")->Value;
		StringGrid2->Cells[2][i] = FDQuery4->FieldByName("act")->Value;
		StringGrid2->Cells[3][i++] = FDQuery4->FieldByName("t_act")->Value;
	}
	FDQuery4->Active = 0;
};

String TForm1::ins_acts(String data, String ip) {
	ans_Act ans(data);
	for (int i = 0; i < ans.arr.size(); i++) {
		try {
			FDQuery5->SQL->Text =
				"insert into act (id,act,t_act,file) values (:i,:a,:t,:f)";
			FDQuery5->ParamByName("i")->AsInteger = ans.arr[i].id;
			FDQuery5->ParamByName("a")->AsString = ans.arr[i].act;
			FDQuery5->ParamByName("t")->AsString = ans.arr[i].time;
			FDQuery5->ParamByName("f")->AsString = ans.arr[i].file.name;
			FDQuery5->ExecSQL();
		}
		catch (...) {
		}
	}
	showActs();
	return simple_ans().ToJson();
};

void __fastcall TForm1::Button1Click(TObject *Sender) {
	try {
		Variant v, vVarApp, vVarBooks, vVarBook, vVarSheets, vVarSheet,
		vVarCell;
		try {
			vVarApp = CreateOleObject("Excel.Application");
			// fStart = true;
		}
		catch (...) {
			return;
		}
		vVarApp.OlePropertySet("Visible", true);
		vVarBooks = vVarApp.OlePropertyGet("Workbooks");
		vVarApp.OlePropertySet("SheetsInNewWorkbook", 3);
		vVarBooks.OleProcedure("Add");
		vVarBook = vVarBooks.OlePropertyGet("Item", 1);
		vVarSheets = vVarBook.OlePropertyGet("Worksheets");
		vVarSheet = vVarSheets.OlePropertyGet("Item", 1);
		for (int j = 0; j < StringGrid2->ColCount; j++) {
			for (int i = 0; i < StringGrid2->RowCount; i++) {
				vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet
					("Item", i + 1, j + 1);
				// UnicodeString aSG = StringGrid1->Cells[j][i];
				vVarCell.OlePropertySet("Value",
					TVariant(StringGrid2->Cells[j][i].c_str()));
				vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
			}
		}
		// ShowMessage();

		return;
	}
	catch (...) {
		return;
	}
}
// ---------------------------------------------------------------------------
