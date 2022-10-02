// ---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "IniFiles.hpp"
#include <Data.DB.hpp>
#include <FireDAC.Comp.Client.hpp>
#include <FireDAC.Phys.hpp>
#include <FireDAC.Phys.Intf.hpp>
#include <FireDAC.Stan.Async.hpp>
#include <FireDAC.Stan.Def.hpp>
#include <FireDAC.Stan.Error.hpp>
#include <FireDAC.Stan.Intf.hpp>
#include <FireDAC.Stan.Option.hpp>
#include <FireDAC.Stan.Pool.hpp>
#include <FireDAC.UI.Intf.hpp>
#include <FireDAC.VCLUI.Wait.hpp>
#include "File1.h"
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>
#include "CryptoSession.h"
#include <IdContext.hpp>
#include <FireDAC.Comp.DataSet.hpp>
#include <FireDAC.DApt.hpp>
#include <FireDAC.DApt.Intf.hpp>
#include <FireDAC.DatS.hpp>
#include <FireDAC.Stan.Param.hpp>
#include <FireDAC.Phys.SQLite.hpp>
#include <FireDAC.Phys.SQLiteDef.hpp>
#include <FireDAC.Stan.ExprFuncs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <map>

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components

	TIdTCPServer *Server;
	TMemo *Memo1;
	TFDQuery *FDQuery1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TFDQuery *FDQuery2;
	TFDQuery *FDQuery3;
	TFDQuery *FDQuery4;
	TFDQuery *FDQuery5;
	TFDConnection *FDConnection1;
	TFDQuery *FDQuery6;
	TIdTCPClient *Client1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TGroupBox *GroupBox1;
	TStringGrid *StringGrid1;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TComboBox *ComboBox1;
	TButton *Button1;
	TGroupBox *GroupBox4;
	TLabeledEdit *LabeledEdit1;
	TLabeledEdit *LabeledEdit2;
	TButton *Button2;
	TGroupBox *GroupBox6;
	TButton *Button3;
	TStringGrid *StringGrid2;
	TComboBox *ComboBox2;
	TComboBox *ComboBox3;
	TButton *Button4;
	TLabeledEdit *LabeledEdit3;
	TFDQuery *FDQuery7;
	TFDQuery *FDQuery8;

	void __fastcall ServerExecute(TIdContext *AContext);
	void __fastcall ComboBox2Change(TObject *Sender);
	void __fastcall ComboBox3Change(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);

	map<TIdContext*, CryptoSession*>clients;

	String decrypt(TIdContext *AContext);
	void send_mes(String request, TIdContext *AContext);
	String log(String data);
	bool initDBConnection();

	bool checkSign(String);

	void loadSign();
	void loadAgs();
	void loadResults();
	String ins_res(String, String);
	String sel_signs();

	vector<int>signs;
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
