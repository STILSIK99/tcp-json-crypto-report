// ---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "Unit1.h"
#include "Unit3.h"

// ---------------------------------------------------------------------------
class TForm2 : public TForm {
__published: // IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox2;
	TButton *Button2;
	TButton *Button3;
	TMemo *Memo1;
	TGroupBox *GroupBox1;
	TButton *Button1;
	TLabeledEdit *LabeledEdit1;
	TButton *Button4;
	TEdit *Edit1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *Edit2;
	TButton *Button5;

	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button5Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm2(TComponent* Owner);
	void loadData();
	void showData(int);
	void recalc();

	vector<String>pages;
	File file;
	int left, right, current;
	int spp;

	int searchWords(String);
	void clear();
	int item = 0;

};

// ---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
// ---------------------------------------------------------------------------
#endif
