// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#include <utilcls.h>
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
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
	vVarParagraph = vVarParagraphs.OleFunction("Item", 1);
	vVarParagraph.OlePropertyGet("Range").OlePropertySet("Text",
		WideString("567"));
	vVarParagraph.OlePropertySet("Alignment", 1);
	vVarParagraphs.OleProcedure("Add");
	vVarParagraphs.OleProcedure("Add");
	vVarParagraph = vVarParagraphs.OleFunction("Item", 3);
	vVarParagraph.OlePropertySet("Alignment", 1);
	vVarRange = vVarParagraph.OlePropertyGet("Range");
	vVarDoc.OlePropertyGet("Tables").OleProcedure("Add", vVarRange, 2, 2, 1, 1);
	vVarTable = vVarDoc.OlePropertyGet("Tables").OleFunction("Item", 1);
	vVarTable.OlePropertyGet("Rows").OlePropertySet("Alignment", 1);
	vVarTable.OleFunction("AutoFitBehavior", 2);
	vVarTable.OlePropertyGet("Range").OlePropertyGet("Cells").OlePropertySet
		("VerticalAlignment", 1);
	vVarApp.OlePropertyGet("ActiveWindow").OlePropertyGet("View").OlePropertySet
		("TableGridlines", true);
	v1 = vVarTable.OlePropertyGet("Rows");
	v1.OleFunction("Item", 1).OlePropertySet("Height", 20);
	v1 = vVarTable.OlePropertyGet("Columns");
	for (int i = 0; i < 2; i++) {
		v1.OleFunction("Item", i + 1).OlePropertySet("PreferredWidthType", 2);
		v1.OleFunction("Item", i + 1).OlePropertySet("PreferredWidth",
			i*70 + 30);
	}
	v = vVarTable.OlePropertyGet("Shading");

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			vVarCell = vVarTable.OleFunction("Cell", i + 1, j + 1);
			vVarCell.OleFunction("Select");
			v = vVarApp.OlePropertyGet("Selection").OlePropertyGet("Font");
			if (i == 0) {
				v.OlePropertySet("Bold", 1);
			}
			else {
				v.OlePropertySet("Bold", 0);
			}
			v.OlePropertySet("Size", 14);
			v.OlePropertySet("Color", 0);
			vVarCell.OlePropertyGet("Range").OlePropertySet("Text",
				WideString("123"));
			v1 = vVarApp.OlePropertyGet("Selection").OlePropertyGet
				("ParagraphFormat");
			v1.OlePropertySet("Alignment", 1);
		}
	}

	AnsiString vAsCurDir = GetCurrentDir();
	AnsiString vAsCurDir1 = vAsCurDir + "\\test.doc";
	vVarDoc = vVarDocs.OleFunction("Item", 1);
	vVarDoc.OleProcedure("SaveAs", WideString(vAsCurDir1.c_str()));
	vVarApp.OleProcedure("Quit");
	// ShellExecute(NULL, L"open", vAsCurDir1.c_str(), NULL, NULL, SW_SHOWNORMAL);
	system(AnsiString(String("start ") + String(vAsCurDir1.c_str())).c_str());
}
// ---------------------------------------------------------------------------
