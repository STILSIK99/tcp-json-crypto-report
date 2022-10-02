#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Наименование";
	StringGrid1->ColWidths[0] = 50;
	StringGrid1->ColWidths[1] = 300;

	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Наименование";
	StringGrid2->Cells[2][0] = "Вид оружия";
	StringGrid2->Cells[3][0] = "Примечание";
	StringGrid2->ColWidths[0] = 50;
	StringGrid2->ColWidths[1] = 300;
	StringGrid2->ColWidths[2] = 200;
	StringGrid2->ColWidths[3] = 200;

	StringGrid3->Cells[0][0] = "№";
	StringGrid3->Cells[1][0] = "Название оружия";
	StringGrid3->Cells[2][0] = "Вид оружия";
	StringGrid3->Cells[3][0] = "Серия оружия";
	StringGrid3->Cells[4][0] = "Номер оружия";
	StringGrid3->Cells[5][0] = "Примечание к оружию";
	StringGrid3->Cells[6][0] = "Закреплено";
	StringGrid3->ColWidths[0] = 70;
	StringGrid3->ColWidths[1] = 300;
	StringGrid3->ColWidths[2] = 200;
	StringGrid3->ColWidths[3] = 200;
	StringGrid3->ColWidths[4] = 200;
	StringGrid3->ColWidths[5] = 300;
	StringGrid3->ColWidths[6] = 300;

	StringGrid4->Cells[0][0] = "№";
	StringGrid4->Cells[1][0] = "Звание";
	StringGrid4->Cells[2][0] = "ФИО";
	StringGrid4->Cells[3][0] = "Роль";
	StringGrid4->Cells[4][0] = "Примечание";
	StringGrid4->ColWidths[0] = 70;
	StringGrid4->ColWidths[1] = 220;
	StringGrid4->ColWidths[2] = 300;
	StringGrid4->ColWidths[3] = 200;
	StringGrid4->ColWidths[4] = 400;

	StringGrid6->Cells[0][0] = "№";
	StringGrid6->Cells[1][0] = "Дата";
	StringGrid6->Cells[2][0] = "Дежурный";
	StringGrid6->ColWidths[0] = 70;
	StringGrid6->ColWidths[1] = 150;
	StringGrid6->ColWidths[2] = 400;
	DateTimePicker1->Date = Now();
	DateTimePicker2->Date = Now();
	DateTimePicker3->Date = Now();
	DateTimePicker4->Date = Now();
	DateTimePicker5->Date = Now();
}

// ---------------------------------------------------------------------------
void TForm2::up_vid() {
	Myvid = ans_VG(Form1->send_mes(com("vid_gun", "select", 0,
		" ").ToJson())).data;
	sh_vid();
	up_gt();
};

void TForm2::sh_vid() {
	StringGrid1->RowCount = 1;
	ComboBox1->Clear();
	for (auto x : Myvid) {
		int k = StringGrid1->RowCount++;
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = x.name_vid;
		ComboBox1->Items->Add(x.name_vid);
	}
	ComboBox4->Items = ComboBox1->Items;
	ComboBox2->Items = ComboBox1->Items;
};

void TForm2::up_gt() {
	Mygt = ans_GT(Form1->send_mes(com("gun_type", "select", 0,
		" ").ToJson())).data;
	sh_gt();
};

void TForm2::sh_gt() {
	StringGrid2->RowCount = 1;
	ComboBox3->Clear();
	for (auto x : Mygt) {
		int k = StringGrid2->RowCount++;
		StringGrid2->Cells[0][k] = String(k);
		StringGrid2->Cells[1][k] = x.name_gun;
		StringGrid2->Cells[2][k] = x.vid.name_vid;
		StringGrid2->Cells[3][k] = x.info;
		ComboBox3->Items->Add(x.name_gun + " " + x.vid.name_vid);
	}
};

void TForm2::start() {
	up_vid();
	up_gt();
	up_role();
	up_rang();
	up_pers();
	up_gun();
};

