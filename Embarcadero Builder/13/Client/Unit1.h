// ---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "File1.h"
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ComCtrls.hpp>
#include <IdContext.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>


#include <cstring>
#include <map>
#include <utility>
#include <windows.h>
#include <stdio.h>
#include "Unit2.h"

#include <Vcl.Grids.hpp>

#include <vcl.Filectrl.hpp>
#include <regex>
#include <cstring>
#include <vector>
using namespace std;

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components

	TIdTCPClient *Client;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TEdit *Edit1;
	TEdit *Edit2;
	TLabeledEdit *LabeledEdit1;
	TLabeledEdit *LabeledEdit2;
	TButton *Button1;
	TButton *Button2;

	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);

private: // User declarations
		public : // User declarations
	__fastcall TForm1(TComponent* Owner);

	String decrypt();
	String send_mes(String request);

	// vector<String>files;
	//
	// void scan(String, TTreeNode*);
	// void checkFiles();

};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
