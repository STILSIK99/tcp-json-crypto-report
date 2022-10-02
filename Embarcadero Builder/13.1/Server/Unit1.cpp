// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

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
	StringGrid1->Cells[1][0] = "Название";
	StringGrid1->Cells[2][0] = "Смещение";
	StringGrid1->Cells[3][0] = "Сигнатура";
	StringGrid1->ColWidths[0] = 50;
	StringGrid1->ColWidths[1] = 150;
	StringGrid1->ColWidths[2] = 100;
	StringGrid1->ColWidths[3] = 300;

	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Агент";
	StringGrid2->Cells[2][0] = "Файл";
	StringGrid2->Cells[3][0] = "Найденная сигнатура";
	StringGrid2->ColWidths[0] = 50;
	StringGrid2->ColWidths[1] = 150;
	StringGrid2->ColWidths[2] = 400;
	StringGrid2->ColWidths[3] = 100;

	loadSign();

	loadAgs();
	loadResults();

	StringGrid1->FixedRows = 1;
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
		clients[AContext] = new CryptoSession("signAnalyzer");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text += request;
		com req(request);

		Memo1->Text += AContext->Binding->PeerIP + " ---- " + req.ToJson();
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 123).ToJson();
		}
		if (req.table_name == "results") {
			if (req.type == "insert") {
				answer = ins_res(req.data, AContext->Binding->PeerIP);
				loadAgs();
			}
		}
		if (req.table_name == "sign") {
			if (req.type == "select") {
				answer = sel_signs();
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
	Memo1->Text += answer;
	AContext->Connection->Disconnect();
}

// ---------------------------------------------------------------------------
String TForm1::sel_signs() {
	ans_Sign ans;
	FDQuery7->SQL->Text = "select * from sign";
	FDQuery7->Active = 1;
	for (FDQuery7->First(); !FDQuery7->Eof; FDQuery7->Next()) {
		ans.arr.push_back(Signature(FDQuery7->FieldByName("name")->Value,
			FDQuery7->FieldByName("posled")->Value,
			FDQuery7->FieldByName("shift")->Value));
	}
	FDQuery7->Active = 0;
	return ans.ToJson();
};

String TForm1::ins_res(String data, String ip) {
	ans_Res ans(data);
	simple_ans reply;
	FDQuery6->SQL->Text = "insert into results values(null,:a,:f,:s)";
	for (int i = 0; i < ans.arr.size(); i++) {
		FDQuery6->ParamByName("a")->AsString = ip;
		FDQuery6->ParamByName("f")->AsString = ans.arr[i].filename;
		FDQuery6->ParamByName("s")->AsString = ans.arr[i].signName;
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

void __fastcall TForm1::ComboBox2Change(TObject *Sender) {
	ComboBox3->Clear();
	loadResults();
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	FDQuery4->SQL->Text = "select DISTINCT file from results where agent=:a";
	FDQuery4->ParamByName("a")->AsString = ComboBox2->Text;
	FDQuery4->Active = 1;
	for (FDQuery4->First(); !FDQuery4->Eof; FDQuery4->Next()) {
		ComboBox3->Items->Add(FDQuery4->FieldByName("file")->Value);
	}
	FDQuery4->Active = 0;

}

void TForm1::loadAgs() {
	ComboBox2->Clear();
	FDQuery3->SQL->Text = "select DISTINCT agent from results";
	FDQuery3->Active = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		ComboBox2->Items->Add(FDQuery3->FieldByName("agent")->Value);
	}
	FDQuery3->Active = 0;
};

void __fastcall TForm1::ComboBox3Change(TObject *Sender) {
	loadResults();
}

bool TForm1::checkSign(String sign) {
	// (String reg, String str) {
	string subject = AnsiString(sign).c_str();
	string result;
	try {
		regex re("[A-F0-9\.]+");
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
	if (!checkSign(LabeledEdit1->Text)) {
		ShowMessage("Используйте символы: \" 0-9, A-F, . \"");
		return;
	}

	try {
		LabeledEdit2->Text.ToInt();
	}
	catch (...) {
		ShowMessage("Неверное смещение");
		return;
	}

	FDQuery1->SQL->Text = "insert into sign values (null,:p,:s,:n)";
	FDQuery1->ParamByName("p")->AsString = LabeledEdit1->Text;
	FDQuery1->ParamByName("s")->AsString = LabeledEdit2->Text;
	FDQuery1->ParamByName("n")->AsString = LabeledEdit3->Text;
	FDQuery1->ExecSQL();

	LabeledEdit1->Clear();
	LabeledEdit2->Clear();
	LabeledEdit3->Clear();

	loadSign();

}

void TForm1::loadSign() {
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	StringGrid1->RowCount = 1;
	signs.clear();

	FDQuery2->SQL->Text = "select * from sign";
	FDQuery2->Active = 1;
	for (FDQuery2->First(); !FDQuery2->Eof; FDQuery2->Next()) {
		int k = StringGrid1->RowCount++;
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = FDQuery2->FieldByName("name")->Value;
		StringGrid1->Cells[2][k] = FDQuery2->FieldByName("descr")->Value;
		StringGrid1->Cells[3][k] = FDQuery2->FieldByName("shift")->Value;
		StringGrid1->Cells[4][k] = FDQuery2->FieldByName("posled")->Value;
		ComboBox1->Items->Add(FDQuery2->FieldByName("name")->Value);
		signs.push_back(StrToInt(FDQuery2->FieldByName("id")->Value));
	}
	FDQuery2->Active = 0;
};

// ---------------------------------------------------------------------------

void TForm1::loadResults() {
	FDQuery5->SQL->Text = "select DISTINCT * from results where 1=1 ";
	if (ComboBox2->ItemIndex != -1) {
		FDQuery5->SQL->Text += " and agent=:a ";
		FDQuery5->ParamByName("a")->AsString = ComboBox2->Text;
	}
	if (ComboBox3->ItemIndex != -1) {
		FDQuery5->SQL->Text += " and file=:f ";
		FDQuery5->ParamByName("f")->AsString = ComboBox3->Text;
	}
	for (int i = 0; i < StringGrid2->ColCount; i++) {
		for (int j = 1; j < StringGrid2->RowCount; j++) {
			StringGrid2->Cells[i][j] = "";
		}
	}
	StringGrid2->RowCount = 1;

	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		int k = StringGrid2->RowCount++;
		StringGrid2->Cells[0][k] = String(k);
		StringGrid2->Cells[1][k] = FDQuery5->FieldByName("agent")->Value;
		StringGrid2->Cells[2][k] = FDQuery5->FieldByName("file")->Value;
		StringGrid2->Cells[3][k] = FDQuery5->FieldByName("signName")->Value;
	}
	FDQuery5->Active = 0;
	if (StringGrid2->RowCount > 1) {
		StringGrid2->FixedRows = 1;
	}
};

void __fastcall TForm1::Button4Click(TObject *Sender) {
	ComboBox2->ItemIndex = -1;
	ComboBox3->ItemIndex = -1;
	loadResults();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	FDQuery8->SQL->Text = "delete from sign where id=:i";
	FDQuery8->ParamByName("i")->AsInteger = signs[ComboBox1->ItemIndex];
	FDQuery8->ExecSQL();
	loadSign();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
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
