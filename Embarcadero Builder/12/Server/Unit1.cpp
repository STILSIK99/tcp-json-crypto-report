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
	try {
		if (!initDBConnection()) {
			return;
		}
		ShowMessage("Connected!");
		StringGrid1->Cells[0][0] = "№";
		StringGrid1->Cells[1][0] = "Дата запроса";
		StringGrid1->Cells[2][0] = "Адрес агента";
		StringGrid1->Cells[3][0] = "Директория поиска";
		StringGrid1->Cells[4][0] = "Запрос поиска";
		StringGrid1->Cells[5][0] = "Ограничение по дате создания";
		StringGrid1->Cells[6][0] = "Ограничение по размеру файла";

		StringGrid1->ColWidths[0] = 70;
		StringGrid1->ColWidths[1] = 200;
		StringGrid1->ColWidths[2] = 200;
		StringGrid1->ColWidths[3] = 250;
		StringGrid1->ColWidths[4] = 250;
		StringGrid1->ColWidths[5] = 300;
		StringGrid1->ColWidths[6] = 300;

		StringGrid2->Cells[0][0] = "№";
		StringGrid2->Cells[1][0] = "Файл";
		StringGrid2->Cells[2][0] = "Размер";
		StringGrid2->Cells[3][0] = "Дата создания";

		StringGrid2->ColWidths[0] = 70;
		StringGrid2->ColWidths[1] = 350;
		StringGrid2->ColWidths[2] = 150;
		StringGrid2->ColWidths[3] = 150;

		upd_grid();
	}
	catch (...) {
		Form1->Close();
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
		clients[AContext] = new CryptoSession("poisk");
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
		if (req.table_name == "search") {
			if (req.type == "insert") {
				answer = ins_search(req.data, AContext->Binding->PeerIP);
			}
		}
		if (req.table_name == "result") {
			if (req.type == "insert") {
				ins_result(req.data);
				answer = simple_ans().ToJson();
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
	simple_ans ans;
	FDQuery1->SQL->Text =
		"select count(*) m from audit where pash=:p and userName=:u";
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

String TForm1::ins_search(String data, String ip) {
	SSearch ans(data);
	simple_ans reply;
	try {

		FDQuery2->SQL->Text =
			"insert into search (agent,request,path,size,dateC) values (:i,:r,:p,:s,:d)";
		FDQuery2->ParamByName("i")->AsString = ip;
		FDQuery2->ParamByName("r")->AsString = ans.reg;
		FDQuery2->ParamByName("p")->AsString = ans.path;
		FDQuery2->ParamByName("s")->AsString = ans.size;
		FDQuery2->ParamByName("d")->AsString = ans.dateC;
		FDQuery2->ExecSQL();
		FDQuery2->SQL->Text =
			"select MAX(ID) m from search where agent=:i and request=:r and path=:p";
		FDQuery2->Active = 1;
		FDQuery2->First();
		if (!FDQuery2->Eof) {
			reply.key = StrToInt(FDQuery2->FieldByName("m")->Value);
		}
		else
			reply.key = -1;
		FDQuery2->Active = 0;
	}
	catch (...) {
		reply.key = -1;
	}
	upd_grid();
	return reply.ToJson();
};

void TForm1::upd_grid() {
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	ComboBox1->Clear();
	idSearch.clear();
	FDQuery3->SQL->Text = "select * from search order by ID desc";
	FDQuery3->Active = 1;
	int i = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		StringGrid1->RowCount = i + 1;
		StringGrid1->Cells[0][i] = IntToStr(i);
		StringGrid1->Cells[1][i] = FDQuery3->FieldByName("date")->Value;
		StringGrid1->Cells[2][i] = FDQuery3->FieldByName("agent")->Value;
		StringGrid1->Cells[3][i] = FDQuery3->FieldByName("path")->Value;
		StringGrid1->Cells[4][i] = FDQuery3->FieldByName("request")->Value;
		StringGrid1->Cells[5][i] = FDQuery3->FieldByName("dateC")->Value;
		StringGrid1->Cells[6][i] = FDQuery3->FieldByName("size")->Value;
		idSearch.push_back(StrToInt(FDQuery3->FieldByName("ID")->Value));
		ComboBox1->Items->Add(i++);
	}
	FDQuery3->Active = 0;
};

void TForm1::ins_result(String data) {
	ans_Files ans(data);
	for (int i = 0; i < ans.arr.size(); i++) {
		try {
			FDQuery4->SQL->Text =
				"insert into results (id,name,size,date) values (:i,:n,:s,:d)";
			FDQuery4->ParamByName("i")->AsInteger = ans.id;
			FDQuery4->ParamByName("n")->AsString = ans.arr[i].path;
			FDQuery4->ParamByName("s")->AsString = ans.arr[i].size;
			FDQuery4->ParamByName("d")->AsString = ans.arr[i].date;
			FDQuery4->ExecSQL();
		}
		catch (...) {
		}
	}
};

void TForm1::getSearchInfo(int id) {
	FDQuery5->SQL->Text = "select * from results where id=:i";
	FDQuery5->ParamByName("i")->AsInteger = id;
	for (int i = 0; i < StringGrid2->ColCount; i++) {
		for (int j = 1; j < StringGrid2->RowCount; j++) {
			StringGrid2->Cells[i][j] = "";
		}
	}
	try {
		FDQuery5->Active = 1;
		int i = 1;
		for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
			StringGrid2->RowCount = i + 1;
			StringGrid2->Cells[0][i] = IntToStr(i);
			StringGrid2->Cells[1][i] = FDQuery5->FieldByName("name")->Value;
			StringGrid2->Cells[2][i] = FDQuery5->FieldByName("size")->Value;
			StringGrid2->Cells[3][i++] = FDQuery5->FieldByName("date")->Value;
		}
	}
	catch (...) {
	}
	FDQuery3->Active = 0;

};

void __fastcall TForm1::Button1Click(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	getSearchInfo(idSearch[ComboBox1->ItemIndex]);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender) {
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
