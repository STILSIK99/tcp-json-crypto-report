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
	cryptoSession = new CryptoSession("driverpack");
}

// ---------------------------------------------------------------------------
String TForm1::send_mes(String request) {
	try {
		// ShowMessage(request);
		Client->Connect(Edit1->Text, Edit2->Text.ToInt());
		if (!Client->Connected()) {
			ShowMessage("���������� �����������!");
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
	login n_w;
	n_w.log = LabeledEdit1->Text;
	TIdHashMessageDigest5* md5 = new TIdHashMessageDigest5;
	n_w.hash = md5->HashStringAsHex(LabeledEdit1->Text + md5->HashStringAsHex
		(LabeledEdit2->Text));
	delete md5;
	LabeledEdit1->Text = "";
	LabeledEdit2->Text = "";
	simple_ans answer(send_mes(com("polz", "select", 0,
		n_w.ToJson()).ToJson()));
	if (answer.key != 0) {
		ShowMessage("����� ����������!");
		Form2->load();
		Form2->Show();
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
		ShowMessage("��� ����������!");
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	Form2->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::IdTCPServer1Execute(TIdContext *AContext) {
	simple_ans ans(AContext->Connection->IOHandler->ReadLn());

	if (ans.result == "turn_on") {
		if (Timer1->Enabled == 0) {
			if (Application->MessageBox(_T("�������� ���� ������?"),
				_T("������ �� ���������"),
				MB_YESNO + MB_TOPMOST + MB_ICONQUESTION) == IDYES) {
				// if (MessageBox.Show("������ �� ���������", "�������� �������?",
				// MessageBoxButtons.YesNo) == DialogResult.Yes) {
				simple_ans a("yes", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
			}
			else {
				simple_ans a("no", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
				return;
			}
		}
		else {
			if (Application->MessageBox(_T("�������� ���� ������?"),
				_T("������ �� ���������"),
				MB_YESNO + MB_TOPMOST + MB_ICONQUESTION) == IDYES) {
				// if (MessageBox.Show("������ �� ���������", "�������� �������?",
				// MessageBoxButtons.YesNo) == DialogResult.Yes) {
				simple_ans a("yes", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
			}
			else {
				simple_ans a("no", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
				return;
			}
		}
	}
	Timer1->Enabled = 0;
	Timer1->Interval = 1000 * ans.key;
	Timer1->Enabled = 1;

	if (ans.result == "turn_off") {
		Timer1->Enabled = 0;
		ShowMessage("���� ������ ��������");
	}

	AContext->Connection->Disconnect();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender) {
	Drivers my;
	my.arr = get_drivers();
	send_mes(com("drivers", "insert", 0, my.ToJson()).ToJson());

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action) {
	send_mes(com("close", "", 0, "").ToJson());
}
// ---------------------------------------------------------------------------
