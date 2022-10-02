// ---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Samples.Spin.hpp>
#include "File1.h"
#include "Unit1.h"

// ---------------------------------------------------------------------------
class TForm2 : public TForm {
__published: // IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TLabel *Label1;
	TLabel *Label2;
	TStringGrid *StringGrid1;
	TSpinEdit *SpinEdit1;
	TButton *Button1;
	TGroupBox *GroupBox1;
	TLabeledEdit *LabeledEdit1;
	TGroupBox *GroupBox2;
	TComboBox *ComboBox1;
	TLabeledEdit *LabeledEdit2;
	TButton *Button3;
	TStringGrid *StringGrid2;
	TGroupBox *GroupBox3;
	TLabeledEdit *LabeledEdit3;
	TComboBox *ComboBox2;
	TLabeledEdit *LabeledEdit4;
	TButton *Button5;
	TGroupBox *GroupBox4;
	TComboBox *ComboBox3;
	TComboBox *ComboBox4;
	TLabeledEdit *LabeledEdit5;
	TButton *Button6;
	TButton *Button4;
	TSpinEdit *SpinEdit2;
	TTabSheet *TabSheet5;
	TLabel *Label3;
	TStringGrid *StringGrid3;
	TGroupBox *GroupBox7;
	TComboBox *ComboBox7;
	TComboBox *ComboBox8;
	TButton *Button11;
	TGroupBox *GroupBox9;
	TComboBox *ComboBox13;
	TButton *Button12;
	TTabSheet *TabSheet2;
	TLabel *Label4;
	TStringGrid *StringGrid4;
	TSpinEdit *SpinEdit3;
	TButton *Button7;
	TGroupBox *GroupBox5;
	TComboBox *ComboBox5;
	TLabeledEdit *LabeledEdit6;
	TLabeledEdit *LabeledEdit7;
	TButton *Button8;
	TComboBox *ComboBox11;
	TGroupBox *GroupBox8;
	TComboBox *ComboBox9;
	TLabeledEdit *LabeledEdit8;
	TLabeledEdit *LabeledEdit9;
	TComboBox *ComboBox10;
	TButton *Button10;
	TComboBox *ComboBox12;
	TTabSheet *TabSheet4;
	TStringGrid *StringGrid6;
	TDateTimePicker *DateTimePicker1;
	TDateTimePicker *DateTimePicker2;
	TButton *Button13;
	TGroupBox *GroupBox10;
	TGroupBox *GroupBox11;
	TGroupBox *GroupBox12;
	TDateTimePicker *DateTimePicker3;
	TDateTimePicker *DateTimePicker4;
	TDateTimePicker *DateTimePicker5;
	TButton *Button14;
	TComboBox *ComboBox14;
	TButton *Button15;
	TComboBox *ComboBox15;
	TButton *Button16;
	TButton *Button2;
	TLabeledEdit *LabeledEdit10;

	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall ComboBox3Change(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
	void __fastcall ComboBox10Change(TObject *Sender);
	void __fastcall Button13Click(TObject *Sender);
	void __fastcall Button10Click(TObject *Sender);
	void __fastcall Button16Click(TObject *Sender);
	void __fastcall Button15Click(TObject *Sender);
	void __fastcall Button14Click(TObject *Sender);
	void __fastcall Button11Click(TObject *Sender);
	void __fastcall Button12Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm2(TComponent* Owner);
	void up_vid();
	void sh_vid();
	void up_gt();
	void sh_gt();
	void up_role();
	void up_rang();
	void up_pers();
	void sh_pers();
	// void up_zak();
	// void sh_zak();
	void up_gun();
	void up_gun_zak();
	void sh_gun_zak();
	void up_gun_nozak();
	void sh_gun_nozak();
	void start();

	vector<VG>Myvid;
	vector<GT>Mygt;
	vector<role>Myrole;
	vector<rang>Myrang;
	vector<person>Mypers;
	vector<person>Mypers_duty;
	vector<duty>Myduty;
	vector<zakrep>Myzak;
	vector<zakrep>Mygun_zak;
	vector<gun>Mygun;
	vector<gun>Mygun_nozak;
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
// ---------------------------------------------------------------------------
#endif
