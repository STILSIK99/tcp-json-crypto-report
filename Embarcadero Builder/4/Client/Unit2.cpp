// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include "Excel_XP_srvr.h"
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	strGrd();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button2Click(TObject *Sender) {
	ListBox1->Clear();
	for (int i = 0; i < mydrivers.size(); i++) {
		String res = reg_s(LabeledEdit1->Text, mydrivers[i]);
		if (res != "") {
			ListBox1->Items->Add(mydrivers[i]);
		}
	}
}

// ---------------------------------------------------------------------------
void TForm2::strGrd() {
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Драйвер";
	StringGrid1->Cells[2][0] = "Соответствие эталону";
	StringGrid1->Cells[3][0] = "Наличиче на этом ПК";
	StringGrid1->ColWidths[0] = 50;
	StringGrid1->ColWidths[1] = 200;
	StringGrid1->ColWidths[2] = 250;
	StringGrid1->ColWidths[3] = 250;
	for (int i = 1; i < StringGrid1->RowCount; i++) {
		for (int j = 0; j < StringGrid1->ColCount; j++) {
			StringGrid1->Cells[j][i] = "";
		}
	}
	StringGrid1->RowCount = 1;
	StringGrid1->FixedCols = 1;
};

void TForm2::load() {
	strGrd();
	ListBox1->Clear();
	mydrivers = get_drivers();
	sort(mydrivers.begin(), mydrivers.end());

	Drivers otchet;
	otchet.arr = mydrivers;
	simple_ans ans_s(Form1->send_mes(com("drivers", "insert", 0,
		otchet.ToJson()).ToJson()));
	if (ans_s.key == 0)
		ShowMessage(ans_s.result);

	Drivers standard =
		Drivers(Form1->send_mes(com("drivers", "select", 0, "").ToJson()));
	sort(standard.arr.begin(), standard.arr.end());
	map<String, int>drdr;
	vector<String>full;
	strGrd();
	int k = 1;
	for (int i = 0; i < standard.arr.size(); i++) {
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = standard.arr[i];
		StringGrid1->Cells[2][k] = "+";
		drdr[standard.arr[i]] = k++;
		full.push_back(standard.arr[i]);
		StringGrid1->RowCount = k;

	}
	for (int i = 0; i < mydrivers.size(); i++) {
		if (drdr.count(mydrivers[i]) == 0) {
			StringGrid1->Cells[0][k] = String(k);
			StringGrid1->Cells[1][k] = mydrivers[i];
			StringGrid1->Cells[3][k] = "+";
			drdr[mydrivers[i]] = k++;
			full.push_back(mydrivers[i]);
			StringGrid1->RowCount = k;
		}
		else {
			StringGrid1->Cells[3][drdr[mydrivers[i]]] = "+";
		}
	}
	StringGrid1->FixedRows = 1;

	twoStr stat(Form1->send_mes(com("drivers", "select", 1, "").ToJson()));
	LabeledEdit2->Text = stat.ff;
	LabeledEdit3->Text = stat.ss;
};

bool get_otchet(TStringGrid *StringGrid1) {
	try {
		Variant v, vVarApp, vVarBooks, vVarBook, vVarSheets, vVarSheet,
		vVarCell;
		try {
			vVarApp = CreateOleObject("Excel.Application");
			// fStart = true;
		}
		catch (...) {
			return false;
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
				vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet
					("Item", i + 1, j + 1);
				// UnicodeString aSG = StringGrid1->Cells[j][i];
				vVarCell.OlePropertySet("Value",
					TVariant(StringGrid1->Cells[j][i].c_str()));
				vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
			}
		}
		// ShowMessage();

		return true;
	}
	catch (...) {
		return false;
	}
}

void __fastcall TForm2::Button1Click(TObject *Sender) {
	get_otchet(StringGrid1);
}
// ---------------------------------------------------------------------------
