// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <utilcls.h>
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	cryptoSession = new CryptoSession("driver");
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::N1Click(TObject *Sender) {
	GroupBox1->Show();
	Image1->Hide();
}

// ---------------------------------------------------------------------------
String TForm1::send_mes(String request) {
	try {
		// ShowMessage(request);
		Client->Connect(LabeledEdit3->Text, LabeledEdit4->Text.ToInt());
		if (!Client->Connected()) {
			ShowMessage("Соединение отсутствует!");
			return "";
		}
		char* crypted = 0;
		unsigned long size = request.Length() * 2 + 2;
		cryptoSession->CryptData((const char*)request.data(), crypted, size);
		TMemoryStream * ms = new TMemoryStream;
		ms->Write(crypted, size);
		delete[]crypted;
		ms->Seek(0, 0);
		Client->IOHandler->WriteBufferOpen();
		Client->IOHandler->Write(ms, -1, true);
		Client->IOHandler->WriteBufferClose();
		ms->Free();
		return decrypt();
	}
	catch (...) {
		// ShowMessage("Ошибка сообщения!");
		Client->Disconnect();
		return "";
	}
};

String TForm1::decrypt() {
	TMemoryStream * ms = new TMemoryStream;
	Client->IOHandler->ReadStream(ms);
	unsigned long size = ms->Size;
	char * crypted = new char[size];
	ms->Seek(0, 0);
	ms->ReadBuffer(crypted, size);
	char * decrypted = 0;
	cryptoSession->DecryptData(crypted, decrypted, size);
	ms->Free();
	Client->Disconnect();
	return String((wchar_t*)decrypted);
};

// ---------------------------------------------------------------------------
String TForm1::match_regex(String reg, String str) {
	string subject = AnsiString(str).c_str();
	string result;
	try {
		regex re(AnsiString(reg).c_str());
		smatch match;
		if (regex_search(subject, match, re)) {
			return str;
		}
	}
	catch (regex_error& e) {
	}
	return "";
};

listDriv TForm1::readDrivs() {
	listDriv ans;
	LPVOID drivers[ARRAY_SIZE];
	DWORD cbNeeded;
	int cDrivers, i;
	char buffer[256];
	DWORD size = 256;
	GetComputerNameA(buffer, &size);
	// String cname = AnsiString(buffer);
	String cname;
	cname = IP1->CurrentIP;
	compName = cname;

	if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded <
		sizeof(drivers)) {
		TCHAR szDriver[ARRAY_SIZE];

		cDrivers = cbNeeded / sizeof(drivers[0]);

		_tprintf(TEXT("There are %d drivers:\n"), cDrivers);
		for (i = 0; i < cDrivers; i++) {
			if (GetDeviceDriverBaseName(drivers[i], szDriver,
				sizeof(szDriver) / sizeof(szDriver[0]))) {
				// _tprintf(TEXT("%d: %s\n"), i + 1, szDriver);
				driv a;
				a.dname = szDriver;
				ans.arr.push_back(a);
			}
		}
	}
	else {
	}
	ans.cname = cname;
	return ans;
}

