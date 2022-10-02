// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "Unit2.h"
TForm2 *Form2;
#include <sstream>
#include <iostream>
#include <cstring>
#include <regex>
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
#include <utilcls.h>

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	file.name = "";
	file.source = "";
	spp = 1000;
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender) {
	Form3->loadData();
	Form3->Show();
}

void TForm2::clear() {
	file.name = "";
	file.source = "";
	spp = 1000;
	left = 0;
	current = 0;
	right = 0;
	pages.clear();
	Memo1->Text = "";
	Edit1->Text = "";
	String a = "/ " + String(right);
	Label1->Caption = a;
}

// ---------------------------------------------------------------------------
void TForm2::loadData() {
	Label3->Caption = file.name;
	Edit1->Text = "";
	Edit2->Text = "";
	recalc();

	if (pages.size() == 0) {
		return;
	}
	left = 1;
	current = 1;
	right = pages.size();
	String a = "/ " + String(right);
	Label1->Caption = a;
	Edit1->Text = String(current);
	showData(current);
};

void TForm2::recalc() {
	pages.clear();
	try {
		spp = StrToInt(LabeledEdit1->Text);
	}
	catch (...) {
		spp = 1000;
		LabeledEdit1->Text = "1000";
	}
	for (int i = 0; i < (file.source.Length() + spp - 1) / spp; i++) {
		pages.push_back(file.source.SubString(i*spp + 1, spp));
	}
	if (pages.size() == 0) {
		return;
	}
};

void __fastcall TForm2::Button2Click(TObject *Sender) {
	if (current == left) {
		ShowMessage("Это начало!");
		return;
	}
	showData(--current);
	Edit1->Text = current;
}
// ---------------------------------------------------------------------------

void __fastcall TForm2::Button3Click(TObject *Sender) {
	if (current == right) {
		ShowMessage("Это последняя страница!");
		return;
	}
	showData(++current);
	Edit1->Text = current;
}

// ---------------------------------------------------------------------------
void TForm2::showData(int a) {
	Memo1->Text = pages[a - 1];
	int col = searchWords(pages[a - 1]);
	Edit2->Text = "Найдено " + String(col) + " слов(а)";
};

void __fastcall TForm2::Button4Click(TObject *Sender) {
	loadData();
}

// ---------------------------------------------------------------------------
int TForm2::searchWords(String str) {
	int count = 0;
	int sch = 0;
	for (int i = 1; i < str.Length(); i++) {
		if (str[i] <= 32) {
			if (sch > 3) {
				count++;
			}
			sch = 0;
		}
		else
			sch++;
	}
	if (sch > 3) {
		count++;
	}
	return count;
};

void __fastcall TForm2::FormClose(TObject *Sender, TCloseAction &Action) {
	Form1->send_mes(com("close", "", 0, "").ToJson());
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button5Click(TObject *Sender) {
	Variant v, v1, vVarCell, vVarTable, vVarRange, vVarParagraph, vVarApp,
		vVarDocs, vVarDoc, vVarParagraphs;
	try {
		vVarApp = CreateOleObject("Word.Application");
	}
	catch (...) {
		ShowMessage("Ошибка");
		return;
	}

	vVarApp.OlePropertySet("Visible", false);
	vVarDocs = vVarApp.OlePropertyGet("Documents");
	vVarDocs.OleProcedure("Add");
	if (vVarDocs.OlePropertyGet("Count") != 1) {
		ShowMessage("Ошибка");
		return;
	}

	vVarDoc = vVarDocs.OleFunction("Item", 1);
	vVarDoc.OleProcedure("Activate");

	vVarParagraphs = vVarDoc.OlePropertyGet("Paragraphs");
	vVarParagraphs.OleProcedure("Add");
	vVarParagraph = vVarParagraphs.OleFunction("Item", ++item);
	vVarParagraph.OlePropertyGet("Range").OlePropertySet("Text",
		WideString(Memo1->Text.c_str()));

	AnsiString vAsCurDir = GetCurrentDir();
	vAsCurDir = "\"" + vAsCurDir;
	AnsiString vAsCurDir1 = vAsCurDir + "\\test.doc\"";
	vVarDoc = vVarDocs.OleFunction("Item", 1);
	vVarDoc.OleProcedure("SaveAs", WideString(vAsCurDir1.c_str()));
	vVarApp.OleProcedure("Quit");
	// ShellExecute(NULL, L"open", vAsCurDir1.c_str(), NULL, NULL, SW_SHOWNORMAL);
	system(AnsiString(vAsCurDir1).c_str());
}
// ---------------------------------------------------------------------------
