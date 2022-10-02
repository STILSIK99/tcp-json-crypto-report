// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "IdHashMessageDigest.hpp"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	cryptoSession = new CryptoSession("driveInfo");
}

// ---------------------------------------------------------------------------
String TForm1::send_mes(String request) {
	try {
		// ShowMessage(request);
		Client->Connect(Edit1->Text, Edit2->Text.ToInt());
		if (!Client->Connected()) {
			ShowMessage("Соединение отсутствует!");
			Form2->Timer1->Enabled = 0;
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

void __fastcall TForm1::Button1Click(TObject *Sender) {
	// Form2->loadData();
	// Form2->Show();
	// return;

	TIdHashMessageDigest5* md5 = new TIdHashMessageDigest5;
	simple_ans answer(send_mes(com("user", "select", 0,
		login(LabeledEdit1->Text, md5->HashStringAsHex(LabeledEdit1->Text +
		md5->HashStringAsHex(LabeledEdit2->Text))).ToJson()).ToJson()));
	// Form2->Memo1->Text += login(LabeledEdit1->Text,
	// md5->HashStringAsHex(LabeledEdit1->Text + md5->HashStringAsHex
	// (LabeledEdit2->Text))).ToJson();
	delete md5;
	LabeledEdit1->Text = "";
	LabeledEdit2->Text = "";
	if (answer.key != 0) {
		ShowMessage("Добро пожаловать!");
		Form2->loadData();
		Form2->Show();
	}
	else
		ShowMessage(answer.result);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject * Sender) {
	try {
		simple_ans answer(send_mes(com("test", "", 0, "").ToJson()));
		ShowMessage(answer.result);
	}
	catch (...) {
		Client->Disconnect();
		ShowMessage("Нет соединения!");
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::IdTCPServer1Execute(TIdContext * AContext) {
	simple_ans ans(AContext->Connection->IOHandler->ReadLn());
	if (ans.result == "on") {
		if (Application->MessageBox(_T("Включить сбор данных?"),
			_T("Запрос от менеджера"), MB_YESNO + MB_TOPMOST + MB_ICONQUESTION)
			== IDYES) {
			simple_ans a("yes", 0);
			AContext->Connection->IOHandler->WriteLn(a.ToJson());
		}
		else {
			simple_ans a("no", 0);
			AContext->Connection->IOHandler->WriteLn(a.ToJson());
			AContext->Connection->Disconnect();
			return;
		}
	}
	Form1->send_mes(com("drives", "insert", 0, Form2->data.ToJson()).ToJson());

	Form2->flag = 1;

	if (ans.result == "off") {
		Form2->flag = 0;
		ShowMessage("Сбор данных выключен");
	}

	AContext->Connection->Disconnect();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject * Sender) {
	Form2->Show();
}
// ---------------------------------------------------------------------------
