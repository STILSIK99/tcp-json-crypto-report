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

#include <vector>
#include <cstring>
#include <map>
#include <utility>
#include <windows.h>
#include <stdio.h>

#include <Vcl.Grids.hpp>

#include <vcl.Filectrl.hpp>
#include <regex>
#include <windows.h>
#include <cstring>
#include "CryptoSession.h"

using namespace std;

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components

	TIdTCPClient *Client;
	TButton *Button2;
	TGroupBox *GroupBox4;
	TLabeledEdit *Edit1;
	TLabeledEdit *Edit2;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTreeView *TreeView1;
	TGroupBox *GroupBox1;
	TButton *Button1;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TStringGrid *StringGrid1;
	TProgressBar *ProgressBar1;

	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);

	CryptoSession * cryptoSession;

	String decrypt();
	String send_mes(String request);

	vector<Signature>signs;
	vector<String>files;

	void scan(String, TTreeNode*);
	void checkFiles();


};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
