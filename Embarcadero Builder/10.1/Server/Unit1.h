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
	TIdTCPClient *Client1;
	TFDQuery *FDQuery1;
	TFDQuery *FDQuery2;
	TFDQuery *FDQuery3;
	TFDQuery *FDQuery4;
	TFDQuery *FDQuery5;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TGroupBox *GroupBox4;
	TComboBox *ComboBox1;
	TComboBox *ComboBox2;
	TGroupBox *GroupBox5;
	TListBox *ListBox1;
	TChart *Chart1;
	TLineSeries *Series1;
	TAreaSeries *Series2;
	TLineSeries *Series4;
	TMemo *Memo2;
	TFDQuery *FDQuery6;
	TFDQuery *FDQuery7;
	TLabeledEdit *LabeledEdit1;
	TButton *Button1;
	TFDQuery *FDQuery8;
	TButton *Button2;

	void __fastcall ServerExecute(TIdContext *AContext);
	void __fastcall ListBox1DblClick(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall ComboBox2Change(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);

	map<TIdContext*, CryptoSession*>clients;

	String decrypt(TIdContext *AContext);
	void send_mes(String request, TIdContext *AContext);
	String log(String data, String);
	bool initDBConnection();

	String ins_data(String, String);

	map<String, agent>agsData;

	void loadAgs(int);
	void loadData(int);

	String curAg;
	String curDisk;

	void disconnect(String);

	void get_info(String, String);

	void loadOtchet();

};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