void __fastcall TForm2::Button1Click(TObject *Sender) {
	if (SpinEdit1->Value < 1 || SpinEdit1->Value > Myvid.size()) {
		ShowMessage("Неверно задан номер записи!");
		return;
	}
	ShowMessage(simple_ans(Form1->send_mes(com("vid_gun", "delete", 0,
		Myvid[SpinEdit1->Value - 1].ToJson()).ToJson())).result);
	up_vid();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button2Click(TObject *Sender) {
	if (LabeledEdit1->Text == "") {
		ShowMessage("Неверные параметры!");
		return;
	}
	VG a;
	a.name_vid = LabeledEdit1->Text;
	ShowMessage(simple_ans(Form1->send_mes(com("vid_gun", "insert", 0,
		a.ToJson()).ToJson())).result);
	up_vid();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button3Click(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		ShowMessage("Выберите объект изменения!");
		return;
	}
	if (LabeledEdit2->Text == "") {
		ShowMessage("Неверные параметры!");
		return;
	}
	VG a = Myvid[ComboBox1->ItemIndex];
	a.name_vid = LabeledEdit2->Text;
	ShowMessage(simple_ans(Form1->send_mes(com("vid_gun", "update", 0,
		a.ToJson()).ToJson())).result);
	up_vid();
	ComboBox1->ItemIndex = -1;
	LabeledEdit2->Text = "";
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button4Click(TObject *Sender) {
	if (SpinEdit2->Value < 1 || SpinEdit2->Value > Mygt.size()) {
		ShowMessage("Неверно задан номер записи!");
		return;
	}
	ShowMessage(simple_ans(Form1->send_mes(com("gun_type", "delete", 0,
		Mygt[SpinEdit2->Value - 1].ToJson()).ToJson())).result);
	up_gt();
	SpinEdit1->Value = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button6Click(TObject *Sender) {
	if (ComboBox3->ItemIndex == -1) {
		ShowMessage("Выберите объект изменения!");
		return;
	}
	if (ComboBox4->ItemIndex == -1) {
		ShowMessage("Выберите вид оружия!");
		return;
	}
	if (LabeledEdit5->Text == "") {
		ShowMessage("Задайте название!");
		return;
	}
	GT a = Mygt[ComboBox3->ItemIndex];
	a.vid = Myvid[ComboBox4->ItemIndex];
	a.name_gun = LabeledEdit5->Text;
	a.info = LabeledEdit10->Text;
	ShowMessage(simple_ans(Form1->send_mes(com("gun_type", "update", 0,
		a.ToJson()).ToJson())).result);
	up_gt();
	ComboBox3->ItemIndex = -1;
	ComboBox4->ItemIndex = -1;
	LabeledEdit5->Text = "";
	LabeledEdit10->Text = "";
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button5Click(TObject *Sender) {
	if (ComboBox2->ItemIndex == -1) {
		ShowMessage("Выберите вид оружия!");
		return;
	}
	if (LabeledEdit3->Text == "") {
		ShowMessage("Задайте название!");
		return;
	}
	GT a;
	a.vid = Myvid[ComboBox2->ItemIndex];
	a.name_gun = LabeledEdit3->Text;
	a.info = LabeledEdit4->Text;
	ShowMessage(simple_ans(Form1->send_mes(com("gun_type", "insert", 0,
		a.ToJson()).ToJson())).result);
	up_gt();
	ComboBox2->ItemIndex = -1;
	LabeledEdit3->Text = "";
	LabeledEdit4->Text = "";
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::ComboBox1Change(TObject *Sender) {
	if (ComboBox1->ItemIndex != -1) {
		LabeledEdit2->Text = Myvid[ComboBox1->ItemIndex].name_vid;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm2::ComboBox3Change(TObject *Sender) {
	if (ComboBox3->ItemIndex != -1) {
		LabeledEdit5->Text = Mygt[ComboBox3->ItemIndex].name_gun;
		LabeledEdit10->Text = Mygt[ComboBox3->ItemIndex].info;
		for (int i = 0; i < Myvid.size(); i++) {
			if (Myvid[i].id_vid == Mygt[ComboBox3->ItemIndex].vid.id_vid) {
				ComboBox4->ItemIndex = i;
				break;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void TForm2::up_role() {
	Myrole = ans_role(Form1->send_mes(com("role", "select", 0,
		" ").ToJson())).data;
	ComboBox11->Clear();
	for (auto x : Myrole) {
		ComboBox11->Items->Add(x.name_role);
	}
	ComboBox12->Items = ComboBox11->Items;
};

void TForm2::up_rang() {
	Myrang = ans_rang(Form1->send_mes(com("rang", "select", 0,
		" ").ToJson())).data;
	ComboBox5->Clear();
	for (auto x : Myrang) {
		ComboBox5->Items->Add(x.name_rang);
	}
	ComboBox9->Items = ComboBox5->Items;
};

void __fastcall TForm2::Button7Click(TObject *Sender) {
	if (SpinEdit3->Value < 1 || SpinEdit3->Value > Mypers.size()) {
		ShowMessage("Неверно задан номер записи!");
		return;
	}
	ShowMessage(simple_ans(Form1->send_mes(com("personnel", "delete", 0,
		Mypers[SpinEdit3->Value - 1].ToJson()).ToJson())).result);
	up_pers();
	SpinEdit3->Value = 0;
}

// ---------------------------------------------------------------------------
void TForm2::up_pers() {
	Mypers = ans_person(Form1->send_mes(com("personnel", "select", 0,
		" ").ToJson())).data;
	sh_pers();
};

void TForm2::sh_pers() {
	StringGrid4->RowCount = 1;
	ComboBox10->Clear();
	ComboBox14->Clear();
	ComboBox15->Clear();
	Mypers_duty.clear();
	for (auto x : Mypers) {
		int k = StringGrid4->RowCount++;
		StringGrid4->Cells[0][k] = String(k);
		StringGrid4->Cells[1][k] = x.ra.name_rang;
		StringGrid4->Cells[2][k] = x.fio;
		StringGrid4->Cells[3][k] = x.ro.name_role;
		StringGrid4->Cells[4][k] = x.info;
		ComboBox10->Items->Add(x.ra.name_rang + " " + x.fio);
		if (x.ro.id_role == 2) {
			ComboBox14->Items->Add(x.ra.name_rang + " " + x.fio);
			ComboBox15->Items->Add(x.ra.name_rang + " " + x.fio);
			Mypers_duty.push_back(x);
		}
	}
	ComboBox8->Items = ComboBox10->Items;
};

void __fastcall TForm2::Button8Click(TObject *Sender) {
	if (ComboBox5->ItemIndex == -1) {
		ShowMessage("Выберите звание!");
		return;
	}
	if (ComboBox11->ItemIndex == -1) {
		ShowMessage("Выберите роль!");
		return;
	}
	if (LabeledEdit6->Text == "") {
		ShowMessage("Введите ФИО военнослужащего!");
		return;
	}
	person a;
	a.fio = LabeledEdit6->Text;
	a.ro = Myrole[ComboBox11->ItemIndex];
	a.ra = Myrang[ComboBox5->ItemIndex];
	a.info = LabeledEdit7->Text;
	ShowMessage(simple_ans(Form1->send_mes(com("personnel", "insert", 0,
		a.ToJson()).ToJson())).result);
	up_pers();
	ComboBox5->ItemIndex = -1;
	ComboBox11->ItemIndex = -1;
	LabeledEdit6->Text = "";
	LabeledEdit7->Text = "";
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::ComboBox10Change(TObject *Sender) {
	if (ComboBox10->ItemIndex != -1) {
		person a = Mypers[ComboBox10->ItemIndex];
		LabeledEdit8->Text = a.fio;
		LabeledEdit9->Text = a.info;
		for (int i = 0; i < Myrole.size(); i++) {
			if (a.ro.id_role == Myrole[i].id_role) {
				ComboBox12->ItemIndex = i;
				break;
			}
		}
		for (int i = 0; i < Myrang.size(); i++) {
			if (a.ra.id_rang == Myrang[i].id_rang) {
				ComboBox9->ItemIndex = i;
				break;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button13Click(TObject *Sender) {
	TDate d1, d2;
	d1 = DateTimePicker1->Date;
	d2 = DateTimePicker2->Date;
	if (d1 > d2)
		swap(d1, d2);
	two_date a;
	a.date1 = d1;
	a.date2 = d2;
	Myduty = ans_duty(Form1->send_mes(com("duty", "select", 1,
		a.ToJson()).ToJson())).data;
	StringGrid6->RowCount = 1;
	for (auto x : Myduty) {
		int k = StringGrid6->RowCount++;
		StringGrid6->Cells[0][k] = String(k);
		StringGrid6->Cells[1][k] = x.date_d;
		StringGrid6->Cells[2][k] = x.dej.ra.name_rang + " " + x.dej.fio;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button10Click(TObject *Sender) {
	if (ComboBox10->ItemIndex == -1) {
		ShowMessage("Выберите объект изменения!");
		return;
	}
	if (ComboBox9->ItemIndex == -1) {
		ShowMessage("Выберите звание!");
		return;
	}
	if (ComboBox12->ItemIndex == -1) {
		ShowMessage("Выберите роль!");
		return;
	}
	if (LabeledEdit8->Text == "") {
		ShowMessage("Задайте название!");
		return;
	}
	person a = Mypers[ComboBox10->ItemIndex];
	a.ra = Myrang[ComboBox9->ItemIndex];
	a.ro = Myrole[ComboBox12->ItemIndex];
	a.fio = LabeledEdit8->Text;
	a.info = LabeledEdit9->Text;
	ShowMessage(simple_ans(Form1->send_mes(com("personnel", "update", 0,
		a.ToJson()).ToJson())).result);
	up_pers();
	ComboBox9->ItemIndex = -1;
	ComboBox10->ItemIndex = -1;
	ComboBox12->ItemIndex = -1;
	LabeledEdit8->Text = "";
	LabeledEdit9->Text = "";
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button16Click(TObject *Sender) {
	if (ComboBox15->ItemIndex == -1) {
		ShowMessage("Выберите военнослужащего!");
		return;
	}
	duty a;
	a.date_d = DateTimePicker3->Date.DateString();
	a.dej = Mypers_duty[ComboBox15->ItemIndex];
	ShowMessage(simple_ans(Form1->send_mes(com("duty", "insert", 0,
		a.ToJson()).ToJson())).result);
	Button13Click(Form2);
	ComboBox15->ItemIndex = -1;
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button15Click(TObject *Sender) {
	if (ComboBox14->ItemIndex == -1) {
		ShowMessage("Выберите военнослужащего!");
		return;
	}
	duty a;
	a.date_d = DateTimePicker4->Date.DateString();
	a.dej = Mypers_duty[ComboBox14->ItemIndex];
	ShowMessage(simple_ans(Form1->send_mes(com("duty", "update", 0,
		a.ToJson()).ToJson())).result);
	Button13Click(Form2);
	ComboBox14->ItemIndex = -1;
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button14Click(TObject *Sender) {
	duty a;
	a.date_d = DateTimePicker5->Date.DateString();
	ShowMessage(simple_ans(Form1->send_mes(com("duty", "delete", 0,
		a.ToJson()).ToJson())).result);
	Button13Click(Form2);
};

void TForm2::up_gun() {
	for (int j = 1; j < StringGrid3->RowCount; j++) {
		for (int i = 0; i < StringGrid3->ColCount; i++) {
			StringGrid3->Cells[i][j] = "";
		}
	}
	StringGrid3->RowCount = 1;
	up_gun_zak();
	up_gun_nozak();
};

void __fastcall TForm2::Button11Click(TObject *Sender) {
	if (ComboBox7->ItemIndex == -1) {
		ShowMessage("Выберите оружие!");
		return;
	}
	if (ComboBox8->ItemIndex == -1) {
		ShowMessage("Выберите военнослужащего!");
		return;
	}
	zakrep a;
	a.ls = Mypers[ComboBox8->ItemIndex];
	a.g = Mygun_nozak[ComboBox7->ItemIndex];
	ShowMessage(simple_ans(Form1->send_mes(com("zakreplenie", "insert", 0,
		a.ToJson()).ToJson())).result);
	up_gun();
	ComboBox7->ItemIndex = -1;
	ComboBox8->ItemIndex = -1;
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button12Click(TObject *Sender) {
	if (ComboBox13->ItemIndex == -1) {
		ShowMessage("Выберите военнослужащего!");
		return;
	}
	ShowMessage(simple_ans(Form1->send_mes(com("zakreplenie", "delete", 0,
		Mygun_zak[ComboBox13->ItemIndex].ToJson()).ToJson())).result);
	up_gun();
	ComboBox13->ItemIndex = -1;
}

// ---------------------------------------------------------------------------
void TForm2::up_gun_zak() {
	Mygun_zak = ans_zak(Form1->send_mes(com("gun", "select", 2,
		" ").ToJson())).data;
	sh_gun_zak();
};

void TForm2::sh_gun_zak() {
	ComboBox13->Clear();
	for (auto x : Mygun_zak) {
		int k = StringGrid3->RowCount++;
		StringGrid3->Cells[0][k] = String(k);
		StringGrid3->Cells[1][k] = x.g.type.name_gun;
		StringGrid3->Cells[2][k] = x.g.type.vid.name_vid;
		StringGrid3->Cells[3][k] = x.g.serial;
		StringGrid3->Cells[4][k] = x.g.num;
		StringGrid3->Cells[5][k] = x.g.info;
		StringGrid3->Cells[6][k] = x.ls.ra.name_rang + " " + x.ls.fio;
		ComboBox13->Items->Add(x.g.type.name_gun + ": " + x.g.serial + " " +
			x.g.num + " --- " + x.ls.ra.name_rang + " " + x.ls.fio);
	}
};

void TForm2::up_gun_nozak() {
	Mygun_nozak =
		ans_gun(Form1->send_mes(com("gun", "select", 1, " ").ToJson())).data;
	sh_gun_nozak();
};

void TForm2::sh_gun_nozak() {
	ComboBox7->Clear();
	for (auto x : Mygun_nozak) {
		int k = StringGrid3->RowCount++;
		StringGrid3->Cells[0][k] = String(k);
		StringGrid3->Cells[1][k] = x.type.name_gun;
		StringGrid3->Cells[2][k] = x.type.vid.name_vid;
		StringGrid3->Cells[3][k] = x.serial;
		StringGrid3->Cells[4][k] = x.num;
		StringGrid3->Cells[5][k] = x.info;
		ComboBox7->Items->Add(x.type.name_gun + ": " + x.serial + " " + x.num);
	}
};
