//---------------------------------------------------------------------------

#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>

#include "Unit1.h"
#include <Vcl.WinXPickers.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
//---------------------------------------------------------------------------
class TForm4 : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TStringGrid *StringGrid1;
	TGroupBox *GroupBox3;
	TLabeledEdit *LabeledEdit1;
	TLabeledEdit *LabeledEdit2;
	TComboBox *ComboBox1;
	TButton *Button1;
	TComboBox *ComboBox2;
	TButton *Button2;
	TComboBox *ComboBox4;
	TLabeledEdit *LabeledEdit3;
	TButton *Button3;
	TEdit *Edit1;
	TGroupBox *GroupBox4;
	TGroupBox *GroupBox5;
	TGroupBox *GroupBox6;
	TListBox *ListBox1;
	TButton *Button4;
	TGroupBox *GroupBox7;
	TGroupBox *GroupBox8;
	TButton *Button5;
	TLabel *Label1;
	TMemo *Memo1;
	TComboBox *ComboBox3;
	TButton *Button6;
	TTabSheet *TabSheet3;
	TGroupBox *GroupBox9;
	TGroupBox *GroupBox10;
	TStringGrid *StringGrid2;
	TComboBox *ComboBox5;
	TComboBox *ComboBox6;
	TGroupBox *GroupBox11;
	TButton *Button8;
	TDatePicker *DatePicker1;
	TDatePicker *DatePicker2;
	TLabel *Label2;
	TLabel *Label3;
	TCheckBox *CheckBox1;
	TOpenTextFileDialog *NewTxtFile;
	TMemo *Memo2;
	TButton *Button9;
	TButton *Button7;
	TTabSheet *TabSheet4;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall ListBox1DblClick(TObject *Sender);
	void __fastcall Button9Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall DatePicker1Change(TObject *Sender);
	void __fastcall DatePicker2Change(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
	__fastcall TForm4(TComponent* Owner);
	void loadData(int);
	void clear();




	vector<User>users;
	vector<File>files;
	vector<History>history;
	int idUser;
	String access(int a);
	int access(String a);
	String cutFileName(String str);
    String loadFile(String);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm4 *Form4;
//---------------------------------------------------------------------------
#endif
