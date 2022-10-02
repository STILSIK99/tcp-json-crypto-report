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
#include <psapi.h>
#include "pdh.h"
#include "tlhelp32.h"

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
	TStringGrid *StringGrid1;
	TGroupBox *GroupBox3;
	TGroupBox *GroupBox4;
	TGroupBox *GroupBox5;
	TEdit *Edit3;
	TButton *Button3;
	TTimer *Timer1;
	TLabeledEdit *Edit1;
	TLabeledEdit *Edit2;
	TButton *Button1;
	TLabel *Label1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TListBox *ListBox1;
	TIdTCPServer *IdTCPServer1;

	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall IdTCPServer1Execute(TIdContext *AContext);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private: // User declarations
		public : // User declarations
	__fastcall TForm1(TComponent* Owner);

	CryptoSession * cryptoSession;

	String decrypt();
	String send_mes(String request);
	void getProcessNames();
	int TForm1::getMemory(unsigned long);

	vector<Process>procMas;

	int send;
	int update;
	void showData();

};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
