// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	diskIndex = -1;
	per = 0;
	Series1->Title = "Свободное место";
	Series2->Title = "Занято";
	Series3->Title = "Ограничение";
}

// ---------------------------------------------------------------------------
void TForm2::loadData() {
	ListBox1->Clear();
	vols = get_disks_info();
	vols->time = IntToStr(per);
	Memo1->Text = vols->ToJson();
	for (int i = 0; i < vols->arr.size(); i++) {
		ListBox1->Items->Add(vols->arr[i].name);
	}
	data.push_back(vols);
	if (diskIndex != -1) {
		for (int i = 0; i < data.size(); i++) {
			Series1->AddXY(StrToInt(data[i]->time),
				data[i]->arr[diskIndex].fr.ToDouble());
			Series2->AddXY(StrToInt(data[i]->time),
				data[i]->arr[diskIndex].busy.ToDouble());
		}
	}
}

void __fastcall TForm2::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		return;
	}
	diskIndex = ListBox1->ItemIndex;
	Chart1->Title->Text->Clear();
	Chart1->Title->Text->Add(vols->arr[diskIndex].name);
	Series1->Clear();
	Series2->Clear();
	if (data.size() == 0) {
		return;
	}
	if (diskIndex == -1) {
		return;
	}
	for (int i = 0; i < data.size(); i++) {
		Series1->AddXY(StrToInt(data[i]->time),
			data[i]->arr[diskIndex].fr.ToDouble());
		Series2->AddXY(StrToInt(data[i]->time),
			data[i]->arr[diskIndex].busy.ToDouble());
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Timer1Timer(TObject *Sender) {
	try {
		per += 5;
		loadData();
		Form1->send_mes(com("info", "insert", 0, vols->ToJson()).ToJson());
		if (per % 60 == 0) {
			Series1->Clear();
			Series2->Clear();
			Series3->Clear();
		}
	}
	catch (...) {
		Timer1->Enabled = 0;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::ListBox1Exit(TObject *Sender) {
	Form1->send_mes(com("close", "insert", 0, "").ToJson());
}

// ---------------------------------------------------------------------------
