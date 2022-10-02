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
	cryptoSession = new CryptoSession("watcher");
	userName = "";
	access = 0;
}

// ---------------------------------------------------------------------------
String TForm1::send_mes(String request) {
	try {
		// ShowMessage(request);
		Client->Connect(Edit1->Text, Edit2->Text.ToInt());
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
		ShowMessage("Ошибка сообщения!");
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
	login n_w;
	n_w.log = LabeledEdit1->Text;
	TIdHashMessageDigest5* md5 = new TIdHashMessageDigest5;
	n_w.hash = md5->HashStringAsHex(LabeledEdit1->Text + md5->HashStringAsHex
		(LabeledEdit2->Text));
	delete md5;
	LabeledEdit1->Text = "";
	LabeledEdit2->Text = "";
	simple_ans answer(send_mes(com("login", "select", 0,
		n_w.ToJson()).ToJson()));
	User user(answer.result);
	if (answer.key != -1) {
		ShowMessage("Добро пожаловать!");
		access = answer.key;
		userName = n_w.log;
		if (Form4->access(user.access) == 1) {
			Form4->loadData(0);
			Form4->Show();
		}
		else {
            Form2->clear();
			Form2->Show();
			Form2->loadData();
		}
	}
	else
		ShowMessage(answer.result);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender) {
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
