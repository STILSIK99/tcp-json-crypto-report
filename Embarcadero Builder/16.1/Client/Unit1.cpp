// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "IdHashMessageDigest.hpp"
TForm1 *Form1;

String to_string(SYSTEMTIME utc) {
	stringstream stm;
	const auto w2 = setw(2);
	stm << setfill('0') << std::setw(4)
		<< utc.wYear << '-' << w2 << utc.wMonth << '-' << w2 << utc.wDay <<
		' ' << w2 << utc.wHour << ':' << w2 << utc.wMinute << ':' << w2 <<
		utc.wSecond;
	return stm.str().c_str();
}

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	cryptoSession = new CryptoSession("dirMon");

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Директория мониторинга";
	StringGrid1->Cells[2][0] = "Действие";
	StringGrid1->Cells[3][0] = "Имя файла";
	StringGrid1->ColWidths[0] = 50;
	StringGrid1->ColWidths[1] = 350;
	StringGrid1->ColWidths[2] = 300;
	StringGrid1->ColWidths[3] = 200;

	send = 0;
	con = 0;
}

// ---------------------------------------------------------------------------
String TForm1::send_mes(String request) {
	try {
		// ShowMessage(request);
		Client->Connect(Edit1->Text, Edit2->Text.ToInt());
		if (!Client->Connected()) {
			// ShowMessage("Соединение отсутствует!");
			Client->Disconnect();
			return "";
		}
		char* crypted = 0;
		unsigned long size = request.Length() * 2 + 2;
		cryptoSession->CryptData((const char*)request.data(), crypted, size);
		TMemoryStream * ms = new TMemoryStream;
		ms->Write(crypted, size);
		delete[]crypted;
		ms->Seek(0, 0);
		Client->IOHandler->WriteBufferOpen();
		Client->IOHandler->Write(ms, -1, true);
		Client->IOHandler->WriteBufferClose();
		ms->Free();
		return decrypt();
	}
	catch (...) {
		// ShowMessage("Ошибка сообщения!");
		Client->Disconnect();
		send = 0;
		return "";
	}
};

String TForm1::decrypt() {
	TMemoryStream * ms = new TMemoryStream;
	Client->IOHandler->ReadStream(ms);
	unsigned long size = ms->Size;
	char * crypted = new char[size];
	ms->Seek(0, 0);
	ms->ReadBuffer(crypted, size);
	char * decrypted = 0;
	cryptoSession->DecryptData(crypted, decrypted, size);
	ms->Free();
	Client->Disconnect();
	return String((wchar_t*)decrypted);
};

void __fastcall TForm1::Button2Click(TObject *Sender) {
	try {
		simple_ans answer(send_mes(com("test", "", 111, simple_ans("",
			111).ToJson()).ToJson()));
		if (answer.key == 111) {
			ShowMessage("Менеджер подключен");
			con = 1;
		}
	}
	catch (...) {
		Client->Disconnect();
		ShowMessage("Нет соединения!");
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action) {
	if (con != 0) {
		send_mes(com("disconnect", "", 0, "").ToJson());
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	String Path;
	if (SelectDirectory("Выберите каталог для мониторинга", "", Path)) {
		Path = Path;
	}
	else
		return;
	monitors.push_back(Path);
	showMons();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	if (ComboBox1->ItemIndex == -1) {
		return;
	}
	monitors.erase(monitors.begin() + ComboBox1->ItemIndex);
	showMons();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender) {
	int t = 0;
	try {
		t = Edit3->Text.ToInt();
	}
	catch (...) {
		ShowMessage("Неверно задан параметр");
		return;
	}
	Label1->Caption = "Текущее время сбора данных - " + String(t) + " с";
	Timer1->Interval = 1000 * t;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::IdTCPServer1Execute(TIdContext *AContext) {
	simple_ans ans(AContext->Connection->IOHandler->ReadLn());
	if (ans.result == "turn_on") {
		if (send == 0) {
			if (Application->MessageBox(_T("Отправлять  данные?"),
				_T("Запрос от менеджера"),
				MB_YESNO + MB_TOPMOST + MB_ICONQUESTION) == IDYES) {
				simple_ans a("yes", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
				send = 1;
			}
			else {
				simple_ans a("no", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
			}
		}
	}

	if (ans.result == "turn_off") {
		send = 0;
		ShowMessage("Отправка данных выключена");
	}

	AContext->Connection->Disconnect();
}

// ---------------------------------------------------------------------------
void TForm1::checkChanges(String path) {
	WIN32_FIND_DATA w; // структура для поиска файлов.
	int attr = 0;
	attr |= faDirectory;
	HANDLE h = FindFirstFileW((path + "\\*").c_str(), &w); // начало поиска
	int s = 0;
	while (FindNextFile(h, &w)) // цикл поиска
	{
		if ((w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
			(String(w.cFileName) == "..") || (String(w.cFileName) == ".")) {
			continue;
		}
		else {
			SYSTEMTIME acc;
			SYSTEMTIME cha;
			FileTimeToSystemTime(&w.ftLastAccessTime, &acc);
			FileTimeToSystemTime(&w.ftLastWriteTime, &cha);
			String FN = path + "\\" + w.cFileName;
			String A = to_string(acc);
			String B = to_string(cha);
			if (changes.count(FN) != 0) {
				if (changes[FN].first != A) {
					otchet.push_back(history(FN, "Открытие", A));
					changes[FN].first = A;
				}
				if (changes[FN].second != B) {
					otchet.push_back(history(FN, "Изменение", B));
					changes[FN].second = B;
				}
			}
			else {
				changes[FN] = make_pair(A, B);
			}
		}
	}
	FindClose(h);
};

void TForm1::showData() {
	for (auto x : otchet) {
		int id = StringGrid1->RowCount++;
		StringGrid1->Cells[0][id] = String(id);
		StringGrid1->Cells[1][id] = x.filename;
		StringGrid1->Cells[2][id] = x.change;
		StringGrid1->Cells[3][id] = x.time;
	}
};

void TForm1::showMons() {
	ListBox1->Clear();
	ComboBox1->Clear();
	for (auto x : monitors) {
		ListBox1->Items->Add(x);
	}
	ComboBox1->Items = ListBox1->Items;
};

void __fastcall TForm1::Timer1Timer(TObject *Sender) {
	otchet.clear();
	for (auto path : monitors) {
		checkChanges(path);
	}
	showData();
	if (send == 1) {
		if (otchet.size() != 0) {
			ans_history ans;
			ans.arr = otchet;
			send_mes(com("data", "insert", 0, ans.ToJson()).ToJson());
		}
	}
}
// ---------------------------------------------------------------------------
