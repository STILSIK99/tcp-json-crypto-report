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
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <map>

using std::map;
using std::vector;

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components
	TFDConnection *FDConnection1;
	TIdTCPServer *Server;
	TMemo *Memo1;
	TFDQuery *FDQuery1;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TIdTCPClient *Client1;
	TChart *Chart1;
	TGroupBox *GroupBox4;
	TGroupBox *GroupBox5;
	TGroupBox *GroupBox6;
	TListBox *ListBox1;
	TListBox *ListBox2;
	TComboBox *ComboBox1;
	TLabeledEdit *LabeledEdit1;
	TComboBox *ComboBox2;
	TButton *Button1;
	TFDQuery *FDQuery2;
	TGroupBox *GroupBox7;
	TGroupBox *GroupBox8;
	TGroupBox *GroupBox9;
	TGroupBox *GroupBox10;
	TComboBox *ComboBox3;
	TButton *Button2;
	TComboBox *ComboBox4;
	TButton *Button3;
	TGroupBox *GroupBox11;
	TLabel *Label1;
	TFDQuery *FDQuery3;
	TLineSeries *Series1;
	TLineSeries *Series2;
	TLineSeries *Series3;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TListBox *ListBox3;
	TButton *Button4;

	void __fastcall ServerExecute(TIdContext *AContext);
	void __fastcall ListBox2DblClick(TObject *Sender);
	void __fastcall ListBox1DblClick(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);

	map<TIdContext*, CryptoSession*>clients;

	String decrypt(TIdContext *AContext);
	void send_mes(String request, TIdContext *AContext);
	String log(String data, String);
	bool initDBConnection();

	void clear();
	void loadData(int);

	map<String, vector<listDisk> >IpDisks;
	map<String, map<String, double> >IpDiskBound;
	map<String, int>mons;

	vector<String>agents;

	String ins_info(String data, String);

	String idAg;
	int idDisk;
	String nDisk;

	void checkDisks(String ip);
	void disconnect(String);
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
