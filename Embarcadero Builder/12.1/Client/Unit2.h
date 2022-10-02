// ---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Mask.hpp>
#include "Unit1.h"
#include <Vcl.FileCtrl.hpp>
#include <Vcl.WinXPickers.hpp>
#include <Vcl.ComCtrls.hpp>

#include <vector>

// ---------------------------------------------------------------------------
class TForm2 : public TForm {
__published: // IDE-managed Components
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TLabeledEdit *LabeledEdit1;
	TGroupBox *GroupBox3;
	TRadioGroup *RadioGroup1;
	TButton *Button1;
	TCheckBox *CheckBox1;
	TListBox *ListBox1;
	TLabel *Label1;
	TEdit *Edit1;
	TGroupBox *GroupBox4;
	TGroupBox *GroupBox5;
	TCheckBox *CheckBox2;
	TDateTimePicker *DatePicker1;
	TRadioGroup *RadioGroup2;

	void __fastcall Button1Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm2(TComponent* Owner);

	String reg_s(String, String);

	vector<File>files;

	void search(String, String);

	int size;

	String doDate(SYSTEMTIME);
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
// ---------------------------------------------------------------------------
#endif
