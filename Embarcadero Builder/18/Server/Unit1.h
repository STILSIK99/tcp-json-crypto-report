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
#include <Vcl.DBGrids.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ComCtrls.hpp>
#include <map>

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
	TFDQuery *FDQuery6;
	TFDQuery *FDQuery7;
	TFDQuery *FDQuery8;
	TFDQuery *FDQuery9;
	TFDQuery *FDQuery10;
	TFDQuery *FDQuery11;
	TFDQuery *FDQuery12;
	TFDQuery *FDQuery13;
	TFDQuery *FDQuery14;
	TFDQuery *FDQuery15;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TStringGrid *StringGrid1;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TStringGrid *StringGrid2;
	TGroupBox *GroupBox3;
	TListBox *ListBox1;
	TFDQuery *FDQuery16;

	void __fastcall ServerExecute(TIdContext *AContext);

private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);

	map<TIdContext*, CryptoSession*>clients;

	String decrypt(TIdContext *AContext);
	void send_mes(String request, TIdContext *AContext);
	String log(String data, String ip);
	bool initDBConnection();

	String get_all_files();
	String ins_files(String data);
	String get_source_file(String data);
	String upd_file(String data);
	String del_files(String data);
	String get_file_by_name(String data);

	String get_all_users();
	String ins_users(String data);
	String upd_users(String data);
	String del_users(String data);

	String get_all_history();
	String get_history_user(String data);
	String ins_history(String data);
	String get_filtred(String data);

	String formatDate(String);

	void loadData();
	void clear();
	void loadFiles();
	void loadHistory();
	void loadAgs();

	map<String, String>active;

};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
