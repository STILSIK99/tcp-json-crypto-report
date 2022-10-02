// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include <cstring>
extern "C" string* __declspec(dllexport) __stdcall GetDrives();
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
using std::string;
#include "Excel_XP_srvr.h"
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
#include <utilcls.h>
bool get_otchet(TStringGrid*);

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Литера";
	StringGrid1->Cells[2][0] = "Тип диска";
	StringGrid1->Cells[3][0] = "Название";
	StringGrid1->Cells[4][0] = "Файловая система";
	StringGrid1->Cells[5][0] = "Серийный номер";

	StringGrid1->ColWidths[0] = 70;
	StringGrid1->ColWidths[1] = 200;
	StringGrid1->ColWidths[2] = 200;
	StringGrid1->ColWidths[3] = 300;
	StringGrid1->ColWidths[4] = 300;
	StringGrid1->ColWidths[5] = 200;

	flag = 0;
}

// ---------------------------------------------------------------------------

void TForm2::loadData() {
	Timer1->Enabled = 1;
	string* mas = GetDrives();
	data.arr.clear();
	for (int i = 0; i < 26; i++) {
		Drive x;
		x.DriveName = AnsiString(mas[5 * i].c_str());
		x.DriveType = AnsiString(mas[5 * i + 1].c_str());
		x.NameBuffer = AnsiString(mas[5 * i + 2].c_str());
		x.FileSystem = AnsiString(mas[5 * i + 3].c_str());
		x.VSNumber = AnsiString(mas[5 * i + 4].c_str());
		if (!x.check()) {
			continue;
		}
		data.arr.push_back(x);
	}
	showData();
};

// ---------------------------------------------------------------------------
void TForm2::showData() {
	if (flag) {
		Form1->send_mes(com("drives", "insert", 0, data.ToJson()).ToJson());
	}

	StringGrid1->RowCount = data.arr.size() + 1;
	for (int i = 0; i < data.arr.size(); i++) {
		StringGrid1->Cells[0][i + 1] = String(i + 1);
		StringGrid1->Cells[1][i + 1] = data.arr[i].DriveName;
		StringGrid1->Cells[2][i + 1] = data.arr[i].DriveType;
		StringGrid1->Cells[3][i + 1] = data.arr[i].NameBuffer;
		StringGrid1->Cells[4][i + 1] = data.arr[i].FileSystem;
		StringGrid1->Cells[5][i + 1] = data.arr[i].VSNumber;
	}
};

void __fastcall TForm2::Button1Click(TObject *Sender) {
	get_otchet(StringGrid1);
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Timer1Timer(TObject *Sender) {
	string* mas = GetDrives();
	MasDrive n;
	for (int i = 0; i < 26; i++) {
		Drive x;
		x.DriveName = AnsiString(mas[5 * i].c_str());
		x.DriveType = AnsiString(mas[5 * i + 1].c_str());
		x.NameBuffer = AnsiString(mas[5 * i + 2].c_str());
		x.FileSystem = AnsiString(mas[5 * i + 3].c_str());
		x.VSNumber = AnsiString(mas[5 * i + 4].c_str());
		if (!x.check()) {
			continue;
		}
		n.arr.push_back(x);
	}

	if (n.arr.size() != data.arr.size()) {
		data = n; // список изменился
		showData();
		return;
	}
	// проверяем неизменность
	for (int i = 0; i < n.arr.size(); i++) {
		int fl = 0;
		fl += n.arr[i].DriveName != data.arr[i].DriveName;
		fl += n.arr[i].DriveType != data.arr[i].DriveType;
		fl += n.arr[i].NameBuffer != data.arr[i].NameBuffer;
		fl += n.arr[i].FileSystem != data.arr[i].FileSystem;
		fl += n.arr[i].VSNumber != data.arr[i].VSNumber;
		if (fl == 0) {
			continue;
		}
		data = n; // список изменился
		showData();
		return;
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm2::FormClose(TObject *Sender, TCloseAction &Action) {
	Timer1->Enabled = 0;
	Form1->send_mes(com("close", "", 0, "").ToJson());
}
// ---------------------------------------------------------------------------

bool get_otchet(TStringGrid* StringGrid1) {
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
		// auto x = data->begin();
		for (int j = 0; j < StringGrid1->ColCount; j++) {
			for (int i = 0; i < StringGrid1->RowCount; i++) {
				vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet
					("Item", i + 1, j + 1);
				// UnicodeString aSG = StringGrid1->Cells[j][i];
				vVarCell.OlePropertySet("Value",
					TVariant(StringGrid1->Cells[j][i].c_str()));
				vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
				// x++;
			}
		}
		// ShowMessage();

		return true;
	}
	catch (...) {
		return false;
	}
}
