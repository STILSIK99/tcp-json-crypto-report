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
	if (initDBConnection()) {
		ShowMessage("Connected!");
	}

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Литера";
	StringGrid1->Cells[2][0] = "Тип диска";
	StringGrid1->Cells[3][0] = "Название";
	StringGrid1->Cells[4][0] = "Файловая система";
	StringGrid1->ColWidths[0] = 50;
	StringGrid1->ColWidths[1] = 100;
	StringGrid1->ColWidths[2] = 170;
	StringGrid1->ColWidths[3] = 140;
	StringGrid1->ColWidths[4] = 180;

	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Литера";
	StringGrid2->Cells[2][0] = "Тип диска";
	StringGrid2->Cells[3][0] = "Название";
	StringGrid2->Cells[4][0] = "Файловая система";
	StringGrid2->Cells[5][0] = "Серийный номер";
	StringGrid2->ColWidths[0] = 50;
	StringGrid2->ColWidths[1] = 100;
	StringGrid2->ColWidths[2] = 170;
	StringGrid2->ColWidths[3] = 140;
	StringGrid2->ColWidths[4] = 180;
	StringGrid2->ColWidths[5] = 180;
	clear();
	loadEtalon();
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
		clients[AContext] = new CryptoSession("driveInfo");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text += request;
		com req(request);

		Memo1->Lines->Add(AContext->Binding->PeerIP + " ---- " + req.ToJson());
		if (req.table_name == "user" && req.type == "select") {
			answer = log(req.data, AContext->Binding->PeerIP);
		}
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 1).ToJson();
		}
		if (req.table_name == "drives") {
			if (req.type == "insert") {
				ins_drives(req.data, AContext->Binding->PeerIP);
				answer = simple_ans().ToJson();
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
	login user(data);
	simple_ans ans;
	FDQuery1->SQL->Text = "select * from users where login=:log and pass =:pas";
	FDQuery1->ParamByName("log")->AsString = user.log;
	FDQuery1->ParamByName("pas")->AsString = user.hash;
	FDQuery1->Active = 1;
	FDQuery1->First();
	if (!FDQuery1->Eof) {
		ans.key = 1;
		FDQuery2->SQL->Text = "delete from drives where agent=:ip";
		FDQuery2->ParamByName("ip")->AsString = ip;
		FDQuery2->ExecSQL();
		mons[ip] = 0;
		loadAgs();
	}
	else
		ans.result = "Неверный логин или пароль!";

	return ans.ToJson();
};

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
	ComboBox1->Clear();
	ComboBox2->Clear();
	ListBox2->Clear();
};

void TForm1::loadAgs() {
	clear();
	for (auto x = mons.begin(); x != mons.end(); ++x) {
		String s = x->first + " - ";
		if (x->second == 0) {
			s += "сбор данных отключен";
		}
		else {
			s += "сбор данных включен";
		}
		ComboBox1->Items->Add(x->first);
		ComboBox2->Items = ComboBox1->Items;
		ListBox1->Items->Add(s);
	}
};

void TForm1::disconnect(String ip) {
	mons.erase(ip);
	loadAgs();
};

void __fastcall TForm1::ListBox1DblClick(TObject *Sender) {
	int id = ListBox1->ItemIndex;
	if (id == -1) {
		return;
	}
	auto x = mons.begin();
	for (; id != 0; --id) {
		++x;
	}
	if (x->second == 0) {
		simple_ans start("on", 1);
		try {
			Client1->Connect(x->first, 45000);
			if (!Client1->Connected()) {
				ShowMessage("Соединение отсутствует!");
				disconnect(x->first);
			}
			else {
				Client1->IOHandler->WriteLn(start.ToJson());
				simple_ans answer(Client1->IOHandler->ReadLn());
				if (answer.result == "yes") {
					mons[x->first] = 1;
				}
				else
					ShowMessage("Агент отклонил команду!");
			}
		}
		catch (...) {
		}

	}
	else {
		simple_ans stop("off", 0);
		try {
			Client1->Connect(x->first, 45000);
			if (!Client1->Connected()) {
				ShowMessage("Соединение отсутствует!");
				disconnect(x->first);
			}
			else {
				Client1->IOHandler->WriteLn(stop.ToJson());
				mons[x->first] = 0;
			}
		}
		catch (...) {
		}
	}
	Client1->Disconnect();
	loadAgs();
}

// ---------------------------------------------------------------------------
void TForm1::loadEtalon() {
	etalon.arr.clear();
	try {
		FDQuery3->SQL->Text =
			"select d.* from drives d,etalon e where e.agent=d.agent";
		FDQuery3->Active = 1;
		for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
			int k = StringGrid1->RowCount++;
			Drive x;
			x.DriveName = FDQuery3->FieldByName("a1")->Value;
			x.DriveType = FDQuery3->FieldByName("a2")->Value;
			x.NameBuffer = FDQuery3->FieldByName("a3")->Value;
			x.FileSystem = FDQuery3->FieldByName("a4")->Value;
			etalon.arr.push_back(x);
			StringGrid1->Cells[0][k] = String(k);
			StringGrid1->Cells[1][k] = FDQuery3->FieldByName("a1")->Value;
			StringGrid1->Cells[2][k] = FDQuery3->FieldByName("a2")->Value;
			StringGrid1->Cells[3][k] = FDQuery3->FieldByName("a3")->Value;
			StringGrid1->Cells[4][k] = FDQuery3->FieldByName("a4")->Value;
		}
		FDQuery3->Active = 0;
	}
	catch (...) {
	}
};

