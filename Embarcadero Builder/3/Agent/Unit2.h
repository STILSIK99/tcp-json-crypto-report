// ---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdContext.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>
#include <utility>
#include <vector>
// #include <>
using namespace std;

// ---------------------------------------------------------------------------
class TForm2 : public TForm {
__published: // IDE-managed Components
	TTimer *Timer1;
	TIdTCPClient *Client;
	TIdTCPServer *IdTCPServer1;
	TMemo *Memo1;

	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall IdTCPServer1Execute(TIdContext *AContext);

private: // User declarations
public: // User declarations
	__fastcall TForm2(TComponent* Owner);

	pair<long long, long long>current = make_pair(0, 0);

	vector<pair<String, pair<long long, long long> > >otchet;

	String ip = "";
	int send = 0;

};

// ---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
// ---------------------------------------------------------------------------
#endif
