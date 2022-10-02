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
#include <Vcl.DBGrids.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <map>
#include <algorithm>
#include <numeric>

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components
	TFDConnection *FDConnection1;
	TIdTCPServer *Server;
	TMemo *Memo1;
	TFDQuery *FDQuery1;
	TFDQuery *FDQuery2;
	TFDQuery *FDQuery3;
	TFDQuery *FDQuery4;
	TFDQuery *FDQuery5;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TDBGrid *DBGrid1;
	TFDQuery *FDQuery6;
	TDataSource *DataSource1;
	TComboBox *ComboBox1;
	TButton *Button1;
	TListBox *ListBox1;
	TLabeledEdit *LabeledEdit1;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox3;
	TButton *Button2;
	TLabel *Label1;
	TButton *Button3;
	TIdTCPClient *Client1;
	TTabSheet *TabSheet4;
	TDBGrid *DBGrid2;
	TGroupBox *GroupBox2;
	TComboBox *ComboBox2;
	TButton *Button4;
	TFDQuery *FDQuery7;
	TDataSource *DataSource2;
	TStatusBar *StatusBar1;

	void __fastcall ServerExecute(TIdContext *AContext);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);

	map<TIdContext*, CryptoSession*>clients;

	String decrypt(TIdContext *AContext);
	void send_mes(String request, TIdContext *AContext);
	String log(String data, String);
	bool initDBConnection();

	String get_all_etalon();
	String ins_etalon(String);

	String get_sr_znach();
	String get_max_driv();
	String ins_driv(String, String);

	map<String, int>agents;

	void reload();
	void del_connection(String);
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
