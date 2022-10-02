// ---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include "Unit1.h"
#include <VCLTee.Series.hpp>

// ---------------------------------------------------------------------------
class TForm2 : public TForm {
__published: // IDE-managed Components
	TTimer *Timer1;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TListBox *ListBox1;
	TGroupBox *GroupBox3;
	TChart *Chart1;
	TLineSeries *Series1;
	TLineSeries *Series2;
	TLineSeries *Series3;
	TMemo *Memo1;

	void __fastcall ListBox1DblClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall ListBox1Exit(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm2(TComponent* Owner);

	listDisk* vols;

	std::vector<listDisk*>data;

	void clear();
	void loadData();

	int diskIndex;
	int per;
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
// ---------------------------------------------------------------------------
#endif
