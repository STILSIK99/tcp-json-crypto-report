// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include <fstream>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm4 *Form4;

// ---------------------------------------------------------------------------
__fastcall TForm4::TForm4(TComponent* Owner) : TForm(Owner) {
	idUser = -1;
	DatePicker1->Date = StrToDate(DateToStr(Now()));
}

// ---------------------------------------------------------------------------
void TForm4::loadData(int histOld) {
	clear();
	// users
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Логин";
	StringGrid1->Cells[2][0] = "Пароль";
	StringGrid1->Cells[3][0] = "Права доступа";
	users = ans_Users(Form1->send_mes(com("users", "select", 0,
		"").ToJson())).arr;
	StringGrid1->RowCount = users.size() + 1;
	for (int i = 0; i < users.size(); i++) {
		StringGrid1->Cells[0][i + 1] = String(i + 1);
		StringGrid1->Cells[1][i + 1] = users[i].name;
		StringGrid1->Cells[2][i + 1] = users[i].pash;
		StringGrid1->Cells[3][i + 1] = users[i].access;
		ComboBox2->Items->Add(users[i].name);
	}
	ComboBox6->Items = ComboBox2->Items;
	// files
	files = ans_Files(Form1->send_mes(com("files", "select", 0,
		"").ToJson())).arr;
	for (int i = 0; i < files.size(); i++) {
		ComboBox5->Items->Add(files[i].name);
		ListBox1->Items->Add(files[i].date + " - " + files[i].name);
	}
	ComboBox3->Items = ComboBox5->Items;
	// history
	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Файл";
	StringGrid2->Cells[2][0] = "Пользователь";
	StringGrid2->Cells[3][0] = "Время доступа";
	if (histOld != 1) {
		history = ans_History(Form1->send_mes(com("history", "select", 0,
			"").ToJson())).arr;
	}
	StringGrid2->RowCount = history.size() + 1;
	for (int i = 0; i < history.size(); i++) {
		StringGrid2->Cells[0][i + 1] = String(i + 1);
		StringGrid2->Cells[1][i + 1] = history[i].fileName;
		StringGrid2->Cells[2][i + 1] = history[i].userName;
		StringGrid2->Cells[3][i + 1] = history[i].datetime;
	}

};

