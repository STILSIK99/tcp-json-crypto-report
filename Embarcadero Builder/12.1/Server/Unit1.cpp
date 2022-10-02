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
	try {
		if (!initDBConnection()) {
			return;
		}
		ShowMessage("Connected!");

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
	ans_Files ans(data);
	simple_ans reply;
	int k = -1;
	try {
		FDQuery2->SQL->Text =
			"insert into search (agent,request,path) values (:i,:r,:p)";
		FDQuery2->ParamByName("i")->AsString = ip;
		FDQuery2->ParamByName("r")->AsString = ans.zapros.reg;
		FDQuery2->ParamByName("p")->AsString = ans.zapros.path;
		FDQuery2->ExecSQL();
		FDQuery2->SQL->Text =
			"select MAX(ID) m from search where agent=:i and request=:r and path=:p";
		FDQuery2->Active = 1;
		FDQuery2->First();
		if (!FDQuery2->Eof) {
			k = StrToInt(FDQuery2->FieldByName("m")->Value);
		}
		FDQuery2->Active = 0;
	}
	catch (...) {
		k = -1;
		reply.result = "Ошибка добавления записи поиска";
	}
	upd_grid();

	for (int i = 0; i < ans.arr.size(); i++) {
		try {
			FDQuery4->SQL->Text =
				"insert into results (id,name,size,date) values (:i,:n,:s,:d)";
			FDQuery4->ParamByName("i")->AsInteger = k;
			FDQuery4->ParamByName("n")->AsString = ans.arr[i].path;
			FDQuery4->ParamByName("s")->AsString = ans.arr[i].size;
			FDQuery4->ParamByName("d")->AsString = ans.arr[i].date;
			FDQuery4->ExecSQL();
		}
		catch (...) {
			reply.result = "Не все результаты поиска внесены";
		}
	}
	return reply.ToJson();
};

void TForm1::upd_grid() {
	ListBox1->Clear();
	idSearch.clear();
	FDQuery3->SQL->Text = "select * from search order by ID desc";
	FDQuery3->Active = 1;
	int i = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		String s = "Агент: ";
		s += FDQuery3->FieldByName("agent")->Value;
		s += " Когда: ";
		s += FDQuery3->FieldByName("date")->Value;
		s += " Искал: '";
		s += FDQuery3->FieldByName("request")->Value;
		s += "' Где: ";
		s += FDQuery3->FieldByName("path")->Value;
		idSearch.push_back(StrToInt(FDQuery3->FieldByName("ID")->Value));
		ListBox1->Items->Add(s);
	}
	FDQuery3->Active = 0;
};

void TForm1::history(int id) {
	ListBox2->Clear();
	FDQuery5->SQL->Text = "select * from results where id=:i";
	FDQuery5->ParamByName("i")->AsInteger = id;
	try {
		FDQuery5->Active = 1;
		int i = 1;
		for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
			String s = "Имя файла: ";
			s += FDQuery5->FieldByName("name")->Value;
			s += " Размер файла:";
			s += FDQuery5->FieldByName("size")->Value;
			s += " Дата создания:";
			s += FDQuery5->FieldByName("date")->Value;
			ListBox2->Items->Add(s);
		}
	}
	catch (...) {
	}
	FDQuery3->Active = 0;

};

void __fastcall TForm1::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		return;
	}
	history(idSearch[ListBox1->ItemIndex]);
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

	String s = "";
	for (int i = 0; i < ListBox2->Items->Count; i++) {
		s += String(i + 1) + ".\t" + ListBox2->Items->Strings[i] + ".\r\n";
	}

	vVarDoc = vVarDocs.OleFunction("Item", 1);
	vVarDoc.OleProcedure("Activate");

	vVarParagraphs = vVarDoc.OlePropertyGet("Paragraphs");
	vVarParagraphs.OleProcedure("Add");
	vVarParagraph = vVarParagraphs.OleFunction("Item", 1);
	vVarParagraph.OlePropertyGet("Range").OlePropertySet("Text",
		WideString(s.c_str()));
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
