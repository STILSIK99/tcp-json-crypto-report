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
	DateTimePicker1->Date = Now();
	DateTimePicker2->Date = Now();
	DateTimePicker3->Date = Now();
	DateTimePicker4->Date = Now();
	DateTimePicker5->Date = Now();
	DateTimePicker6->Date = Now();
	DateTimePicker7->Date = Now();
	DateTimePicker8->Date = Now();

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Наименование";
	StringGrid1->Cells[2][0] = "Военнослужащий";
	StringGrid1->Cells[3][0] = "Дежурный по курсу";
	StringGrid1->Cells[4][0] = "Дата выдачи";
	StringGrid1->ColWidths[0] = 70;
	StringGrid1->ColWidths[1] = 300;
	StringGrid1->ColWidths[2] = 300;
	StringGrid1->ColWidths[3] = 300;
	StringGrid1->ColWidths[4] = 120;

	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Наименование";
	StringGrid2->Cells[2][0] = "Военнослужащий";
	StringGrid2->Cells[3][0] = "Дежурный по курсу";
	StringGrid2->Cells[4][0] = "Дата приёма";
	StringGrid2->ColWidths[0] = 70;
	StringGrid2->ColWidths[1] = 300;
	StringGrid2->ColWidths[2] = 300;
	StringGrid2->ColWidths[3] = 300;
	StringGrid2->ColWidths[4] = 120;

	StringGrid3->Cells[0][0] = "№";
	StringGrid3->Cells[1][0] = "Наименование";
	StringGrid3->Cells[2][0] = "Вид оружия";
	StringGrid3->Cells[3][0] = "Серия";
	StringGrid3->Cells[4][0] = "Номер";
	StringGrid3->Cells[5][0] = "Дежурный по курсу";
	StringGrid3->Cells[6][0] = "Дата сдачи";
	// StringGrid3->Cells[7][0] = "Примечание";
	StringGrid3->ColWidths[0] = 70;
	StringGrid3->ColWidths[1] = 300;
	StringGrid3->ColWidths[2] = 150;
	StringGrid3->ColWidths[3] = 200;
	StringGrid3->ColWidths[4] = 200;
	StringGrid3->ColWidths[5] = 300;
	StringGrid3->ColWidths[6] = 120;
	// StringGrid3->ColWidths[7] = 200;

	StringGrid4->Cells[0][0] = "№";
	StringGrid4->Cells[1][0] = "Наименование";
	StringGrid4->Cells[2][0] = "Вид оружия";
	StringGrid4->Cells[3][0] = "Серия";
	StringGrid4->Cells[4][0] = "Номер";
	StringGrid4->Cells[5][0] = "Дежурный по курсу";
	StringGrid4->Cells[6][0] = "Дата приема";
	StringGrid4->Cells[7][0] = "Примечание";
	StringGrid4->ColWidths[0] = 70;
	StringGrid4->ColWidths[1] = 300;
	StringGrid4->ColWidths[2] = 250;
	StringGrid4->ColWidths[3] = 200;
	StringGrid4->ColWidths[4] = 200;
	StringGrid4->ColWidths[5] = 300;
	StringGrid4->ColWidths[6] = 120;
	StringGrid4->ColWidths[7] = 300;

	StringGrid5->Cells[0][0] = "№";
	StringGrid5->Cells[1][0] = "Серия";
	StringGrid5->Cells[2][0] = "Номер";
	StringGrid5->ColWidths[0] = 70;
	StringGrid5->ColWidths[1] = 200;
	StringGrid5->ColWidths[2] = 200;

	StringGrid6->Cells[0][0] = "№";
	StringGrid6->Cells[1][0] = "Наименование";
	StringGrid6->Cells[2][0] = "Вид оружия";
	StringGrid6->Cells[3][0] = "Серия";
	StringGrid6->Cells[4][0] = "Номер";
	StringGrid6->ColWidths[0] = 70;
	StringGrid6->ColWidths[1] = 300;
	StringGrid6->ColWidths[2] = 250;
	StringGrid6->ColWidths[3] = 200;
	StringGrid6->ColWidths[4] = 200;
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button9Click(TObject *Sender) {
	for (int i = 1; i < StringGrid5->RowCount; i++) {
		if (!check(String(StringGrid5->Cells[1][i]), 1)) {
			ShowMessage("Ошибка серии в строке " + String(i + 1));
			return;
		}
		if (!check(StringGrid5->Cells[2][i], 2)) {
			ShowMessage("Ошибка номера в строке " + String(i + 1));
			return;
		}
	}
	ans_gun ans;
	for (int i = 1; i < StringGrid5->RowCount; i++) {
		gun a;
		a.type = Mygt[ComboBox6->ItemIndex];
		a.num = StringGrid5->Cells[2][i];
		a.serial = StringGrid5->Cells[1][i];
		ans.data.push_back(a);
	}
	ShowMessage(simple_ans(Form1->send_mes(com("gun", "insert", 0,
		ans.ToJson()).ToJson())).result);
	for (int i = 1; i < StringGrid5->ColCount; i++) {
		for (int j = 1; j < StringGrid5->RowCount; j++) {
			StringGrid5->Cells[j][i] = "";
		}
	}
	StringGrid5->RowCount = 1;
	SpinEdit4->Value = 0;
	ComboBox6->ItemIndex = -1;
	start();

}
// ---------------------------------------------------------------------------

