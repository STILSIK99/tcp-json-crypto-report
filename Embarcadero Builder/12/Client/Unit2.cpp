// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	// ShowMessage(DatePicker1->Date.DateString());
	DatePicker1->Date = Now();
}
// ---------------------------------------------------------------------------

String TForm2::reg_s(String reg, String str) {
	string subject = AnsiString(str).c_str();
	string result;
	try {
		regex re(AnsiString(reg).c_str());
		smatch match;
		if (regex_search(subject, match, re)) {
			return str;
		}
	}
	catch (regex_error& e) {
	}
	return "";
};

void __fastcall TForm2::Button1Click(TObject *Sender) {
	String Path;
	if (CheckBox1->Checked) {
		try {
			size = Edit1->Text.ToInt();

			for (int i = 1; i <= RadioGroup1->ItemIndex; i++) {
				size *= 1024;
			}

		}
		catch (...) {
			ShowMessage("Некорректный размер");
		}
	}
	if (SelectDirectory("Выберите каталог", "", Path)) // Path - папка
	{
		Path = Path;
	}
	else
		return;
	// Path = "E:\\Projects\\3 year kursach\\4";
	Label1->Caption = Path;
	files.clear();
	search(Path, LabeledEdit1->Text);
	SSearch req(Path, LabeledEdit1->Text, getParam(0), getParam(1));
	ListBox1->Clear();

	// ShowMessage(String(files.size()));
	for (auto x : files) {
		ListBox1->Items->Add(x.path + " - " + x.size + " - " + x.date);
	}

	ans_Files ans;
	ans.arr = files;
	ans.id = simple_ans(Form1->send_mes(com("search", "insert", 0,
		req.ToJson()).ToJson())).key;
	Form1->send_mes(com("result", "insert", 0, ans.ToJson()).ToJson());

}

// ---------------------------------------------------------------------------
void TForm2::search(String path, String reg) {
	WIN32_FIND_DATA w; // структура для поиска файлов.
	int attr = 0;
	attr |= faDirectory;
	HANDLE h = FindFirstFileW((path + "\\*").c_str(), &w); // начало поиска
	int s = 0;
	while (FindNextFile(h, &w)) // цикл поиска
	{
		if ((w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			(String(w.cFileName) != "..") && (String(w.cFileName) != ".")) {
			search(path + "\\" + w.cFileName, reg);
		}
		else {
			String res = reg_s(reg, w.cFileName);
			if (res != "" && res != ".." && res != ".") {
				// ListBox1->Items->Add(path + "\\" + w.cFileName);
				bool fl = false;
				if (CheckBox1->Checked) {
					if (RadioButton1->Checked) {
						fl = StrToInt(size) < w.nFileSizeLow;
					}
					if (RadioButton2->Checked) {
						fl = StrToInt(size) > w.nFileSizeLow;
					}
				}
				else
					fl = true;
				SYSTEMTIME t;
				FileTimeToSystemTime(&w.ftCreationTime, &t);
				bool fl1 = false;
				if (CheckBox2->Checked) {
					String d1 = DatePicker1->Date.DateString();
					String d2 = doDate(t);
					fl1 = d2 == d1;
				}
				else
					fl1 = true;

				if (fl && fl1) {
					File x(path + "\\" + w.cFileName, String(w.nFileSizeLow),
						doDate(t));
					files.push_back(x);
					s++;
				}

				// File x();
			}
		}
	}
	FindClose(h); // закрытие дискриптора поиска
	// ShowMessage("s - " + String(s));
};

String TForm2::doDate(SYSTEMTIME t) {
	String ans;
	if (String(t.wDay).Length() < 2) {
		ans += "0";
	}
	ans += String(t.wDay);
	ans += ".";
	if (String(t.wMonth).Length() < 2) {
		ans += "0";
	}
	ans += String(t.wMonth);
	ans += ".";
	ans += String(t.wYear);
	return ans;
};

String TForm2::getParam(int a) {
	String ans;
	if (a) {
		if (CheckBox1->Checked) {
			if (RadioButton1->Checked) {
				ans += ">";
			}
			else
				ans += "<";
			ans += Edit1->Text;
			switch (RadioGroup1->ItemIndex) {
			case 0:
				ans += " байт";
				break;
			case 1:
				ans += " КБайт";
				break;
			case 2:
				ans += " МБайт";
				break;
			case 3:
				ans += " ГБайт";
				break;
			default: ;
			}
		}
		else
			ans = "no size";
	}
	else {
		if (CheckBox2->Checked) {
			ans += DatePicker1->Date.DateString();
		}
		else
			ans = "no date";
	}
	return ans;
};
