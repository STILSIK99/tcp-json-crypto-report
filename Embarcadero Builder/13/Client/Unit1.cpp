// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#include <fstream>

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "IdHashMessageDigest.hpp"
TForm1 *Form1;
using namespace std;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {

}

// ---------------------------------------------------------------------------
String TForm1::send_mes(String request) {
	String ans = "";
	try {
		// ShowMessage(request);
		Client->Connect(Edit1->Text, Edit2->Text.ToInt());
		if (!Client->Connected()) {
			// ShowMessage("Соединение отсутствует!");
			Client->Disconnect();
			return ans;
		}
		Client->IOHandler->WriteLn(request, enUTF8);
		ans = Client->IOHandler->ReadLn(enUTF8);
	}
	catch (...) {
		// ShowMessage("Ошибка сообщения!");
	}
	Client->Disconnect();
	return ans;
};

void __fastcall TForm1::Button2Click(TObject *Sender) {
	try {
		simple_ans answer(send_mes(com("test", ".", 0, ".").ToJson()));
		if (answer.key != 0) {
			ShowMessage("Сервер подключен");
		}
	}
	catch (...) {
		Client->Disconnect();
		ShowMessage("Нет соединения");
		return;
	}
}

// ---------------------------------------------------------------------------

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
	if (answer.key != 0) {
		Form2->Show();
		Form2->signatures =
			ans_Sign(send_mes(com("sign", "select", 0, ".").ToJson())).arr;
		Form2->ComboBox1->Clear();
		for (auto x : Form2->signatures) {
			Form2->ComboBox1->Items->Add(x.name);
		}
	}
	else
		ShowMessage(answer.result);

}

// ---------------------------------------------------------------------------
