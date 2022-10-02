#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	if (O->Execute()) {
		String con = String("Database=") + O->FileName +
			String(";DriverID=SQLite");

		if (initDBConnection(con)) {
			ShowMessage("Connected!");
		}
	}
	else
		ExitProcess(0);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
bool TForm1::initDBConnection(String con) {

	Form1->FDConnection1->ConnectionString = con;
	try {
		Form1->FDConnection1->Open();
	}
	catch (...) {
		return false;
	}
	return Form1->FDConnection1->Connected;
};

// ---------------------------------------------------------------------------
String TForm1::decrypt(TIdContext *AContext) {
	TMemoryStream * ms = new TMemoryStream;
	AContext->Connection->IOHandler->ReadStream(ms);
	unsigned long size = ms->Size;
	char * crypted = new char[size];
	ms->Seek(0, 0);
	ms->ReadBuffer(crypted, size);
	char * decrypted = 0;
	clients[AContext]->DecryptData(crypted, decrypted, size);
	ms->Free();
	return String((wchar_t*)decrypted);
};

void TForm1::send_mes(String request, TIdContext *AContext) {
	Memo1->Lines->Add(request);
	char* crypted = 0;
	unsigned long size = request.Length() * 2 + 2;
	clients[AContext]->CryptData((const char*)request.data(), crypted, size);
	TMemoryStream * ms = new TMemoryStream;
	ms->Write(crypted, size);
	delete[]crypted;
	ms->Seek(0, 0);
	AContext->Connection->IOHandler->WriteBufferOpen();
	AContext->Connection->IOHandler->Write(ms, -1, true);
	AContext->Connection->IOHandler->WriteBufferClose();
	ms->Free();
	AContext->Connection->Disconnect();
};

// ---------------------------------------------------------------------------

void __fastcall TForm1::ServerExecute(TIdContext *AContext) {
	if (clients.count(AContext) == 0) {
		clients[AContext] = new CryptoSession("driverpack");
	}
	UnicodeString answer = "";
	try {
		UnicodeString request = decrypt(AContext);
		Memo1->Text += request;
		com req(request);

		Memo1->Lines->Add(AContext->Binding->PeerIP + " ---- " + req.ToJson());
		if (req.table_name == "polz" && req.type == "select") {
			answer = log(req.data, AContext->Binding->PeerIP);
		}
		if (req.table_name == "test") {
			answer = simple_ans("Есть соединение!", 1).ToJson();
		}

		if (req.table_name == "drivers") {
			if (req.type == "select") {
				if (req.key == 0) {
					answer = get_standard();
				}
				if (req.key == 1) {
					answer = get_statistic();
				}
			}
			if (req.type == "insert") {
				answer = ins_drivers(req.data, AContext->Binding->PeerIP);
			}
		}
		if (req.table_name == "close") {
			disconnect(AContext->Binding->PeerIP);
			AContext->Connection->Disconnect();
			return;
		}

		if (answer == "") {
			simple_ans ans;
			ans.result = "Command error!";
			send_mes(ans.ToJson(), AContext);
		}
		else
			send_mes(answer, AContext);
	}
	catch (...) {
		simple_ans ans;
		ans.result = "Decrypt error!";
		AContext->Connection->IOHandler->WriteLn(ans.ToJson(), enUTF8);
	}
	AContext->Connection->Disconnect();
}
// ---------------------------------------------------------------------------

String TForm1::log(String data, String Ip) {
	login n_w(data);
	simple_ans ans;
	FDQuery1->SQL->Text =
		"select * from polzovatel where user=:log and hash=:pass";
	FDQuery1->ParamByName("log")->AsString = n_w.log;
	FDQuery1->ParamByName("pass")->AsString = n_w.hash;
	FDQuery1->Active = 1;
	FDQuery1->First();
	if (!FDQuery1->Eof) {
		ans.key = 1;
		int fl(0);
		for (int i = 0; i < ags.size(); i++) {
			if (ags[i].ip == Ip) {
				fl++;
			}
		}
		if (!fl) {
			ags.push_back(agent(Ip, 0));
		}
		upd_ags();
	}
	else
		ans.result = "Login error!";
	FDQuery1->Active = 0;
	return ans.ToJson();
};

String TForm1::get_standard() {
	Drivers ans;
	FDQuery2->SQL->Text =
		"select d.driverName from drivers d, standard s where d.ip=s.ip";
	FDQuery2->Active = 1;
	for (FDQuery2->First(); !FDQuery2->Eof; FDQuery2->Next()) {
		ans.arr.push_back(FDQuery2->FieldByName("driverName")->Value);
	}
	FDQuery2->Active = 0;
	return ans.ToJson();
};

