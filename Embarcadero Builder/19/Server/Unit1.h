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
#include <FireDAC.Phys.MSSQL.hpp>
#include <FireDAC.Phys.MSSQLDef.hpp>
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
#include <map>
bool initDBConnection();

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components

	TMemo *Memo1;
	TButton *Button1;
	TFDConnection *FDConnection1;
	TIdTCPServer *IdTCPServer1;
	TFDTable *FDTable1;
	TFDQuery *FDQuery1;
	TFDTable *FDTable2;
	TFDQuery *FDQuery2;
	TFDTable *FDTable3;
	TFDQuery *FDQuery3;
	TFDTable *FDTable4;
	TFDTable *FDTable5;
	TFDQuery *FDQuery4;
	TFDTable *FDTable6;
	TFDQuery *FDQuery5;
	TFDTable *FDTable7;
	TFDQuery *FDQuery6;
	TFDTable *FDTable8;
	TFDQuery *FDQuery7;
	TFDTable *FDTable9;
	TFDTable *FDTable10;
	TFDTable *FDTable11;
	TFDTable *FDTable12;
	TFDQuery *FDQuery8;
	TFDTable *FDTable13;

	void __fastcall Button1Click(TObject *Sender);
	void __fastcall IdTCPServer1Execute(TIdContext *AContext);

private: // User declarations
		public : // User declarations
	__fastcall TForm1(TComponent* Owner);

	map<TIdContext*, CryptoSession*>clients;

	String decrypt(TIdContext *AContext);
	void send_mes(String request, TIdContext *AContext);
	String log(String data);
	int get_dej(String data);

	String get_all_vid(String data);
	String change_vid(String data);
	String set_vid(String data);
	String del_vid(String data);

	String get_all_gt(String data);
	String change_gt(String data);
	String set_gt(String data);
	String del_gt(String data);

	String get_all_pers(String data);
	String change_pers(String data);
	String set_pers(String data);
	String del_pers(String data);

	String get_all_role(String data);
	String get_all_rang(String data);

	String get_duty(String data);
	String change_duty(String data);
	String set_duty(String data);
	String del_duty(String data);

	String get_zak(String data);
	String change_zak(String data);
	String set_zak(String data);
	String del_zak(String data);

	String get_gun(String data);
	// String change_gun(String data, TIdContext *AContext);
	String set_gun(String data);
	// String del_gun(String data, TIdContext *AContext);

	String get_gun_zak(String data);
	String get_gun_nozak(String data);

	String get_zak_ls(String data);
	String get_gun_from_pers(String data);

	String get_outp(String data);
	String set_outp(String data);

	String get_inp(String data);
	String set_inp(String data);

	String get_ins(String data);
	String set_ins(String data);

	String get_outs(String data);
	String set_outs(String data);

	String get_gun_for_outs(String);
	String set_gun_sklad(String data);

};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
