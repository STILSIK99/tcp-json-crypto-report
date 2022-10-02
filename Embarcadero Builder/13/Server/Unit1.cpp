// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"

#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
#include <utilcls.h>
#include <regex>

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
using namespace std;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {

	if (!initDBConnection()) {
		Application->Terminate();
	}

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Агент";
	StringGrid1->Cells[2][0] = "Время";
	StringGrid1->Cells[3][0] = "Файл";
	StringGrid1->Cells[4][0] = "Сигнатура";
	StringGrid1->ColWidths[0] = 50;
	StringGrid1->ColWidths[1] = 100;
	StringGrid1->ColWidths[2] = 100;
	StringGrid1->ColWidths[3] = 300;
	StringGrid1->ColWidths[4] = 300;

	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Сигнатура";
	StringGrid2->Cells[2][0] = "Последовательность";
	StringGrid2->Cells[3][0] = "Дата добавления";
	StringGrid2->ColWidths[0] = 50;
	StringGrid2->ColWidths[1] = 150;
	StringGrid2->ColWidths[2] = 400;
	StringGrid2->ColWidths[3] = 200;

	StringGrid3->Cells[0][0] = "№";
	StringGrid3->Cells[1][0] = "Агент";
	StringGrid3->Cells[2][0] = "Время";
	StringGrid3->ColWidths[0] = 50;
	StringGrid3->ColWidths[1] = 100;
	StringGrid3->ColWidths[2] = 100;

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
	// ShowMessage(ConnectionString);
	try {
		Form1->FDConnection1->Open();
	}
	catch (...) {
		return false;
	}
	return Form1->FDConnection1->Connected;
};

void TForm1::send_mes(String request, TIdContext *AContext) {
	AContext->Connection->IOHandler->WriteLn(request, enUTF8);
	AContext->Connection->Disconnect();
};

// ---------------------------------------------------------------------------

void __fastcall TForm1::ServerExecute(TIdContext *AContext) {
	UnicodeString answer = "";
	try {
		UnicodeString request = AContext->Connection->IOHandler->ReadLn(enUTF8);
		Form2->Memo1->Text += request;
		com req(request);

		Form2->Memo1->Text += AContext->Binding->PeerIP + " ---- " +
			req.ToJson();
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 123).ToJson();
		}
		if (req.table_name == "scanning") {
			if (req.type == "insert") {
				answer = ins_scan(req.data, AContext->Binding->PeerIP);
				loadData();
			}
		}
		if (req.table_name == "sign") {
			if (req.type == "select") {
				answer = sel_signs();
			}
		}
		if (req.table_name == "login") {
			if (req.type == "select") {
				answer = log(req.data);
			}
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
	Form2->Memo1->Text += answer;
	AContext->Connection->Disconnect();
}

// ---------------------------------------------------------------------------
String TForm1::sel_signs() {
	ans_Sign ans;
	FDQuery7->SQL->Text = "select * from sign";
	FDQuery7->Active = 1;
	for (FDQuery7->First(); !FDQuery7->Eof; FDQuery7->Next()) {
		ans.arr.push_back(Sign(FDQuery7->FieldByName("sign")->Value,
			FDQuery7->FieldByName("data")->Value,
			FDQuery7->FieldByName("time")->Value));
	}
	FDQuery7->Active = 0;
	return ans.ToJson();
};

