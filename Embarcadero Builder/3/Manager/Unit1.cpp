// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <cstdlib>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
#include <utilcls.h>
#include <vector>
using namespace std;

bool get_otchet(TStringGrid * StringGrid1) {
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
					("Item", i + 3, j + 1);
				// UnicodeString aSG = StringGrid1->Cells[j][i];
				vVarCell.OlePropertySet("Value",
					TVariant(StringGrid1->Cells[j][i].c_str()));
				vVarCell.OlePropertySet("ColumnWidth", TVariant(30));
			}
		}
		// ShowMessage();
		vVarCell = vVarSheet.OlePropertyGet("Cells").OlePropertyGet
			("Item", 1, 1);
		// UnicodeString aSG = StringGrid1->Cells[j][i];
		vVarCell.OlePropertySet("Value",
			TVariant(("Агент - " + Form1->ComboBox2->Text).c_str()));
		return true;
	}
	catch (...) {
		return false;
	}
}

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	if (!OpenDialog1->Execute()) {
		return;
	}
	String con = "Database=" + OpenDialog1->FileName + ";DriverID=SQLite";
	FDConnection1->ConnectionString = con;
	try {
		FDConnection1->Connected = 1;
	}
	catch (...) {
		Application->Terminate();
	}

	if (!FDConnection1->Connected) {
		Application->Terminate();
	}

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Доступная физическая память";
	StringGrid1->Cells[2][0] = "Занятая физическая память";
	StringGrid1->Cells[3][0] = "Время";
	StringGrid1->ColWidths[0] = 100;
	StringGrid1->ColWidths[1] = 300;
	StringGrid1->ColWidths[2] = 300;
	StringGrid1->ColWidths[3] = 100;

	loadDB();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::IdTCPServer1Execute(TIdContext * AContext) {
	//
	FDQuery1->SQL->Text = "insert into data values(null,:ag,:to,:av,:ti)";
	FDQuery1->ParamByName("ag")->AsString = AContext->Binding->PeerIP;
	String request = AContext->Connection->IOHandler->ReadLn(enUTF8);
	auto req = (TJSONObject*)TJSONObject::ParseJSONValue(request);
	try {
		if (String(req->GetValue("type")->Value().c_str()) == "insert") {
			if (String(req->GetValue("table")->Value().c_str()) == "data") {
				auto mas = (TJSONArray*)req->GetValue("data");
				for (int i = 0; i < mas->Count; i++) {
					auto x = (TJSONObject*)TJSONObject::ParseJSONValue
						(mas->Items[i]->ToString());
					FDQuery1->ParamByName("to")->AsString =
						x->GetValue("total")->Value().c_str();
					FDQuery1->ParamByName("av")->AsString =
						x->GetValue("avail")->Value().c_str();
					FDQuery1->ParamByName("ti")->AsString =
						x->GetValue("time")->Value().c_str();
					x->Free();
					FDQuery1->ExecSQL();
				}
				mas->Free();
			}
		}
		auto reply = new TJSONObject;
		reply->AddPair("answer", "yes");
		AContext->Connection->IOHandler->WriteLn(reply->ToString(), enUTF8);
		reply->Free();

	}
	catch (...) {
	}
	AContext->Connection->Disconnect();
	req->Free();
	loadDB();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	auto js = new TJSONObject;
	js->AddPair("type", "connection");
	try {
		Client->Connect(Edit1->Text, Edit2->Text.ToInt());
		if (!Client->Connected()) {
			return;
		}
		agents.erase(Edit1->Text);
		// ogran.erase(Edit1->Text);
		Client->IOHandler->WriteLn(js->ToString(), enUTF8);
		auto x = (TJSONObject*)TJSONObject::ParseJSONValue
			(Client->IOHandler->ReadLn(enUTF8));
		if (String(x->GetValue("answer")->Value().c_str()) == "yes") {
			agents[Edit1->Text] = false;
			// ogran[Edit1->Text] = make_pair(false, -1);
		}
		x->Free();
	}
	catch (...) {
	}
	Client->Disconnect();
	upd_agents();

}
// ---------------------------------------------------------------------------

void TForm1::upd_agents() {
	ListBox1->Clear();
	ComboBox1->Clear();
	for (auto x : agents) {
		ComboBox1->Items->Add(x.first);
		String f = x.first;
		if (x.second) {
			f += " мониторинг включен";
		}
		else
			f += " мониторинг отключен";
		ListBox1->Items->Add(f);
	}
}

