// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit3.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;

// ---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner) : TForm(Owner) {
}

// ---------------------------------------------------------------------------
void TForm3::loadData() {
	ListBox1->Clear();
	ListBox1->Items->Add("Выберите файл для просмотра двойным кликом:");
	ListBox2->Items = ListBox1->Items;
	history = ans_History(Form1->send_mes(com("history", "select", 1,
		Form1->userName).ToJson())).arr;
	// history
	for (int i = 0; i < history.size(); i++) {
		ListBox2->Items->Add(history[i].datetime + " - " + history[i].fileName);
	}
	// files
	files = ans_Files(Form1->send_mes(com("files", "select", 0,
		"").ToJson())).arr;
	for (int i = 0; i < files.size(); i++) {
		ListBox1->Items->Add(files[i].date + " - " + files[i].name);
	}
};

void __fastcall TForm3::ListBox2DblClick(TObject *Sender) {
	if (ListBox2->ItemIndex < 1) {
		return;
	}
	Form2->file =
		File(simple_ans(Form1->send_mes(com("files", "select", 2,
		history[ListBox2->ItemIndex - 1].fileName).ToJson())).result);
	send_history();
	Form2->loadData();
	Form3->Hide();
}
// ---------------------------------------------------------------------------

void __fastcall TForm3::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex < 1) {
		return;
	}
	Form2->file =
		File(simple_ans(Form1->send_mes(com("files", "select", 1,
		String(files[ListBox1->ItemIndex - 1].id)).ToJson())).result);
	send_history();
	Form2->loadData();
	Form3->Hide();
}

// ---------------------------------------------------------------------------
void TForm3::send_history() {
	History h;
	h.userName = Form1->userName;
	h.fileName = Form2->file.name;
	Form1->send_mes(com("history", "insert", 0, h.ToJson()).ToJson());
};
