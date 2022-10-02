// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
#include "File2.h"

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	flag = 0;
	id = -1;
	ComboBox1->Clear();
	current = create_otchet();
	for (int i = 0; i < current->arr.size(); i++) {
		ComboBox1->Items->Add(current->arr[i].name);
	}
	all.push_back(current);

}
// ---------------------------------------------------------------------------

void __fastcall TForm2::Timer1Timer(TObject *Sender) {
	current = create_otchet();
	ComboBox1->Clear();
	int fl = 0;
	for (int i = 0; i < current->arr.size(); i++) {
		ComboBox1->Items->Add(current->arr[i].name);
	}

	all.push_back(current);
	if (flag) {
		Form1->send_mes(com("disk", "insert", 0, current->ToJson()).ToJson());
	}
	load();
}

// ---------------------------------------------------------------------------
void TForm2::load() {
	Series1->Clear();
	Series2->Clear();
	Chart1->Title->Text->Clear();
	if (id == -1) {
		return;
	}
	Chart1->Title->Text->Add(all[0]->arr[id].name);
	for (int i = 0; i < all.size(); i++) {
		Series2->AddXY(i*3, all[i]->arr[id].occup);
		Series1->AddXY(i*3, all[i]->arr[id].total);
	}

};

void TForm2::clear() {
	id = -1;
	Chart1->Title->Text->Clear();
	ComboBox1->ItemIndex = -1;
};

void __fastcall TForm2::ComboBox1Change(TObject *Sender) {
	id = ComboBox1->ItemIndex;
	ComboBox1->ItemIndex = -1;
	load();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::FormClose(TObject *Sender, TCloseAction &Action) {
	Form1->send_mes(com("close", "insert", 0, current->ToJson()).ToJson());
	flag = 0;
}
// ---------------------------------------------------------------------------
