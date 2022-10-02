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
	curr_monitor = -1;
	hh = -1;
	mm = -1;
	send = 0;
	curr_file = "";

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Директория мониторинга";
	StringGrid1->Cells[2][0] = "Время начала";
	StringGrid1->ColWidths[0] = 40;
	StringGrid1->ColWidths[1] = 260;
	StringGrid1->ColWidths[2] = 300;

	StringGrid2->Cells[0][0] = "№";
	StringGrid2->Cells[1][0] = "Имя файла";
	StringGrid2->Cells[2][0] = "Событие";
	StringGrid2->Cells[3][0] = "Время события";
	StringGrid2->ColWidths[0] = 70;
	StringGrid2->ColWidths[1] = 150;
	StringGrid2->ColWidths[2] = 100;
	StringGrid2->ColWidths[3] = 300;

}
// ---------------------------------------------------------------------------

void TForm2::checkTimes(String path, int idMon) {
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
			FFile x;
			x.name = w.cFileName;
			FileTimeToSystemTime(&w.ftLastAccessTime, &x.t_A);
			FileTimeToSystemTime(&w.ftLastWriteTime, &x.t_C);
			if (run[path].files.count(w.cFileName) != 0) {
				FFile old = run[path].files[w.cFileName];
				if (StrTime(old.t_A) != StrTime(x.t_A)) {
					Act aa = Act("Открытие", StrTime(x.t_A), idMon);
					aa.file = x;
					session.arr.push_back(aa);
				}
				if (StrTime(old.t_A) != StrTime(x.t_A)) {
					Act aa = Act("Изменение", StrTime(x.t_C), idMon);
					aa.file = x;
					session.arr.push_back(aa);
				}
			}
			run[path].files[w.cFileName] = x;
		}
	}
	FindClose(h); // закрытие дискриптора поиска
	// ShowMessage("s - " + String(s));
};

void __fastcall TForm2::Timer1Timer(TObject * Sender) {
	// acts.arr.clear();
	if (!run.size()) {
		Timer1->Enabled = 0;
	}
	for (auto x = run.begin(); x != run.end(); x++) {
		String path = x->first;
		checkTimes(path, x->second.id);
	}
	if (send) {
		Form1->send_mes(com("acts", "insert", 0, session.ToJson()).ToJson());
	}

	for (auto x : session.arr) {
		acts.arr.push_back(x);
	}
	session.arr.clear();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button2Click(TObject * Sender) {
	send = 1;
	String Path;
	if (SelectDirectory("Выберите каталог", "", Path)) // Path - папка
	{
		Path = Path;
	}
	else
		return;
	// Path = "E:\\Projects\\3 year kursach\\16\\Client";
	simple_ans ans;
	if (send) {
		ans = simple_ans(Form1->send_mes(com("monitor", "insert", 0,
			Path).ToJson()));
	}
	monitor a;
	a.id = ans.key;
	// a.id = run.size();
	run[Path] = a;
	showMonitors();
	if (!Timer1->Enabled) {
		Timer1->Enabled = 1;
	}

}

// ---------------------------------------------------------------------------
void TForm2::showMonitors() {
	for (int i = 1; i < StringGrid1->RowCount; i++) {
		for (int j = 0; j < StringGrid1->ColCount; j++) {
			StringGrid1->Cells[j][i] = "";
		}
	}
	ComboBox1->Clear();
	ComboBox2->Clear();
	int k = 0;
	for (auto x = run.begin(); x != run.end(); x++) {
		StringGrid1->RowCount = ++k + 1;
		StringGrid1->Cells[0][k] = String(k);
		StringGrid1->Cells[1][k] = x->first;
		StringGrid1->Cells[2][k] = x->second.timeStart;
		ComboBox1->Items->Add(x->first);
	}
	ComboBox2->Items = ComboBox1->Items;
};

void __fastcall TForm2::ComboBox1Change(TObject * Sender) {
	if (run.count(ComboBox1->Text)) {
		curr_monitor = run[ComboBox1->Text].id;
	}
	else
		return;
	ComboBox2->Clear();
	hh = -1;
	mm = -1;
	curr_file = "";
	for (auto x = run[ComboBox1->Text].files.begin();
	x != run[ComboBox1->Text].files.end(); x++) {
		ComboBox3->Items->Add(x->first);
	}
	showActs();
}

// ---------------------------------------------------------------------------
void TForm2::showActs() {
	for (int i = 1; i < StringGrid2->RowCount; i++) {
		for (int j = 0; j < StringGrid2->ColCount; j++) {
			StringGrid2->Cells[j][i] = "";
		}
	}
	StringGrid2->RowCount = 1;
	for (int i = 0; i < acts.arr.size(); i++) {
		if (acts.arr[i].id != curr_monitor) { // фильтр монитора
			continue;
		}
		if (curr_file != "") { // фильтр имени
			if (acts.arr[i].file.name != curr_file) {
				continue;
			}
		}
		if (hh != -1 && mm != -1) { // фильтр времени
			String currTime = TDateTime(Now()).FormatString("dd.mm.yyyy hh:mm");
			int diffMin = 60 * (StrToInt(currTime.SubString(12, 2))) +
				StrToInt(currTime.SubString(15, 2)) - 180;
			int actTime = 60 * (StrToInt(acts.arr[i].time.SubString(12, 2))) +
				StrToInt(acts.arr[i].time.SubString(15, 2));
			if (currTime.SubString(1, 10) != acts.arr[i].time.SubString(1,
				10) || ((diffMin - actTime) > (hh * 60 + mm))) {
				continue;
			}
		}
		StringGrid2->Cells[0][i + 1] = String(StringGrid2->RowCount++);
		StringGrid2->Cells[1][i + 1] = acts.arr[i].file.name;
		StringGrid2->Cells[2][i + 1] = acts.arr[i].act;
		StringGrid2->Cells[3][i + 1] = acts.arr[i].time;
	}

};

void __fastcall TForm2::Button3Click(TObject * Sender) {
	if (ComboBox3->ItemIndex != -1)
		curr_file = ComboBox3->Text;
	else
		curr_file = "";
	try {
		hh = Edit1->Text.ToInt();
		mm = Edit2->Text.ToInt();
		if (hh < 0 || mm < 0) {
			hh = -1;
			mm = -1;
			Edit1->Text = "";
			Edit2->Text = "";
		}

	}
	catch (...) {
		hh = -1;
		mm = -1;
		Edit1->Text = "";
		Edit2->Text = "";
		// ShowMessage("Некорретные данные");
	}

	showActs();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button4Click(TObject * Sender) {
	showActs();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button5Click(TObject * Sender) {
	hh = -1;
	mm = -1;
	curr_file = "";
	showActs();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender) {
	if (ComboBox2->ItemIndex == -1) {
		return;
	}
	run.erase(ComboBox2->Text);
	showMonitors();
	ComboBox2->ItemIndex = -1;
}
// ---------------------------------------------------------------------------
