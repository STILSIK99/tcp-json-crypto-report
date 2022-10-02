// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include "Unit1.h"
#include <set>
#include <fstream>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Файл";
	StringGrid1->Cells[2][0] = "Сигнатура";
	StringGrid1->ColWidths[0] = 50;
	StringGrid1->ColWidths[1] = 300;
	StringGrid1->ColWidths[2] = 300;
	StringGrid1->RowCount = 1;
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender) {
	String path;
	if (!SelectDirectory("Выберите каталог", "", path)) // Path - папка
	{
		return;
	}
	ListBox1->Clear();
	checkDir(path);
	Form1->send_mes(com("scanning", "insert", 0, ans.ToJson()).ToJson());
	scanRes = ans.arr;
	ans.arr.clear();

}
// ---------------------------------------------------------------------------

void TForm2::checkDir(String path) {

	WIN32_FIND_DATA w; // структура для поиска файлов.
	int attr = 0;
	attr |= faDirectory;
	HANDLE h = FindFirstFileW((path + "\\*").c_str(), &w); // начало поиска
	int s = 0;
	map<String, int>exist;
	while (FindNextFile(h, &w)) // цикл поиска
	{
		if ((String(w.cFileName) == "..") || (String(w.cFileName) == ".")) {
			continue;
		}

		if (w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			checkDir(path + "\\" + String(w.cFileName));
		}
		else {
			ListBox1->Items->Add(path + "\\" + String(w.cFileName));
			for (auto s : signatures) {
				vector<int>res;
				char simbol;
				ifstream ff;
				ff.open(String(path + "\\" + String(w.cFileName)).c_str(),
					ios::binary);
				int sch = 0;
				while (ff >> simbol) {

					// я поставил ограничение на 100 байт, чтобы не висла программа на долго
					if (sch++ == 100) {
						break;
						// если этот if закомментить, то будет полностью файл проверять на наличие этой сигнатуры
					}
					res.push_back(0);
					for (int next = 0; next < res.size(); ++next) {
						char f =
							StrToInt("0x" + s.data.SubString
							(2 * (res[next]) + 1, 2));
						if (f == simbol) {
							res[next]++;
							if (res[next] == (s.data.Length() / 2)) {
								ans.arr.push_back
									(Scan(path + "\\" + String(w.cFileName),
									s.name));
								res.erase(res.begin() + next);
							}
						}
						else
							res.erase(res.begin() + next);
					}
				}
				ff.close();
			}

		}
	}
	FindClose(h);
};

void __fastcall TForm2::ComboBox1Change(TObject *Sender) {
	for (int i = 0; i < StringGrid1->ColCount; i++) {
		for (int j = 1; j < StringGrid1->RowCount; j++) {
			StringGrid1->Cells[i][j] = "";
		}
	}
	StringGrid1->RowCount = 1;
	if (ComboBox1->ItemIndex == -1) {
		for (auto x : scanRes) {
			int f = StringGrid1->RowCount++;
			StringGrid1->Cells[0][f] = String(f);
			StringGrid1->Cells[1][f] = x.file;
			StringGrid1->Cells[2][f] = x.sign;
		}
	}
	else {
		for (auto x : scanRes) {
			if (x.sign == ComboBox1->Text) {
				int f = StringGrid1->RowCount++;
				StringGrid1->Cells[0][f] = String(f);
				StringGrid1->Cells[1][f] = x.file;
				StringGrid1->Cells[2][f] = x.sign;
			}
		}
	}
	try {
		StringGrid1->FixedRows = 1;
	}
	catch (...) {
	}
}
// ---------------------------------------------------------------------------