void __fastcall TForm1::Button2Click(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	auto js = new TJSONObject;
	js->AddPair("type", "disconnection");
	try {
		Client->Connect(ComboBox1->Text, Edit2->Text.ToInt());
		if (!Client->Connected()) {
			agents.erase(ComboBox1->Text);
			// ogran.erase(ComboBox1->Text);
			return;
		}
		Client->IOHandler->WriteLn(js->ToString(), enUTF8);
		auto x = (TJSONObject*)TJSONObject::ParseJSONValue
			(Client->IOHandler->ReadLn(enUTF8));
		if (String(x->GetValue("answer")->Value().c_str()) == "yes") {
			agents.erase(ComboBox1->Text);
			// ogran.erase(ComboBox1->Text);
		}

		x->Free();
	}
	catch (...) {
	}
	Client->Disconnect();
	upd_agents();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		return;
	}
	String ag = ComboBox1->Items->Strings[ListBox1->ItemIndex];
	if (agents[ag]) {
		auto js = new TJSONObject;
		js->AddPair("type", "send_off");
		try {
			Client->Connect(ag, Edit2->Text.ToInt());
			if (!Client->Connected()) {
				agents.erase(ag);
				// ogran.erase(ag);
				return;
			}
			Client->IOHandler->WriteLn(js->ToString(), enUTF8);
			auto x = (TJSONObject*)TJSONObject::ParseJSONValue
				(Client->IOHandler->ReadLn(enUTF8));
			if (String(x->GetValue("answer")->Value().c_str()) == "yes") {
				agents[ag] = false;
			}
			x->Free();
		}
		catch (...) {
		}
		Client->Disconnect();

	}
	else {
		auto js = new TJSONObject;
		js->AddPair("type", "send_on");
		try {
			Client->Connect(ag, Edit2->Text.ToInt());
			if (!Client->Connected()) {
				agents.erase(ag);
				// ogran.erase(ag);
				return;
			}
			Client->IOHandler->WriteLn(js->ToString(), enUTF8);
			auto x = (TJSONObject*)TJSONObject::ParseJSONValue
				(Client->IOHandler->ReadLn(enUTF8));
			if (String(x->GetValue("answer")->Value().c_str()) == "yes") {
				agents[ag] = true;
			}
			x->Free();
		}
		catch (...) {
			Client->Disconnect();
		}
	}
	upd_agents();
}

// ---------------------------------------------------------------------------
void TForm1::loadDB() {
	ComboBox1->Clear();
	FDQuery2->SQL->Text = "select distinct agent from data";
	FDQuery2->Active = 1;
	for (FDQuery2->First(); !FDQuery2->Eof; FDQuery2->Next()) {
		ComboBox2->Items->Add(FDQuery2->FieldByName("agent")->Value);
	}
	FDQuery2->Active = 0;
}

void __fastcall TForm1::ComboBox3Change(TObject *Sender) {
	ogr = 0;
	Edit3->Text = "";
	SHD();
}

// ---------------------------------------------------------------------------
void TForm1::SHD() {
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	FDQuery3->SQL->Text = "select * from data where agent=:a ";
	FDQuery3->ParamByName("a")->AsString = ComboBox2->Text;
	if (ComboBox3->ItemIndex != -1) {
		TDateTime current(Now());
		switch (ComboBox3->ItemIndex) {
		case 0:
			current -= StrToTime("00:10:00");
			break;
		case 1:
			current -= StrToTime("00:30:00");
			break;
		case 2:
			current -= StrToTime("01:00:00");
			break;
		case 3:
			current -= StrToTime("06:00:00");
			break;

		default: ;
		}
		FDQuery3->SQL->Text += "and time > '" + current.FormatString
			("hh:mm:ss") + "'";
	}
	FDQuery3->SQL->Text += " order by id";
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	StringGrid1->RowCount = 1;

	FDQuery3->Active = 1;
	for (FDQuery3->First(); !FDQuery3->Eof; FDQuery3->Next()) {
		if (ogr == 1) {
			String f1 = FDQuery3->FieldByName("avail")->Value;
			String f2 = Edit3->Text;
			if (ch(f1, f2)) {
				continue; // f1 < f2
			}
		}
		int k = StringGrid1->RowCount++;
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = FDQuery3->FieldByName("total")->Value;
		StringGrid1->Cells[2][k] = FDQuery3->FieldByName("avail")->Value;
		StringGrid1->Cells[3][k] = FDQuery3->FieldByName("time")->Value;
	}
	FDQuery3->Active = 0;
}

void __fastcall TForm1::ComboBox2Change(TObject *Sender) {
	ogr = 0;
	Edit3->Text = "";
	SHD();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	try {
		long long f = atoll((char *)Edit3->Text.c_str());
		ogr = 1;
	}
	catch (...) {
		ShowMessage("Неверные данные!");
		return;
	}
	SHD();

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender) {
	get_otchet(StringGrid1);
}
// ---------------------------------------------------------------------------

bool TForm1::ch(String a1, String a2) {
	if (a2.Length() > a1.Length()) {
		return true;
	}
	if (a2.Length() < a1.Length()) {
		return false;
	}
	for (int i = 1; i <= a2.Length(); i++) {
		if (a2[i] > a1[i]) {
			return true;
		}
	}
	return false;
}
