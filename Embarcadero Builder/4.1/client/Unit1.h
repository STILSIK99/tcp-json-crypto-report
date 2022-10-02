// ---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>

#include "CryptoSession.h"
#include "File1.h"
#include <windows.h>
#include "IdHashMessageDigest.hpp"

#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <Vcl.ComCtrls.hpp>
#include <IdContext.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>
#include <IdIPWatch.hpp>

#include <vector>
#include <set>

#include <utilcls.h>
#include <Vcl.OleServer.hpp>
#include <ComObj.hpp>
using namespace std;

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components
	TGroupBox *GroupBox1;
	TMainMenu *MainMenu1;
	TMenuItem *N1;
	TImage *Image1;
	TLabeledEdit *LabeledEdit1;
	TLabeledEdit *LabeledEdit2;
	TButton *Button1;
	TGroupBox *GroupBox2;
	TLabeledEdit *LabeledEdit3;
	TLabeledEdit *LabeledEdit4;
	TButton *Button2;
	TIdTCPClient *Client;
	TGroupBox *GroupBox3;
	TGroupBox *GroupBox4;
	TGroupBox *GroupBox5;
	TListBox *ListBox1;
	TListBox *ListBox2;
	TListBox *ListBox3;
	TGroupBox *GroupBox6;
	TGroupBox *GroupBox7;
	TGroupBox *GroupBox8;
	TLabeledEdit *LabeledEdit5;
	TLabeledEdit *LabeledEdit6;
	TButton *Button4;
	TButton *Button5;
	TButton *Button6;
	TButton *Button7;
	TButton *Button8;
	TStatusBar *StatusBar1;
	TButton *Button9;
	TButton *Button10;
	TTimer *Timer1;
	TIdTCPServer *IdTCPServer1;
	TIdIPWatch *IP1;
	TGroupBox *GroupBox10;
	TGroupBox *GroupBox11;
	TGroupBox *GroupBox12;
	TButton *Button11;

	void __fastcall N1Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
	void __fastcall Button9Click(TObject *Sender);
	void __fastcall Button10Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall IdTCPServer1Execute(TIdContext *AContext);
	void __fastcall Button11Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);
	String match_regex(String reg, String str);
	listDriv readDrivs();
	String decrypt();
	String send_mes(String request);

	CryptoSession * cryptoSession;
	listDriv etalon;
	listDriv onPC;
	listDriv notEnough;
	listDriv excess;
	String compName;

	void reload();
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