void TForm4::clear() {
	for (int i = 0; i < StringGrid1->RowCount; i++) {
		for (int j = 0; j < StringGrid1->ColCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	for (int i = 0; i < StringGrid2->RowCount; i++) {
		for (int j = 0; j < StringGrid2->ColCount; j++) {
			StringGrid2->Cells[i][j] = "";
		}
	}
	ComboBox2->Clear();
	ComboBox3->Clear();
	ComboBox5->Clear();
	ComboBox6->Clear();
	Edit1->Text = "";
	LabeledEdit1->Text = "";
	LabeledEdit2->Text = "";
	LabeledEdit3->Text = "";
	ListBox1->Clear();
	Memo1->Clear();
	StringGrid1->RowCount = 0;
	StringGrid2->RowCount = 0;
};

void __fastcall TForm4::Button1Click(TObject *Sender) {
	if (LabeledEdit1->Text != "" && LabeledEdit2->Text != "" &&
		ComboBox1->ItemIndex >= 0) {
		TIdHashMessageDigest5* md5 = new TIdHashMessageDigest5;
		ShowMessage(simple_ans(Form1->send_mes(com("users", "insert", 0,
			User(0, LabeledEdit1->Text,
			md5->HashStringAsHex(LabeledEdit1->Text + md5->HashStringAsHex
			(LabeledEdit2->Text)), ComboBox1->Text).ToJson()).ToJson()))
			.result);
		delete md5;
		loadData(0);
	}
	else {
		ShowMessage("Неверные данные!");
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm4::Button2Click(TObject *Sender) {
	if (ComboBox2->ItemIndex < 0) {
		ShowMessage("Выберите пользователя!");
	}
	else {
		idUser = ComboBox2->ItemIndex;
		Edit1->Text = users[idUser].name;
		ComboBox4->ItemIndex = access(users[idUser].access);
		LabeledEdit3->Text = "";
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm4::Button3Click(TObject *Sender) {
	if (idUser < 0) {
		ShowMessage("Выберите пользователя!");
		return;
	}
	User update = users[idUser];
	update.access = access(ComboBox4->ItemIndex);
	TIdHashMessageDigest5* md5 = new TIdHashMessageDigest5;
	update.pash = md5->HashStringAsHex
		(Edit1->Text + md5->HashStringAsHex(LabeledEdit3->Text));
	delete md5;
	ShowMessage(simple_ans(Form1->send_mes(com("users", "update", 0,
		update.ToJson()).ToJson())).result);
	loadData(0);
}

// ---------------------------------------------------------------------------
String TForm4::access(int a) {
	if (a == 1)
		return "Администратор";
	else
		return "Пользователь";
};

int TForm4::access(String a) {
	if (a == "Администратор")
		return 1;
	else
		return 0;
};

void __fastcall TForm4::Button5Click(TObject *Sender) {
	if (!NewTxtFile->Execute()) {
		return;
	}
	String text = loadFile(NewTxtFile->FileName);
	File nFile(0, text, cutFileName(NewTxtFile->FileName), "");
	ShowMessage(simple_ans(Form1->send_mes(com("files", "insert", 0,
		nFile.ToJson()).ToJson())).result);
	loadData(0);
}

// ---------------------------------------------------------------------------
void __fastcall TForm4::Button6Click(TObject *Sender) {
	if (!NewTxtFile->Execute()) {
		return;
	}
	if (ComboBox3->ItemIndex < 0) {
		ShowMessage("Выберите файл!");
		return;
	}
	String text = loadFile(NewTxtFile->FileName);
	File nFile(files[ComboBox3->ItemIndex].id, text,
		files[ComboBox3->ItemIndex].name, "");
	ShowMessage(simple_ans(Form1->send_mes(com("files", "update", 0,
		nFile.ToJson()).ToJson())).result);
	loadData(0);
}

// ---------------------------------------------------------------------------
void __fastcall TForm4::Button4Click(TObject *Sender) {
	ans_Files req;
	for (int i = 0; i < ListBox1->Count; i++) {
		if (ListBox1->Selected[i]) {
			req.arr.push_back(files[i]);
		}
	}
	ShowMessage(simple_ans(Form1->send_mes(com("files", "delete", 0,
		req.ToJson()).ToJson())).result);
	loadData(0);

}

// ---------------------------------------------------------------------------
void __fastcall TForm4::ListBox1DblClick(TObject *Sender) {
	Memo1->Text =
		File(simple_ans(Form1->send_mes(com("files", "select", 1,
		String(files[ListBox1->ItemIndex].id)).ToJson())).result).source;
}
// ---------------------------------------------------------------------------

void __fastcall TForm4::Button9Click(TObject *Sender) {
	sorting s;
	if (ComboBox5->ItemIndex != -1) {
		s.data = 1;
		s.ff = ComboBox5->Text;
	}
	if (ComboBox6->ItemIndex != -1) {
		s.data1 = 1;
		s.ss = ComboBox6->Text;
	}
	if (CheckBox1->Checked) {
		s.date = 1;
		s.date1 = DateToStr(DatePicker1->Date);
		s.date2 = DateToStr(DatePicker2->Date);
	}
	history = ans_History(Form1->send_mes(com("history", "select", 2,
		s.ToJson()).ToJson())).arr;
	loadData(1);
}
// ---------------------------------------------------------------------------

String TForm4::cutFileName(String str) {
	String ans = "";
	for (int i = str.Length(); i > 0; i--) {
		if (str[i] == '\\') {
			return ans;
		}
		ans = str.SubString(i, 1) + ans;
	}
	return ans;
};

void __fastcall TForm4::Button7Click(TObject *Sender) {
	if (ComboBox2->ItemIndex == -1) {
		ShowMessage("Выберите пользователя!");
		return;
	}
	ShowMessage(simple_ans(Form1->send_mes(com("users", "delete", 0,
		users[ComboBox2->ItemIndex].ToJson()).ToJson())).result);
	loadData(0);
}

// ---------------------------------------------------------------------------
void __fastcall TForm4::DatePicker1Change(TObject *Sender) {
	if (DatePicker1->Date > DatePicker2->Date) {
		DatePicker2->Date = DatePicker1->Date;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm4::DatePicker2Change(TObject *Sender) {
	if (DatePicker1->Date > DatePicker2->Date) {
		DatePicker2->Date = DatePicker1->Date;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm4::Button8Click(TObject *Sender) {
	loadData(0);
}

// ---------------------------------------------------------------------------
String TForm4::loadFile(String fileName) {
	string text = "";
	string s;
	ifstream file(fileName.c_str());
	while (getline(file, s)) {
		text += s + "\n";
	}
	return String(AnsiString(text.c_str()));
};

void __fastcall TForm4::FormClose(TObject *Sender, TCloseAction &Action) {
	Form1->send_mes(com("close", "", 0, "").ToJson());
}
// ---------------------------------------------------------------------------