String TForm1::ins_drivers(String data, String ip) {
	Drivers in(data);
	simple_ans ans;
	try {
		FDQuery4->SQL->Text = "delete from drivers where ip=:ip";
		FDQuery4->ParamByName("ip")->AsString = ip;
		FDQuery4->ExecSQL();
		FDQuery3->SQL->Text =
			"insert into drivers (ip,driverName) values (:ip,:DN)";
		for (int i = 0; i < in.arr.size(); i++) {
			FDQuery3->ParamByName("ip")->AsString = ip;
			FDQuery3->ParamByName("DN")->AsString = in.arr[i];
			FDQuery3->ExecSQL();
		}
		ans.result = "Данные успешно внесены!";
		for (int i = 0; i < ags.size(); i++) {
			if (ags[i].ip == ip) {
				ListBox1->Items->Strings[i] = ags[i].ip + " - период сбора " +
					String(ags[i].period) + " c - последнее обновление " +
					Now().TimeString();
				break;
			}
		}
	}
	catch (...) {
		ans.result = "Ошибка, данные не внесены!";
	}
	return ans.ToJson();
};

String TForm1::get_statistic() {
	twoStr ans;
	vector<int>count;
	int kolvo = 0;
	FDQuery5->SQL->Text = "select count(*) c,ip from drivers group by ip";
	FDQuery5->Active = 1;
	for (FDQuery5->First(); !FDQuery5->Eof; FDQuery5->Next()) {
		count.push_back(StrToInt(FDQuery5->FieldByName("c")->Value));
		kolvo++;
	}
	ans.ff = String(int(accumulate(count.begin(), count.end(),
		0) / double(kolvo) + 0.5));
	ans.ss = String(*max_element(begin(count), end(count)));
	FDQuery5->Active = 0;
	return ans.ToJson();
};

void TForm1::upd_ags() {
	ListBox1->Clear();
	for (int i = 0; i < ags.size(); i++) {
		String a = ags[i].ip;
		if (ags[i].period) {
			a += " - период сбора " + String(ags[i].period) + " c";
		}
		else
			a += " - период сбора данных не установлен";
		ListBox1->Items->Add(a);
	}
};

void __fastcall TForm1::Button1Click(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		ShowMessage("Выберите агента");
		return;
	}
	int t;
	try {
		t = LabeledEdit1->Text.ToInt();
		if (t < 10) {
			ShowMessage("Не меньше 10 секунд");
			return;
		}
	}
	catch (...) {
		ShowMessage("Задайте время в секундах!");
		return;
	}
	simple_ans req("turn_on", t);
	try {
		Client1->Connect(ags[ListBox1->ItemIndex].ip, 54320);
		if (!Client1->Connected()) {
			ShowMessage("Агент отключен!");
			ags.erase(ags.begin() + ListBox1->ItemIndex);
		}
		else {
			Client1->IOHandler->WriteLn(req.ToJson());
			String ans;
			simple_ans answer(Client1->IOHandler->ReadLn());
			if (answer.result == "yes") {
				ags[ListBox1->ItemIndex].period = t;
			}
			else
				ShowMessage("Мониторинг отменен агентом");
		}
	}
	catch (...) {
	}
	Client1->Disconnect();
	upd_ags();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		ShowMessage("Выберите агента");
		return;
	}
	simple_ans req("turn_off", 0);
	try {
		Client1->Connect(ags[ListBox1->ItemIndex].ip, 54320);
		if (!Client1->Connected()) {
			ShowMessage("Агент отключен!");
			ags.erase(ags.begin() + ListBox1->ItemIndex);
		}
		else {
			Client1->IOHandler->WriteLn(req.ToJson());
			String ans;
			ags[ListBox1->ItemIndex].period = 0;
		}
	}
	catch (...) {
	}
	Client1->Disconnect();
	upd_ags();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex == -1) {
		return;
	}
	current.arr.clear();
	FDQuery6->Active = 0;
	FDQuery6->SQL->Text = "select driverName from drivers where ip=:IP";
	FDQuery6->ParamByName("ip")->AsString = ags[ListBox1->ItemIndex].ip;
	FDQuery6->Active = 1;
	for (FDQuery6->First(); !FDQuery6->Eof; FDQuery6->Next()) {
		current.arr.push_back(FDQuery6->FieldByName("driverName")->Value);
	}
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Название драйвера";
	StringGrid1->RowCount = current.arr.size() + 1;
	for (int i = 0; i < current.arr.size(); i++) {
		StringGrid1->Cells[0][i + 1] = String(i + 1);
		StringGrid1->Cells[1][i + 1] = current.arr[i];
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender) {
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Название драйвера";
	StringGrid1->RowCount = current.arr.size() + 1;
	for (int i = 0; i < current.arr.size(); i++) {
		StringGrid1->Cells[0][i + 1] = String(i + 1);
		StringGrid1->Cells[1][i + 1] = current.arr[i];
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	filter.arr.clear();
	for (int i = 0; i < current.arr.size(); i++) {
		if (reg_s(LabeledEdit2->Text, current.arr[i]) != "") {
			filter.arr.push_back(current.arr[i]);
		}
	}
	StringGrid1->RowCount = filter.arr.size() + 1;
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Название драйвера";
	for (int i = 0; i < filter.arr.size(); i++) {
		StringGrid1->Cells[0][i + 1] = String(i + 1);
		StringGrid1->Cells[1][i + 1] = filter.arr[i];
	}

}

// ---------------------------------------------------------------------------
void TForm1::disconnect(String ip) {
	for (int i = 0; i < ags.size(); i++) {
		if (ags[i].ip == ip) {
			ags.erase(ags.begin() + i);
			upd_ags();
			return;
		}
	}
};
