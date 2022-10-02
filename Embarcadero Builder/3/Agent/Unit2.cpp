// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"

#include <windows.h>
#include <psapi.h>
#include "System.JSON.hpp"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;

// ---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {
	otchet.clear();
}

// ---------------------------------------------------------------------------
void __fastcall TForm2::Timer1Timer(TObject *Sender) {
	pair<long long, long long>np;
	PERFORMANCE_INFORMATION pfi;
	if (GetPerformanceInfo(&pfi, sizeof(PERFORMANCE_INFORMATION))) {
		np.first = static_cast<long long>(pfi.PhysicalTotal)
			*static_cast<long long>(pfi.PageSize);
		np.second = static_cast<long long>(pfi.PhysicalAvailable)
			*static_cast<long long>(pfi.PageSize);
	}
	if (send == 0) {
		return;
	}
	if (np != current) {
		otchet.push_back(make_pair(Now().TimeString(), np));
		current = np;
	}
	if (ip != "" && send == 1) {
		auto req = new TJSONObject;
		req->AddPair("type", "insert");
		req->AddPair("table", "data");
		auto mas = new TJSONArray;
		for (int i = 0; i < otchet.size(); i++) {
			TJSONObject *x = new TJSONObject;
			x->AddPair("total", otchet[i].second.first);
			x->AddPair("avail", otchet[i].second.second);
			x->AddPair("time", otchet[i].first);
			mas->Add(x);
			// x->Free();
		}
		req->AddPair("data", mas);
		Client->Connect(ip, 44445);
		try {

			if (Client->Connected()) {
				Memo1->Text += req->ToString();
				Client->IOHandler->WriteLn(req->ToString(), enUTF8);
				auto js = (TJSONObject*)TJSONObject::ParseJSONValue
					(Client->IOHandler->ReadLn(enUTF8));
				if (String(js->GetValue("answer")->Value().c_str()) == "yes") {
					otchet.clear();
				}
			}
			else {
				send = 0;
				ip = "";
			}

		}
		catch (...) {
			send = 0;
			ip = "";
		}
		Client->Disconnect();
//		mas->Free();
//		req->Free();
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm2::IdTCPServer1Execute(TIdContext *AContext) {
	String req = AContext->Connection->IOHandler->ReadLn(enUTF8);
	Memo1->Text += req;
	auto js = (TJSONObject*)TJSONObject::ParseJSONValue(req);
	auto reply = new TJSONObject;
	reply->AddPair("answer", "yes");
	if (String(js->GetValue("type")->Value().c_str()) == "connection") {
		ip = AContext->Binding->PeerIP;
		send = 0;
	}
	if (String(js->GetValue("type")->Value().c_str()) == "disconnection") {
		ip = "";
		send = 0;
	}
	if (String(js->GetValue("type")->Value().c_str()) == "send_on") {
		send = 1;
		Timer1->Enabled = 1;
		if (ip == "") {
			ip = AContext->Binding->PeerIP;
		}
	}
	if (String(js->GetValue("type")->Value().c_str()) == "send_off") {
		send = 0;
		if (ip == "") {
			ip = AContext->Binding->PeerIP;
		}
	}
	AContext->Connection->IOHandler->WriteLn(reply->ToString());
	AContext->Connection->Disconnect();
	reply->Free();
	js->Free();
}
// ---------------------------------------------------------------------------