void __fastcall TForm1::Button1Click(TObject *Sender) {
	login n_w;
	n_w.log = LabeledEdit1->Text;
	TIdHashMessageDigest5* md5 = new TIdHashMessageDigest5;
	n_w.hash = md5->HashStringAsHex(LabeledEdit1->Text + md5->HashStringAsHex
		(LabeledEdit2->Text));
	delete md5;
	LabeledEdit1->Text = "";
	LabeledEdit2->Text = "";
	simple_ans answer(send_mes(com("autoriz", "select", 0,
		n_w.ToJson()).ToJson()));
	if (answer.key != 0) {
		ShowMessage("Добро пожаловать!");
		GroupBox1->Hide();
		GroupBox3->Show();
		Image1->Visible = 0;
	}
	else
		ShowMessage(answer.result);
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Button8Click(TObject *Sender) {
	reload();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button9Click(TObject *Sender) {
	ShowMessage(simple_ans(send_mes(com("etalon", "insert", 0,
		onPC.ToJson()).ToJson())).result);
	reload();
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Button10Click(TObject *Sender) {
	ShowMessage(simple_ans(send_mes(com("driv", "insert", 0,
		onPC.ToJson()).ToJson())).result);
}

// ---------------------------------------------------------------------------
void TForm1::reload() {
	ListBox1->Clear();
	ListBox2->Clear();
	ListBox3->Clear();

	etalon = listDriv(send_mes(com("etalon", "select", 0, ".").ToJson()));
	onPC = readDrivs();
	notEnough.arr.clear();
	excess.arr.clear();

	set<String>en;
	set<String>nes;
	// список эталона
	for (int i = 0; i < etalon.arr.size(); i++) {
		en.insert(etalon.arr[i].dname);
		ListBox1->Items->Add(etalon.arr[i].dname);
	}
	// список установленных драйверов
	for (int i = 0; i < onPC.arr.size(); i++) {
		nes.insert(onPC.arr[i].dname);
		ListBox2->Items->Add(onPC.arr[i].dname);
	}
	// нехватающие драйвера до эталона
	for (int i = 0; i < etalon.arr.size(); i++) {
		if (nes.count(etalon.arr[i].dname) == 0)
			notEnough.arr.push_back(etalon.arr[i]);
	}
	// лишние драйвера
	for (int i = 0; i < onPC.arr.size(); i++) {
		if (en.count(onPC.arr[i].dname) == 0)
			excess.arr.push_back(onPC.arr[i]);
	}

	ListBox3->Items->Add("Отсутствующие драйвера по эталону:");
	for (int i = 0; i < notEnough.arr.size(); i++) {
		ListBox3->Items->Add(notEnough.arr[i].dname);
	}

	ListBox3->Items->Add("");
	ListBox3->Items->Add("Лишние драйвера по эталону:");
	for (int i = 0; i < excess.arr.size(); i++) {
		ListBox3->Items->Add(excess.arr[i].dname);
	}

	int sr = StrToInt(simple_ans(send_mes(com("driv", "select", 1,
		onPC.ToJson()).ToJson())).result);
	int m = StrToInt(simple_ans(send_mes(com("driv", "select", 2,
		onPC.ToJson()).ToJson())).result);

	StatusBar1->Panels->Items[0]->Text = "Соответствие эталону: " +
		String(etalon.arr.size() - notEnough.arr.size()) + " / " +
		String(etalon.arr.size()) + ", лишних: " + String(excess.arr.size());
	StatusBar1->Panels->Items[1]->Text =
		"Среднее кол-во установленных драйверов на ПК: " + String(sr);
	StatusBar1->Panels->Items[2]->Text =
		"Максимальное кол-во установленных драйверов на ПК: " + String(m);
	StatusBar1->Panels->Items[3]->Text = "Кол-во эталонных драйверов: " +
		String(ListBox1->Count);
	StatusBar1->Panels->Items[4]->Text = "Кол-во загруженных драйверов: " +
		String(ListBox2->Count);
};

void __fastcall TForm1::Button5Click(TObject *Sender) {
	ListBox1->Clear();
	for (int i = 0; i < etalon.arr.size(); i++) {
		if (match_regex(LabeledEdit5->Text, etalon.arr[i].dname) != "")
			ListBox1->Items->Add(etalon.arr[i].dname);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button7Click(TObject *Sender) {
	ListBox2->Clear();
	for (int i = 0; i < onPC.arr.size(); i++) {
		if (match_regex(LabeledEdit6->Text, onPC.arr[i].dname) != "")
			ListBox2->Items->Add(onPC.arr[i].dname);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender) {
	for (int i = 0; i < onPC.arr.size(); i++) {
		ListBox2->Items->Add(onPC.arr[i].dname);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender) {
	for (int i = 0; i < etalon.arr.size(); i++) {
		ListBox1->Items->Add(etalon.arr[i].dname);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender) {
	try {
		send_mes(com("driv", "insert", 0, readDrivs().ToJson()).ToJson());
	}
	catch (...) {
		ShowMessage("Менеджер недоступен!");
		Timer1->Enabled = 0;
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action) {
	try {
		send_mes(com("close", "", 0, "").ToJson());
	}
	catch (...) {
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::IdTCPServer1Execute(TIdContext *AContext) {
	simple_ans ans(AContext->Connection->IOHandler->ReadLn());
	if (ans.result == "start") {
		if (Timer1->Enabled == 1) {
			if (Application->MessageBox(_T("Изменить период?"),
				_T("Запрос от менеджера"),
				MB_YESNO + MB_TOPMOST + MB_ICONQUESTION) == IDYES) {
				// if (MessageBox.Show(,
				// MessageBoxButtons.YesNo) == DialogResult.Yes) {
				simple_ans a("ok", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
			}
			else {
				simple_ans a("no", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
				return;
			}
		}
		else {
			if (Application->MessageBox(_T("Включить монитор?"),
				_T("Запрос от менеджера"),
				MB_YESNO + MB_TOPMOST + MB_ICONQUESTION) == IDYES) {
				// if (MessageBox.Show("Запрос от менеджера", "Включить монитор?",
				// MessageBoxButtons.YesNo) == DialogResult.Yes) {
				simple_ans a("ok", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
			}
			else {
				simple_ans a("no", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
				return;
			}
		}
		Timer1->Enabled = 0;
		Timer1->Interval = 1000 * ans.key;
		Timer1->Enabled = 1;
	}
	if (ans.result == "stop") {
		Timer1->Enabled = 0;
		ShowMessage("Монитор выключен!");
	}

	AContext->Connection->Disconnect();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button11Click(TObject *Sender) {
	if (ListBox3->Items->Count < 2) {
		return;
	}

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

	String othe = "";
	String otch = "";
	int num = 1;
	int it = 1;
	// a == "Отсутствующие драйвера по эталону:"
	for (int i = 0; i < ListBox3->Items->Count; i++) {
		String a = ListBox3->Items->Strings[i];
		if (a == "") {
			continue;
		}
		otch += "\r\n";
		if (a == "Лишние драйвера по эталону:" || i == 0) {
			otch += a;
			continue;
		}
		otch += String(num++) + ".";
		otch += "\t";
		otch += a;
	}

	vVarParagraphs = vVarDoc.OlePropertyGet("Paragraphs");
	vVarParagraphs.OleProcedure("Add");
	vVarParagraph = vVarParagraphs.OleFunction("Item", it++);
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
