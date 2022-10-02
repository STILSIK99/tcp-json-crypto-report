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
	// FDConnection1->Connected = 1;
	if (initDBConnection()) {
		ShowMessage("Connected!");
	}
	idAg = -1;
	idDisk = -1;
	Series1->Title = "Свободное место";
	Series2->Title = "Занято";
	Series3->Title = "Ограничение";
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
		clients[AContext] = new CryptoSession("watcher");
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
		if (req.table_name == "info") {
			if (req.type == "insert") {
				answer = ins_info(req.data, AContext->Binding->PeerIP);
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
	FDQuery1->SQL->Text = "select * from user where log=:log and pasw =:pasw";
	// FDQuery1->SQL->Text = "select * from user where log=:log and pasw=:pasw";
	FDQuery1->ParamByName("log")->AsString = user.log;
	FDQuery1->ParamByName("pasw")->AsString = user.hash;
	// FDQuery1->ExecSQL();
	FDQuery1->Active = 1;
	FDQuery1->First();
	if (!FDQuery1->Eof) {
		ans.key = 1;
		FDQuery3->SQL->Text = "delete from info where agent=:ip";
		FDQuery3->ParamByName("ip")->AsString = ip;
		FDQuery3->ExecSQL();
		agents.push_back(ip);
		mons[ip] = 0;
		map<String, double>a;
		IpDiskBound[ip] = a;
		FDQuery1->Active = 0;
		vector<listDisk>n;
		IpDisks[ip] = n;
		loadData(0);
	}
	else
		ans.result = "Неверный логин или пароль!";

	return ans.ToJson();
};

void TForm1::clear() {
	ListBox1->Clear();
	ListBox2->Clear();
	Chart1->Title->Text->Clear();
	LabeledEdit1->Text = "";
	ComboBox1->Clear();
	ComboBox2->Clear();
	ComboBox3->Clear();
	ComboBox4->Clear();
};

String TForm1::ins_info(String data, String ip) {
	simple_ans ans;
	try {
		listDisk req(data);
		req.cname = ip;
		if (IpDisks.count(ip) == 0) {
			vector<listDisk>a;
			IpDisks[ip] = a;
		}
		IpDisks[ip].push_back(req);
		for (int i = 0; i < req.arr.size(); i++) {
			FDQuery2->SQL->Text =
				"insert into info (agent,disk,free,busy) values(:ag,:d,:free,:busy)";
			FDQuery2->ParamByName("ag")->AsString = ip;
			FDQuery2->ParamByName("d")->AsString = req.arr[i].name;
			FDQuery2->ParamByName("free")->AsString = req.arr[i].fr;
			FDQuery2->ParamByName("busy")->AsString = req.arr[i].busy;
			FDQuery2->ExecSQL();
		}
		ans.result = "Данные добавлены!";
		loadData(1);
		checkDisks(ip);
	}
	catch (...) {
		ans.result = "Ошибка добавления!";
	}
	return ans.ToJson();
};

void TForm1::loadData(int a = 0) {
	if (a == 0) {
		clear();
		for (int i = 0; i < agents.size(); i++) {
			String s = agents[i] + " - ";
			if (mons[agents[i]] == 0) {
				s += "мониторинг отключен";
				ComboBox3->Items->Add(agents[i]);
			}
			else {
				s += "мониторинг включен";
				ComboBox4->Items->Add(agents[i]);
			}
			ListBox1->Items->Add(s);
			ComboBox1->Items = ComboBox4->Items;
		}
	}

	if (idAg != -1 && idDisk != -1) {
		Series1->Clear();
		Series2->Clear();
		Series3->Clear();
		Label1->Caption = idAg;
		for (int i = 0; i < IpDisks[idAg].size(); i++) {
			if (IpDisks[idAg][i].arr.size() < idDisk) {
				continue;
			}
			Series1->AddXY(i*5, IpDisks[idAg][i].arr[idDisk].fr.ToDouble());
			Series2->AddXY(i*5, IpDisks[idAg][i].arr[idDisk].busy.ToDouble());

			if (IpDiskBound.count(idAg) == 0) {
				continue; ;
			}
			if (IpDiskBound[idAg].count(IpDisks[idAg][0].arr[idDisk].name) == 0)
			{
				continue;
			}
			Series3->AddXY(i*5, IpDiskBound[idAg][nDisk]);
		}
	}

};

void __fastcall TForm1::ListBox2DblClick(TObject *Sender) {
	if (ListBox2->ItemIndex == -1) {
		return;
	}
	if (IpDisks.count(idAg) == 0) {
		return;
	}
	Series1->Clear();
	Series2->Clear();
	Series3->Clear();
	idDisk = ListBox2->ItemIndex;
	nDisk = IpDisks[idAg][0].arr[idDisk].name;
	Chart1->Title->Text->Clear();
	Chart1->Title->Text->Add(ListBox2->Items->Strings[idDisk]);
	Label1->Caption = idAg;
	for (int i = 0; i < IpDisks[idAg].size(); i++) {
		Series1->AddXY(i*5, IpDisks[idAg][i].arr[idDisk].fr.ToDouble());
		Series2->AddXY(i*5, IpDisks[idAg][i].arr[idDisk].busy.ToDouble());
		Series3->AddXY(i*5, IpDiskBound[idAg][nDisk]);
	}

}
// ---------------------------------------------------------------------------

void __fastcall TForm1::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		return;
	}
	ListBox2->Clear();
	idAg = agents[ListBox1->ItemIndex];
	if (IpDisks.count(idAg) != 0) {
		if (IpDisks[idAg].size() < 0) {
			return;
		}
		for (int i = 0; i < IpDisks[idAg][0].arr.size(); i++) {
			String a = IpDisks[idAg][0].arr[i].name;
			if (IpDiskBound.count(idAg) == 0) {
				ListBox2->Items->Add(a + " ограничение - " + String(0));
				continue;
			}
			if (IpDiskBound[idAg].count(IpDisks[idAg][0].arr[i].name) == 0) {
				ListBox2->Items->Add(a + " ограничение - " + String(0));
				continue;
			}
			a += " ограничение - " +
				String(IpDiskBound[idAg][IpDisks[idAg][0].arr[i].name]);
			ListBox2->Items->Add(a);
		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender) {
	if (ComboBox3->ItemIndex == -1) {
		return;
	}
	simple_ans start("on", 20);
	try {
		Client1->Connect(ComboBox3->Text, 45000);
		if (!Client1->Connected()) {
			ShowMessage("Соединение отсутствует!");
			for (int i = 0; i < agents.size(); i++) {
				if (agents[i] == ComboBox3->Text) {
					if (idAg == i) {
						idAg = -1;
						idDisk = -1;
					}
					agents.erase(agents.begin() + i);
					mons.erase(ComboBox3->Text);
					IpDisks.erase(ComboBox3->Text);
					IpDiskBound.erase(ComboBox3->Text);
					break;
				}
			}
		}
		else {
			Client1->IOHandler->WriteLn(start.ToJson());
			simple_ans answer(Client1->IOHandler->ReadLn());
			if (answer.result == "yes") {
				mons[ComboBox3->Text] = 20;
			}
			else
				ShowMessage("Агент отклонил команду!");
		}

	}
	catch (...) {
	}
	Client1->Disconnect();
	loadData();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	if (ComboBox4->ItemIndex == -1) {
		ShowMessage("Выберите агента!");
		return;
	}
	simple_ans stop("off", 0);
	try {
		Client1->Connect(ComboBox4->Text, 45000);
		if (!Client1->Connected()) {
			ShowMessage("Соединение отсутствует!");
			for (int i = 0; i < agents.size(); i++) {
				if (agents[i] == ComboBox4->Text) {
					agents.erase(agents.begin() + i);
					mons.erase(ComboBox4->Text);
					IpDisks.erase(ComboBox4->Text);
					IpDiskBound.erase(ComboBox4->Text);
					break;
				}
			}
			loadData();
		}
		else {
			Client1->IOHandler->WriteLn(stop.ToJson());
			mons[ComboBox4->Text] = 0;
		}

	}
	catch (...) {
	}
	Client1->Disconnect();
	loadData();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	double ogr;
	try {
		ogr = LabeledEdit1->Text.ToDouble();
	}
	catch (...) {
		return;
	}
	IpDiskBound[ComboBox1->Text][ComboBox2->Text] = ogr;

	ListBox2->Clear();
	if (IpDisks.count(ComboBox1->Text) != 0) {
		for (int i = 0; i < IpDisks[ComboBox1->Text][0].arr.size(); i++) {
			String a = IpDisks[ComboBox1->Text][0].arr[i].name;
			if (IpDiskBound.count(ComboBox1->Text) == 0) {
				ListBox2->Items->Add(a);
				continue;
			}
			if (IpDiskBound[ComboBox1->Text].count(a) == 0) {
				ListBox2->Items->Add(a);
				continue;
			}
			a += " ограничение - " + String(IpDiskBound[ComboBox1->Text][a]);
			ListBox2->Items->Add(a);
		}
	}
	ComboBox1->ItemIndex = -1;
	ComboBox2->ItemIndex = -1;
	LabeledEdit1->Text = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox1Change(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1)
		return;
	ComboBox2->Clear();
	if (IpDisks.count(agents[ComboBox1->ItemIndex]) == 0) {
		return;
	}
	if (IpDisks[agents[ComboBox1->ItemIndex]].size() == 0) {
		return;
	}
	for (int i = 0; i < IpDisks[agents[ComboBox1->ItemIndex]][0].arr.size
		(); i++) {
		ComboBox2->Items->Add
			(IpDisks[agents[ComboBox1->ItemIndex]][0].arr[i].name);
	}
}

// ---------------------------------------------------------------------------
void TForm1::checkDisks(String ip) {
	if (IpDiskBound.count(ip) == 0) {
		return;
	}
	listDisk last = IpDisks[ip][IpDisks[ip].size() - 1];
	for (int i = 0; i < last.arr.size(); i++) {
		if (IpDiskBound[ip][last.arr[i].name] < 0.1) {
			continue;
		}
		if (last.arr[i].busy.ToDouble() > IpDiskBound[ip][last.arr[i].name]) {
			ListBox3->Items->Add(Now().TimeString() + " - У агента " + ip +
				" на диске " + last.arr[i].name +
				" превышено ограничение занятого места");
		}
	}

};

void TForm1::disconnect(String ip) {
	for (int i = 0; i < agents.size(); i++) {
		if (agents[i] == ip) {
			agents.erase(agents.begin() + i);
			break;
		}
	}
	IpDisks.erase(ip);
	IpDiskBound.erase(ip);
	mons.erase(ip);
	loadData(0);
};

void __fastcall TForm1::Button4Click(TObject *Sender) {
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
	for (int i = 0; i < ListBox3->Items->Count; i++) {
		s += String(i + 1) + ".\t" + ListBox3->Items->Strings[i] + ".\r\n";
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
