// ---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include "Unit2.h"
#include "File1.h"
#include <vector>
using namespace std;

// ---------------------------------------------------------------------------
class TForm2 : public TForm {
__published: // IDE-managed Components

	TListBox *ListBox1;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TButton *Button1;
	TComboBox *ComboBox1;
	TStringGrid *StringGrid1;

	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);

private: // User declarations
		public : // User declarations
	__fastcall TForm2(TComponent* Owner);

	vector<Sign>signatures;

	void checkDir(String);

	vector<Scan>scanRes;
	ans_Scan ans;
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
// ---------------------------------------------------------------------------
#endif
