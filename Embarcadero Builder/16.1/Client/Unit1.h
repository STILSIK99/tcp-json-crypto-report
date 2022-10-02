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
#include <IdContext.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>

#include <vcl.Filectrl.hpp>
#include <regex>
#include <windows.h>
#include <cstring>
#include "CryptoSession.h"
#include <sstream>
#include <iomanip>

using namespace std;

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components
	TIdTCPClient *Client;
	TButton *Button2;
	TGroupBox *GroupBox3;
	TLabeledEdit *Edit1;
	TLabeledEdit *Edit2;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TStringGrid *StringGrid1;
	TGroupBox *GroupBox4;
	TListBox *ListBox1;
	TGroupBox *GroupBox5;
	TButton *Button1;
	TGroupBox *GroupBox6;
	TComboBox *ComboBox1;
	TButton *Button3;
	TTimer *Timer1;
	TGroupBox *GroupBox7;
	TEdit *Edit3;
	TButton *Button4;
	TLabel *Label1;
	TIdTCPServer *IdTCPServer1;

	void __fastcall Button2Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall IdTCPServer1Execute(TIdContext *AContext);
	void __fastcall Timer1Timer(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);

	CryptoSession * cryptoSession;

	String decrypt();
	String send_mes(String request);

	int send = 0;

	int con = 0;

	vector<history>otchet;

	void checkChanges(String path);

	vector<String>monitors;
	map<String, pair<String, String> >changes;

	void showData();

	void showMons();

};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
