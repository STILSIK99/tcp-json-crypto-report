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
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>

#include <vector>

#include <map>

// ---------------------------------------------------------------------------
class TForm2 : public TForm {
__published: // IDE-managed Components

	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TGroupBox *GroupBox4;
	TStringGrid *StringGrid2;
	TStringGrid *StringGrid1;
	TGroupBox *GroupBox5;
	TTimer *Timer1;
	TGroupBox *GroupBox6;
	TGroupBox *GroupBox7;
	TComboBox *ComboBox2;
	TButton *Button1;
	TButton *Button2;
	TComboBox *ComboBox3;
	TButton *Button3;
	TEdit *Edit1;
	TEdit *Edit2;
	TLabel *Label2;
	TComboBox *ComboBox1;
	TGroupBox *GroupBox8;
	TButton *Button4;
	TButton *Button5;

	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm2(TComponent* Owner);

	map<String, monitor>run;

	void checkTimes(String, int);

	ans_Act acts;

	void showMonitors();

	int curr_monitor;
	String curr_file;

	void showActs();

	int hh;
	int mm;
	ans_Act session;
	int send;
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
// ---------------------------------------------------------------------------
#endif