void __fastcall TForm3::ComboBox6Change(TObject *Sender) {
	for (int i = 1; i < StringGrid5->ColCount; i++) {
		for (int j = 1; j < StringGrid5->RowCount; j++) {
			StringGrid5->Cells[i][j] = "";
		}
	}
	StringGrid5->RowCount = 1;
	for (int i = 1; i <= SpinEdit4->Value; i++) {
		StringGrid5->Cells[0][i] = String(i);
		StringGrid5->RowCount++;
	}
	if (StringGrid5->RowCount > 1) {
		StringGrid5->FixedRows = 1;
	}
}

// ---------------------------------------------------------------------------
void TForm3::up_gt() {
	Mygt = ans_GT(Form1->send_mes(com("gun_type", "select", 0,
		" ").ToJson())).data;
	sh_gt();
};

void TForm3::sh_gt() {
	ComboBox6->Clear();
	for (auto x : Mygt) {
		ComboBox6->Items->Add(x.name_gun + " " + x.vid.name_vid);
	}
};

bool TForm3::check(String a, int type) {
	String a1 = "а", a2 = "я", b1 = "А", b2 = "Я", c1 = "0", c2 = "9";
	if (type == 1 && a.Length() <= 10) { // проверка серии
		for (int i = 1; i < a.Length(); i++) {
			if ((a[i] >= a1[1] && a[i] <= a2[1]) ||
				((a[i] >= b1[1] && a[i] <= b2[1]))) {
				continue;
			}
			return false;

		}
		return true;
	}
	if (type == 2 && a.Length() <= 10) { // проверка номера
		for (int i = 1; i < a.Length(); i++) {
			if (a[i] >= c1[1] && a[i] <= c2[1]) {
				continue;
			}
			return false;
		}
		return true;
	}
	return false;
};

