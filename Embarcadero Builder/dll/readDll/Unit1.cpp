// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <cstring>
using namespace std;
typedef unsigned long long ull;

#include "Unit1.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
extern "C" ull* __declspec(dllexport) __stdcall get_disk(string name);
extern "C" string* __declspec(dllexport) __stdcall get_disks_names();

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	string *disks = get_disks_names();
	for (int i = 0; i < 26; i++) {
		if (disks[i][0] == 0) {
			continue;
		}
		Memo1->Lines->Add(String(disks[i].c_str()));
		ull* mas = get_disk(disks[i]);
		Memo1->Lines->Add(StrToInt(mas[0]));
		Memo1->Lines->Add(StrToInt(mas[1]));
		Memo1->Lines->Add(StrToInt(mas[2]));
		Memo1->Lines->Add(StrToInt(mas[3]));
	}
}
// ---------------------------------------------------------------------------