void TForm1::ins_drives(String data, String ip) {
	MasDrive ans(data);
	FDQuery5->SQL->Text = "delete from drives where agent=:i";
	FDQuery5->ParamByName("i")->AsString = ip;
	FDQuery5->ExecSQL();
	FDQuery5->SQL->Text =
		"insert into drives (agent,a1,a2,a3,a4,a5) values (:i,:a1,:a2,:a3,:a4,:a5)";
	for (int i = 0; i < ans.arr.size(); i++) {
		FDQuery5->ParamByName("i")->AsString = ip;
		FDQuery5->ParamByName("a1")->AsString = ans.arr[i].DriveName;
		FDQuery5->ParamByName("a2")->AsString = ans.arr[i].DriveType;
		FDQuery5->ParamByName("a3")->AsString = ans.arr[i].NameBuffer;
		FDQuery5->ParamByName("a4")->AsString = ans.arr[i].FileSystem;
		FDQuery5->ParamByName("a5")->AsString = ans.arr[i].VSNumber;
		FDQuery5->ExecSQL();
	}
};

void __fastcall TForm1::ComboBox1Change(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	int id = ComboBox1->ItemIndex;
	auto x = mons.begin();
	for (; id != 0; --id) {
		++x;
	}
	for (int i = 1; i < StringGrid2->RowCount; i++) {
		for (int j = 0; j < StringGrid2->ColCount; j++) {
			StringGrid2->Cells[j][i] = "";
		}
	}
	StringGrid2->RowCount = 1;
	try {
		FDQuery6->SQL->Text = "select d.* from drives d where d.agent=:i";
		FDQuery6->ParamByName("i")->AsString = x->first;
		FDQuery6->Active = 1;
		for (FDQuery6->First(); !FDQuery6->Eof; FDQuery6->Next()) {
			int k = StringGrid2->RowCount++;
			StringGrid2->Cells[0][k] = String(k);
			StringGrid2->Cells[1][k] = FDQuery6->FieldByName("a1")->Value;
			StringGrid2->Cells[2][k] = FDQuery6->FieldByName("a2")->Value;
			StringGrid2->Cells[3][k] = FDQuery6->FieldByName("a3")->Value;
			StringGrid2->Cells[4][k] = FDQuery6->FieldByName("a4")->Value;
			StringGrid2->Cells[5][k] = FDQuery6->FieldByName("a5")->Value;
		}
		FDQuery6->Active = 0;
	}
	catch (...) {
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox2Change(TObject *Sender) {
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	int id = ComboBox2->ItemIndex;
	auto x = mons.begin();
	for (; id != 0; --id) {
		++x;
	}
	map<String, Drive>cur;
	try {
		FDQuery3->SQL->Text = "select * from drives where agent=:i";
		FDQuery3->ParamByName("i")->AsString = x->first;
		FDQuery3->Active = 1;
		for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
			int k = StringGrid1->RowCount++;
			Drive x;
			x.DriveName = FDQuery3->FieldByName("a1")->Value;
			x.DriveType = FDQuery3->FieldByName("a2")->Value;
			x.NameBuffer = FDQuery3->FieldByName("a3")->Value;
			x.FileSystem = FDQuery3->FieldByName("a4")->Value;
			cur[FDQuery3->FieldByName("a1")->Value] = x;
		}
		FDQuery3->Active = 0;
	}
	catch (...) {
	}
	ListBox2->Clear();
	ListBox2->Items->Add("Недостающие диски:");
	for (int i = 0; i < etalon.arr.size(); i++) {
		if (cur.count(etalon.arr[i].DriveName) == 0) {
			ListBox2->Items->Add(etalon.arr[i].DriveName);
		}
	}
	ListBox2->Items->Add("");
	ListBox2->Items->Add("Не совпадает тип диска:");
	for (int i = 0; i < etalon.arr.size(); i++) {
		if (cur.count(etalon.arr[i].DriveName) != 0) {
			if (cur[etalon.arr[i].DriveName].DriveType != etalon.arr[i]
				.DriveType) {
				ListBox2->Items->Add(etalon.arr[i].DriveName);
			}
		}
	}
	ListBox2->Items->Add("");
	ListBox2->Items->Add("Не совпадает название диска:");
	for (int i = 0; i < etalon.arr.size(); i++) {
		if (cur.count(etalon.arr[i].DriveName) != 0) {
			if (cur[etalon.arr[i].DriveName].NameBuffer != etalon.arr[i]
				.NameBuffer) {
				ListBox2->Items->Add(etalon.arr[i].DriveName);
			}
		}
	}
	ListBox2->Items->Add("");
	ListBox2->Items->Add("Не совпадает файловая система диска:");
	for (int i = 0; i < etalon.arr.size(); i++) {
		if (cur.count(etalon.arr[i].DriveName) != 0) {
			if (cur[etalon.arr[i].DriveName].FileSystem != etalon.arr[i]
				.FileSystem) {
				ListBox2->Items->Add(etalon.arr[i].DriveName);
			}
		}
	}
	for (int i = 0; i < etalon.arr.size(); i++) {
		cur.erase(etalon.arr[i].DriveName);
	}
	ListBox2->Items->Add("");
	ListBox2->Items->Add("Лишние диски:");
	for (auto x = cur.begin(); x != cur.end(); ++x) {
		ListBox2->Items->Add(x->first);
	}
}
// ---------------------------------------------------------------------------
