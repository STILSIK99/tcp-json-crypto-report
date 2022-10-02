// ---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Samples.Spin.hpp>
#include <Vcl.Mask.hpp>
#include "File1.h"
#include "Unit1.h"
#include "Excel_XP_srvr.h"
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>

// ---------------------------------------------------------------------------
class TForm3 : public TForm {
__published: // IDE-managed Components

	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TTabSheet *TabSheet4;
	TStringGrid *StringGrid1;
	TStringGrid *StringGrid2;
	TLabel *Label1;
	TLabel *Label2;
	TDateTimePicker *DateTimePicker1;
	TDateTimePicker *DateTimePicker2;
	TButton *Button1;
	TDateTimePicker *DateTimePicker3;
	TDateTimePicker *DateTimePicker4;
	TButton *Button2;
	TLabel *Label3;
	TDateTimePicker *DateTimePicker5;
	TDateTimePicker *DateTimePicker6;
	TButton *Button3;
	TStringGrid *StringGrid3;
	TLabel *Label4;
	TDateTimePicker *DateTimePicker7;
	TDateTimePicker *DateTimePicker8;
	TButton *Button4;
	TStringGrid *StringGrid4;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox6;
	TLabel *Label5;
	TStringGrid *StringGrid5;
	TButton *Button9;
	TComboBox *ComboBox6;
	TSpinEdit *SpinEdit4;
	TGroupBox *GroupBox3;
	TStringGrid *StringGrid6;
	TButton *Button5;
	TComboBox *ComboBox1;
	TComboBox *ComboBox2;
	TComboBox *ComboBox3;
	TButton *Button6;
	TButton *Button7;
	TSpinEdit *SpinEdit1;
	TButton *Button8;
	TSpinEdit *SpinEdit2;
	TComboBox *ComboBox4;
	TComboBox *ComboBox5;
	TButton *Button10;
	TButton *Button11;
	TButton *Button12;
	TButton *Button13;
	TButton *Button14;

	void __fastcall Button9Click(TObject *Sender);
	void __fastcall ComboBox6Change(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
	void __fastcall SpinEdit4Change(TObject *Sender);
	void __fastcall ComboBox4Change(TObject *Sender);
	void __fastcall ComboBox2Change(TObject *Sender);
	void __fastcall Button10Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall Button11Click(TObject *Sender);
	void __fastcall Button12Click(TObject *Sender);
	void __fastcall Button14Click(TObject *Sender);
	void __fastcall Button13Click(TObject *Sender);

private: // User declarations
		public : // User declarations
	__fastcall TForm3(TComponent* Owner);
	void up_gt();
	void sh_gt();
	void up_pers();
	void sh_pers();
	void up_gun_out(int);
	void up_gun_in(int);
	bool check(String, int);
	void up_gun_for_sklad();
	void start();

	vector<GT>Mygt;
	vector<person>Mypers;
	vector<zakrep>Myzak_out;
	vector<out_p>Myinp;
	vector<gun>Mygun_for_sklad;
	vector<gun>Mygun_send;
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
// ---------------------------------------------------------------------------
#endif
