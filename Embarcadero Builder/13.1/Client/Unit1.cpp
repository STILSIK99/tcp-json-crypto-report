// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "File2.h"
#include <fstream>

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "IdHashMessageDigest.hpp"
TForm1 *Form1;
using namespace std;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	cryptoSession = new CryptoSession("signAnalyzer");

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Файл";
	StringGrid1->Cells[2][0] = "Найденная сигнатура";
	StringGrid1->ColWidths[0] = 100;
	StringGrid1->ColWidths[1] = 200;
	StringGrid1->ColWidths[2] = 200;

}

// ---------------------------------------------------------------------------
String TForm1::send_mes(String request) {
	try {
		// ShowMessage(request);
		Client->Connect(Edit1->Text, Edit2->Text.ToInt());
		if (!Client->Connected()) {
			// ShowMessage("Соединение отсутствует!");
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
		simple_ans answer(send_mes(com("test", "", 0, "").ToJson()));
		if (answer.key == 123) {
			ShowMessage("Сервер подключен");
		}
	}
	catch (...) {
		Client->Disconnect();
		ShowMessage("Нет соединения");
		return;
	}
	signs = ans_Sign(send_mes(com("sign", "select", 0, "").ToJson())).arr;
	ShowMessage("База данных сигнатур получена");
}

// ---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender) {
	String path;
	if (!SelectDirectory("Выберите каталог для мониторинга", "", path)) {
		return;
	}
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	StringGrid1->RowCount = 1;
	files.clear();
	TreeView1->Items->Clear();
	TreeView1->Items->Add(NULL, path);
	scan(path, TreeView1->Items->Item[0]);
	checkFiles();
}

// ---------------------------------------------------------------------------
void TForm1::scan(String path, TTreeNode* parent) {
	WIN32_FIND_DATA w; // структура для поиска файлов.
	int attr = 0;
	attr |= faDirectory;
	HANDLE h = FindFirstFileW((path + "\\*").c_str(), &w); // начало поиска
	int s = 0;
	while (FindNextFile(h, &w)) // цикл поиска
	{
		if ((String(w.cFileName) == "..") || (String(w.cFileName) == ".")) {
			continue;
		}

		if (w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			TTreeNode *node = TreeView1->Items->AddChild(parent,
				String(w.cFileName));
			scan(path + "\\" + String(w.cFileName), node);
		}
		else {
			TTreeNode *node = TreeView1->Items->AddChild(parent,
				String(w.cFileName));
			files.push_back(path + "\\" + String(w.cFileName));
		}
	}
	FindClose(h);
}

void TForm1::checkFiles() {
	ans_Res ans;
	ProgressBar1->Max = files.size() * signs.size();
	ProgressBar1->Position = 0;
	for (auto file : files) {
		for (auto sign : signs) {
			int shift = StrToInt(sign.shift);
			int size = sign.posled.Length() / 2 + shift;
			ifstream ff;
			ff.open(file.c_str(), ios::binary);
			char *buffer = new char[size];
			ff.read(buffer, size);
			String str = ToHex((unsigned char*)buffer, size);
			str = str.SubString(shift, str.Length() - shift);
			delete[]buffer;
			ff.close();

			// str - hex строка, сравниваем с сигнатурой
			string subject = AnsiString(str).c_str();
			string reg = AnsiString(StringReplace(sign.posled, ".", "\.",
				TReplaceFlags() << rfReplaceAll << rfIgnoreCase)).c_str();
			bool a = false;
			try {
				regex re(reg);
				smatch match;
				if (regex_search(subject, match, re)) {
					a = true;
				}
			}
			catch (regex_error& e) {
			}
			ProgressBar1->Position++;
			if (a) {
				ans.arr.push_back(Result(file, sign.name));
				int id = StringGrid1->RowCount++;
				StringGrid1->Cells[0][id] = String(id);
				StringGrid1->Cells[1][id] = file;
				StringGrid1->Cells[2][id] = sign.name;
				StringGrid1->Cells[3][id] = sign.descr;
			}

		}
	}

	send_mes(com("results", "insert", 0, ans.ToJson()).ToJson());
	if (StringGrid1->RowCount > 1) {
		StringGrid1->FixedRows = 1;
	}
};