String TForm1::ins_scan(String data, String ip) {
	ans_Scan ans(data);
	simple_ans reply;
	FDQuery6->SQL->Text = "insert into scanning values(:f,:s,:a,:t,null)";
	for (int i = 0; i < ans.arr.size(); i++) {
		FDQuery6->ParamByName("f")->AsString = ans.arr[i].file;
		FDQuery6->ParamByName("s")->AsString = ans.arr[i].sign;
		FDQuery6->ParamByName("a")->AsString = ip;
		FDQuery6->ParamByName("t")->AsString = Now().TimeString();
		try {
			FDQuery6->ExecSQL();
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

bool TForm1::check(String sign) {
	// (String reg, String str) {
	string subject = AnsiString(sign).c_str();
	string result;
	try {
		regex re("[a-f0-9]+");
		smatch match;
		if (regex_match(subject, match, re)) {
			return true;
		}
	}
	catch (regex_error& e) {
		return false;
	}
	return false;
};

void __fastcall TForm1::Button2Click(TObject *Sender) {
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	FDQuery5->SQL->Text = "delete from sign where sign=:s";
	FDQuery5->ParamByName("s")->AsString = ComboBox2->Text;
	FDQuery5->ExecSQL();
	loadData();
}

void __fastcall TForm1::Button4Click(TObject *Sender) {
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
		for (int j = 0; j < StringGrid1->ColCount; j++) {
			for (int i = 0; i < StringGrid1->RowCount; i++) {
				vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet
					("Item", i + 1, j + 1);
				// UnicodeString aSG = StringGrid1->Cells[j][i];
				vVarCell.OlePropertySet("Value",
					TVariant(StringGrid1->Cells[j][i].c_str()));
				if (j == 2) {
					vVarCell.OlePropertySet("ColumnWidth", TVariant(60));
				}
				else
					vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
			}
		}
		// ShowMessage();
	}
	catch (...) {
	}
}

// ---------------------------------------------------------------------------

void TForm1::loadData() {
	ComboBox1->ItemIndex = -1;
	ComboBox2->ItemIndex = -1;
	Edit1->Text = "";
	Memo1->Text = "";
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid2->Cells[i][j] = "";
		}
	}
	StringGrid1->RowCount = 1;

	for (int i = 0; i < StringGrid2->ColCount; i++) {
		for (int j = 1; j < StringGrid2->RowCount; j++) {
			StringGrid2->Cells[i][j] = "";
		}
	}
	StringGrid2->RowCount = 1;

	for (int i = 0; i < StringGrid3->ColCount; i++) {
		for (int j = 1; j < StringGrid3->RowCount; j++) {
			StringGrid3->Cells[i][j] = "";
		}
	}
	StringGrid3->RowCount = 1;

	FDQuery1->SQL->Text = "select * from sign";
	FDQuery1->Active = 1;
	signBase.clear();
	ComboBox2->Clear();
	for (FDQuery1->First(); !FDQuery1->Eof; FDQuery1->Next()) {
		signBase.push_back(Sign(FDQuery1->FieldByName("sign")->Value,
			FDQuery1->FieldByName("data")->Value,
			FDQuery1->FieldByName("time")->Value));
		int k = StringGrid2->RowCount++;
		StringGrid2->Cells[0][k] = String(k);
		StringGrid2->Cells[1][k] = FDQuery1->FieldByName("sign")->Value;
		StringGrid2->Cells[2][k] = FDQuery1->FieldByName("data")->Value;
		StringGrid2->Cells[3][k] = FDQuery1->FieldByName("time")->Value;
		ComboBox2->Items->Add(FDQuery1->FieldByName("sign")->Value);
	}
	FDQuery1->Active = 0;

	ComboBox1->Clear();
	zapr.clear();
	FDQuery1->SQL->Text =
		"select agent,time from scanning group by agent, time";
	FDQuery1->Active = 1;
	for (FDQuery1->First(); !FDQuery1->Eof; FDQuery1->Next()) {
		int k = StringGrid3->RowCount++;
		StringGrid3->Cells[0][k] = String(k);
		StringGrid3->Cells[1][k] = FDQuery1->FieldByName("agent")->Value;
		StringGrid3->Cells[2][k] = FDQuery1->FieldByName("time")->Value;
		ComboBox1->Items->Add(StringGrid3->Cells[1][k] + " - " +
			StringGrid3->Cells[2][k]);
		zapr.push_back("agent='" + StringGrid3->Cells[1][k] + "' and time='" +
			StringGrid3->Cells[2][k] + "'");
	}
	FDQuery1->Active = 0;

	try {
		StringGrid1->FixedRows = 1;
	}
	catch (...) {
	}

	try {
		StringGrid2->FixedRows = 1;
	}
	catch (...) {
	}
	try {
		StringGrid3->FixedRows = 1;
	}
	catch (...) {
	}
}

void __fastcall TForm1::Button3Click(TObject *Sender) {
	if (Edit1->Text == "") {
		ShowMessage("Задайте название!");
		return;
	}
	String data = Memo1->Text.LowerCase();
	if (!check(data)) {
		ShowMessage("Неверные данные");
		return;
	}

	FDQuery4->SQL->Text = "insert into sign values (:s,:d,:t)";
	FDQuery4->ParamByName("s")->AsString = Edit1->Text;
	FDQuery4->ParamByName("d")->AsString = data;
	FDQuery4->ParamByName("t")->AsString = Now().DateString();

	FDQuery4->ExecSQL();
	loadData();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox1Change(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	FDQuery3->SQL->Text = "select * from scanning where " +
		zapr[ComboBox1->ItemIndex];
	FDQuery3->Active = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		int k = StringGrid1->RowCount++;
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = FDQuery3->FieldByName("agent")->Value;
		StringGrid1->Cells[2][k] = FDQuery3->FieldByName("time")->Value;
		StringGrid1->Cells[3][k] = FDQuery3->FieldByName("file")->Value;
		StringGrid1->Cells[4][k] = FDQuery3->FieldByName("sign")->Value;
	}
	FDQuery3->Active = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	Form2->Show();
}
// ---------------------------------------------------------------------------