void __fastcall TForm3::Button8Click(TObject * Sender) {
	if (SpinEdit2->Value > 0 && SpinEdit2->Value < StringGrid5->RowCount) {
		ShowMessage("Неверно задан номер записи!");
		return;
	}
	SpinEdit4->Value--;
	for (int i = SpinEdit4->Value; i < StringGrid5->RowCount; i++) {
		StringGrid5->Cells[0][i] = StringGrid5->Cells[0][i + 1];
		StringGrid5->Cells[1][i] = StringGrid5->Cells[1][i + 1];
		StringGrid5->Cells[2][i] = StringGrid5->Cells[2][i + 1];
	}
	StringGrid5->RowCount--;
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::SpinEdit4Change(TObject * Sender) {
	StringGrid5->RowCount = SpinEdit4->Value + 1;
	if (StringGrid5->RowCount > 1) {
		StringGrid5->FixedRows = 1;
	}
	for (int i = 1; i <= SpinEdit4->Value; i++) {
		StringGrid5->Cells[0][i] = String(i);
	}
}

// ---------------------------------------------------------------------------
void TForm3::up_pers() {
	Mypers = ans_person(Form1->send_mes(com("personnel", "select", 0,
		" ").ToJson())).data;
	sh_pers();
};

void TForm3::sh_pers() {
	ComboBox2->Clear();
	ComboBox4->Clear();
	for (auto x : Mypers) {
		ComboBox2->Items->Add(x.ra.name_rang + " " + x.fio);
	}
	ComboBox4->Items = ComboBox2->Items;
};

void __fastcall TForm3::ComboBox2Change(TObject * Sender) {
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	Myzak_out = ans_zak(Form1->send_mes(com("zakreplenie", "select", 1,
		String(Mypers[ComboBox2->ItemIndex].id_ls)).ToJson())).data;
	ComboBox3->Clear();
	for (auto x : Myzak_out) {
		ComboBox3->Items->Add(x.g.type.name_gun + ": " + x.g.serial + " " +
			x.g.num);
	}
}

// ---------------------------------------------------------------------------
// void TForm3::up_gun_out(int id_ls) {
// Mygun_out = ans_person(Form1->send_mes(com("personnel", "select", 0,
// String(id_ls)).ToJson())).data;
// ComboBox3->Clear();
// for (auto x : Mygun_out) {
// ComboBox3->Items->Add(x.g.type.name_gun + ": " + x.g.serial + " " +
// x.g.num);
// }
// };

void __fastcall TForm3::ComboBox4Change(TObject * Sender) {
	if (ComboBox4->ItemIndex == -1) {
		return;
	}
	up_gun_in(Mypers[ComboBox4->ItemIndex].id_ls);
};

// ---------------------------------------------------------------------------
void TForm3::up_gun_in(int id_ls) {
	Myinp = ans_outp(Form1->send_mes(com("gun", "select", 3,
		String(id_ls)).ToJson())).data;
	ComboBox5->Clear();
	for (auto x : Myinp) {
		ComboBox5->Items->Add(x.zak.g.type.name_gun + ": " + x.zak.g.serial +
			" " + x.zak.g.num);
	}
};

void __fastcall TForm3::Button10Click(TObject * Sender) {
	if (ComboBox4->ItemIndex == -1) {
		ShowMessage("Выберите военнослужащего!");
		return;
	}
	if (ComboBox5->ItemIndex == -1) {
		ShowMessage("Выберите оружие!");
		return;
	}
	in_p a;
	a.outp = Myinp[ComboBox5->ItemIndex];
	ShowMessage(simple_ans(Form1->send_mes(com("in_personnel", "insert", 0,
		a.ToJson()).ToJson())).result);
	Button1Click(Form3);
	ComboBox4->ItemIndex = -1;
	ComboBox5->ItemIndex = -1;
	start();
}
// ---------------------------------------------------------------------------

void __fastcall TForm3::Button6Click(TObject * Sender) {
	if (ComboBox2->ItemIndex == -1) {
		ShowMessage("Выберите военнослужащего!");
		return;
	}
	if (ComboBox3->ItemIndex == -1) {
		ShowMessage("Выберите оружие!");
		return;
	}
	out_p a;
	a.zak = Myzak_out[ComboBox3->ItemIndex];
	ShowMessage(simple_ans(Form1->send_mes(com("out_personnel", "insert", 0,
		a.ToJson()).ToJson())).result);
	Button2Click(Form3);
	ComboBox2->ItemIndex = -1;
	ComboBox3->ItemIndex = -1;
	start();
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button2Click(TObject * Sender) {
	two_date a;
	TDate d1 = DateTimePicker3->Date, d2 = DateTimePicker4->Date;
	if (d1 > d2)
		swap(d1, d2);
	a.date1 = d1;
	a.date2 = d2;
	ans_outp ans(Form1->send_mes(com("out_personnel", "select", 0,
		a.ToJson()).ToJson()));
	StringGrid1->FixedRows = 0;
	StringGrid1->RowCount = 1;
	for (auto x : ans.data) {
		int k = StringGrid1->RowCount++;
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = x.zak.g.type.name_gun + ": " +
			x.zak.g.serial + " " + x.zak.g.num;
		StringGrid1->Cells[2][k] = x.zak.ls.ra.name_rang + " " + x.zak.ls.fio;
		StringGrid1->Cells[3][k] = x.dej.ra.name_rang + " " + x.dej.fio;
		StringGrid1->Cells[4][k] = x.date_outp;
		StringGrid1->FixedRows = 1;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button1Click(TObject * Sender) {
	two_date a;
	TDate d1 = DateTimePicker1->Date, d2 = DateTimePicker2->Date;
	if (d1 > d2)
		swap(d1, d2);
	a.date1 = d1;
	a.date2 = d2;
	ans_inp ans(Form1->send_mes(com("in_personnel", "select", 0,
		a.ToJson()).ToJson()));
	StringGrid2->FixedRows = 0;
	StringGrid2->RowCount = 1;
	for (auto x : ans.data) {
		int k = StringGrid2->RowCount++;
		StringGrid2->Cells[0][k] = String(k);
		StringGrid2->Cells[1][k] = x.outp.zak.g.type.name_gun + ": " +
			x.outp.zak.g.serial + " " + x.outp.zak.g.num;
		StringGrid2->Cells[2][k] = x.outp.zak.ls.ra.name_rang + " " +
			x.outp.zak.ls.fio;
		StringGrid2->Cells[3][k] = x.dej.ra.name_rang + " " + x.dej.fio;
		StringGrid2->Cells[4][k] = x.date_inp;
		StringGrid2->FixedRows = 1;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button4Click(TObject * Sender) {
	two_date a;
	TDate d1 = DateTimePicker7->Date, d2 = DateTimePicker8->Date;
	if (d1 > d2)
		swap(d1, d2);
	a.date1 = d1;
	a.date2 = d2;
	ans_ins ans(Form1->send_mes(com("in_sklad", "select", 0,
		a.ToJson()).ToJson()));
	StringGrid4->FixedRows = 0;
	StringGrid4->RowCount = 1;
	for (auto x : ans.data) {
		int k = StringGrid4->RowCount++;
		StringGrid4->Cells[0][k] = String(k);
		StringGrid4->Cells[1][k] = x.g.type.name_gun;
		StringGrid4->Cells[2][k] = x.g.type.vid.name_vid;
		StringGrid4->Cells[3][k] = x.g.serial;
		StringGrid4->Cells[4][k] = x.g.num;
		StringGrid4->Cells[5][k] = x.dej.ra.name_rang + " " + x.dej.fio;
		StringGrid4->Cells[6][k] = x.date_ins;
		// StringGrid4->Cells[7][k] = "Примечание";
		StringGrid4->FixedRows = 1;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button3Click(TObject * Sender) {
	two_date a;
	TDate d1 = DateTimePicker5->Date, d2 = DateTimePicker6->Date;
	if (d1 > d2)
		swap(d1, d2);
	a.date1 = d1;
	a.date2 = d2;
	ans_outs ans(Form1->send_mes(com("out_sklad", "select", 1,
		a.ToJson()).ToJson()));
	StringGrid3->FixedRows = 0;
	StringGrid3->RowCount = 1;
	for (auto x : ans.data) {
		int k = StringGrid3->RowCount++;
		StringGrid3->Cells[0][k] = String(k);
		StringGrid3->Cells[1][k] = x.ins.g.type.name_gun;
		StringGrid3->Cells[2][k] = x.ins.g.type.vid.name_vid;
		StringGrid3->Cells[3][k] = x.ins.g.serial;
		StringGrid3->Cells[4][k] = x.ins.g.num;
		StringGrid3->Cells[5][k] = x.dej.ra.name_rang + " " + x.dej.fio;
		StringGrid3->Cells[6][k] = x.date_outs;
		// StringGrid3->Cells[7][k] = "Примечание";
		StringGrid3->FixedRows = 1;
	}
}

// ---------------------------------------------------------------------------
void TForm3::up_gun_for_sklad() {
	Mygun_for_sklad =
		ans_gun(Form1->send_mes(com("out_sklad", "select", 0,
		"").ToJson())).data;
	ComboBox1->Clear();
	for (auto x : Mygun_for_sklad) {
		ComboBox1->Items->Add(x.type.name_gun + ": " + x.serial + " " + x.num);
	}
};

void __fastcall TForm3::ComboBox1Change(TObject * Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	int k = ComboBox1->ItemIndex;
	for (auto x : Mygun_send) {
		if (x.id_gun == Mygun_for_sklad[ComboBox1->ItemIndex].id_gun) {
			return;
		}
	}
	Mygun_send.push_back(Mygun_for_sklad[ComboBox1->ItemIndex]);
	k = StringGrid6->RowCount++;
	StringGrid6->Cells[0][k] = String(k);
	StringGrid6->Cells[1][k] =
		Mygun_for_sklad[ComboBox1->ItemIndex].type.name_gun;
	StringGrid6->Cells[2][k] =
		Mygun_for_sklad[ComboBox1->ItemIndex].type.vid.name_vid;
	StringGrid6->Cells[3][k] = Mygun_for_sklad[ComboBox1->ItemIndex].serial;
	StringGrid6->Cells[4][k] = Mygun_for_sklad[ComboBox1->ItemIndex].num;
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button5Click(TObject * Sender) {
	ans_gun ans;
	ans.data = Mygun_send;
	ShowMessage(simple_ans(Form1->send_mes(com("out_sklad", "insert", 0,
		ans.ToJson()).ToJson())).result);
	for (int i = 1; i < StringGrid6->ColCount; i++) {
		for (int j = 1; j < StringGrid6->RowCount; j++) {
			StringGrid6->Cells[j][i] = "";
		}
	}
	StringGrid6->RowCount = 1;
	Mygun_send.clear();
	Button3Click(Form3);
	start();
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button7Click(TObject * Sender) {
	int k = SpinEdit1->Value;
	vector<gun>Myg;
	StringGrid6->RowCount = 1;
	for (int i = 0; i < Mygun_send.size(); i++) {
		if (i == k) {
			continue;
		}
		int s = StringGrid6->RowCount++;
		StringGrid6->Cells[0][s] = String(s);
		StringGrid6->Cells[1][s] = Mygun_send[i].type.name_gun;
		StringGrid6->Cells[2][s] = Mygun_send[i].type.vid.name_vid;
		StringGrid6->Cells[3][s] = Mygun_send[i].serial;
		StringGrid6->Cells[4][s] = Mygun_send[i].num;
		Myg.push_back(Mygun_send[i]);
	}
	Mygun_send = Myg;
}

// ---------------------------------------------------------------------------
void TForm3::start() {
	up_gt();
	up_pers();
	up_gun_for_sklad();
};

void __fastcall TForm3::Button11Click(TObject * Sender) {
	Variant v, vVarApp, vVarBooks, vVarBook, vVarSheets, vVarSheet, vVarCell;
	try {
		vVarApp = CreateOleObject("Excel.Application");
		// fStart = true;
	}
	catch (...) {
		return;
	}
	vVarApp.OlePropertySet("Visible", true);
	vVarBooks = vVarApp.OlePropertyGet("Workbooks");
	vVarApp.OlePropertySet("SheetsInNewWorkbook", 3);
	vVarBooks.OleProcedure("Add");
	vVarBook = vVarBooks.OlePropertyGet("Item", 1);
	vVarSheets = vVarBook.OlePropertyGet("Worksheets");
	vVarSheet = vVarSheets.OlePropertyGet("Item", 1);

	for (int j = 0; j < StringGrid4->ColCount; j++) {
		for (int i = 0; i < StringGrid4->RowCount; i++) {
			vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet("Item",
				i + 1, j + 1);
			// UnicodeString aSG = StringGrid1->Cells[j][i];
			vVarCell.OlePropertySet("Value",
				TVariant(StringGrid4->Cells[j][i]));
			vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button12Click(TObject * Sender) {
	Variant v, vVarApp, vVarBooks, vVarBook, vVarSheets, vVarSheet, vVarCell;
	try {
		vVarApp = CreateOleObject("Excel.Application");
		// fStart = true;
	}
	catch (...) {
		return;
	}
	vVarApp.OlePropertySet("Visible", true);
	vVarBooks = vVarApp.OlePropertyGet("Workbooks");
	vVarApp.OlePropertySet("SheetsInNewWorkbook", 3);
	vVarBooks.OleProcedure("Add");
	vVarBook = vVarBooks.OlePropertyGet("Item", 1);
	vVarSheets = vVarBook.OlePropertyGet("Worksheets");
	vVarSheet = vVarSheets.OlePropertyGet("Item", 1);

	for (int j = 0; j < StringGrid3->ColCount; j++) {
		for (int i = 0; i < StringGrid3->RowCount; i++) {
			vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet("Item",
				i + 1, j + 1);
			// UnicodeString aSG = StringGrid1->Cells[j][i];
			vVarCell.OlePropertySet("Value",
				TVariant(StringGrid3->Cells[j][i]));
			vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button14Click(TObject * Sender) {
	Variant v, vVarApp, vVarBooks, vVarBook, vVarSheets, vVarSheet, vVarCell;
	try {
		vVarApp = CreateOleObject("Excel.Application");
		// fStart = true;
	}
	catch (...) {
		return;
	}
	vVarApp.OlePropertySet("Visible", true);
	vVarBooks = vVarApp.OlePropertyGet("Workbooks");
	vVarApp.OlePropertySet("SheetsInNewWorkbook", 3);
	vVarBooks.OleProcedure("Add");
	vVarBook = vVarBooks.OlePropertyGet("Item", 1);
	vVarSheets = vVarBook.OlePropertyGet("Worksheets");
	vVarSheet = vVarSheets.OlePropertyGet("Item", 1);

	for (int j = 0; j < StringGrid1->ColCount; j++) {
		for (int i = 0; i < StringGrid1->RowCount; i++) {
			vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet("Item",
				i + 1, j + 1);
			// UnicodeString aSG = StringGrid1->Cells[j][i];
			vVarCell.OlePropertySet("Value",
				TVariant(StringGrid1->Cells[j][i]));
			vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm3::Button13Click(TObject * Sender) {
	Variant v, vVarApp, vVarBooks, vVarBook, vVarSheets, vVarSheet, vVarCell;
	try {
		vVarApp = CreateOleObject("Excel.Application");
		// fStart = true;
	}
	catch (...) {
		return;
	}
	vVarApp.OlePropertySet("Visible", true);
	vVarBooks = vVarApp.OlePropertyGet("Workbooks");
	vVarApp.OlePropertySet("SheetsInNewWorkbook", 3);
	vVarBooks.OleProcedure("Add");
	vVarBook = vVarBooks.OlePropertyGet("Item", 1);
	vVarSheets = vVarBook.OlePropertyGet("Worksheets");
	vVarSheet = vVarSheets.OlePropertyGet("Item", 1);

	for (int j = 0; j < StringGrid2->ColCount; j++) {
		for (int i = 0; i < StringGrid2->RowCount; i++) {
			vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet("Item",
				i + 1, j + 1);
			// UnicodeString aSG = StringGrid1->Cells[j][i];
			vVarCell.OlePropertySet("Value",
				TVariant(StringGrid2->Cells[j][i]));
			vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
		}
	}
}
// ---------------------------------------------------------------------------
