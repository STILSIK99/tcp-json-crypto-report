// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "IdHashMessageDigest.hpp"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	cryptoSession = new CryptoSession("process");

	send = 0;
	update = 1;

	StringGrid1->Cells[0][0] = "№";
	StringGrid1->Cells[1][0] = "Название процесса";
	StringGrid1->Cells[2][0] = "Объем выделенной памяти";
	StringGrid1->ColWidths[0] = 100;
	StringGrid1->ColWidths[1] = 400;
	StringGrid1->ColWidths[2] = 400;
	getProcessNames();
}

// ---------------------------------------------------------------------------
String TForm1::send_mes(String request) {
	try {
		// ShowMessage(request);
		Client->Connect(Edit1->Text, Edit2->Text.ToInt());
		if (!Client->Connected()) {
			// ShowMessage("Соединение отсутствует!");
			send = 0;
			return "";
		}
		char* crypted = 0;
		unsigned long size = request.Length() * 2 + 2;
		cryptoSession->CryptData((const char*)request.data(), crypted, size);
		TMemoryStream * ms = new TMemoryStream;
		ms->Write(crypted, size);
		delete[]crypted;
		ms->Seek(0, 0);
		Client->IOHandler->WriteBufferOpen();
		Client->IOHandler->Write(ms, -1, true);
		Client->IOHandler->WriteBufferClose();
		ms->Free();
		return decrypt();
	}
	catch (...) {
		// ShowMessage("Ошибка сообщения!");
		Client->Disconnect();
		send = 0;
		return "";
	}
};

String TForm1::decrypt() {
	TMemoryStream * ms = new TMemoryStream;
	Client->IOHandler->ReadStream(ms);
	unsigned long size = ms->Size;
	char * crypted = new char[size];
	ms->Seek(0, 0);
	ms->ReadBuffer(crypted, size);
	char * decrypted = 0;
	cryptoSession->DecryptData(crypted, decrypted, size);
	ms->Free();
	Client->Disconnect();
	return String((wchar_t*)decrypted);
};

void __fastcall TForm1::Button1Click(TObject *Sender) {
	update = (update + 1) % 2;
	switch (update) {
	case 0:
		Button1->Caption = "Обновлять";
		break;
	case 1:
		Button1->Caption = "Не обновлять";
		break;
	default: ;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender) {
	try {
		simple_ans answer(send_mes(com("test", "", 0, "").ToJson()));
		if (answer.key == 123) {
			ShowMessage("Сервер подключен");
			return;
		}
	}
	catch (...) {
		Client->Disconnect();
		send = 0;
	}
	ShowMessage("Нет соединения");
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	int n;
	try {
		n = Edit3->Text.ToInt();
		Edit3->Text = "";
	}
	catch (...) {
		ShowMessage("Неверные данные");
		return;
	}
	String cap = "Период сбора данных - " + String(n) + " секунд(ы)";
	GroupBox5->Caption = cap;
	Timer1->Interval = 1000 * n;
}

// ---------------------------------------------------------------------------
int TForm1::getMemory(unsigned long pid) {
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS_EX pmc;
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE, pid);
	if (NULL == hProcess)
		return 0;
	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	CloseHandle(hProcess);
	return physMemUsedByMe;
};

void TForm1::getProcessNames() {

	procMas.clear();
	PROCESSENTRY32* processInfo = new PROCESSENTRY32;
	processInfo->dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	String time = Now().TimeString();
	Label1->Caption = "Последнее обновление: " + time;
	while (Process32Next(hSnapShot, processInfo) != FALSE) {
		procMas.push_back(Process(String(processInfo->szExeFile),
			String((getMemory(processInfo->th32ProcessID) + 512) / 1024)));
	}
	CloseHandle(hSnapShot);
	if (update == 1) {
		showData();
	}
	ans_Proc ans;
	ans.arr = procMas;
	ans.time = time;
	if (send == 1) {
		ListBox1->Items->Add(time + " - " + simple_ans(send_mes(com("data",
			"insert", 0, ans.ToJson()).ToJson())).result);
	}

}

void __fastcall TForm1::Timer1Timer(TObject *Sender) {
	getProcessNames();
}

// ---------------------------------------------------------------------------
void TForm1::showData() {
	if (update == 1) {
		for (int i = 1; i < StringGrid1->RowCount; i++) {
			for (int j = 0; j < StringGrid1->ColCount; j++) {
				StringGrid1->Cells[j][i] = "";
			}
		}
		StringGrid1->RowCount = 1;
		for (auto x : procMas) {
			int id = StringGrid1->RowCount++;
			StringGrid1->Cells[0][id] = String(id);
			StringGrid1->Cells[1][id] = x.name;
			StringGrid1->Cells[2][id] = x.memory + " КБайт";
		}
	}

};

void __fastcall TForm1::IdTCPServer1Execute(TIdContext *AContext) {
	simple_ans ans(AContext->Connection->IOHandler->ReadLn());

	if (ans.result == "turn_on") {
		if (send == 0) {
			if (Application->MessageBox(_T("Отправлять  данные?"),
				_T("Запрос от менеджера"),
				MB_YESNO + MB_TOPMOST + MB_ICONQUESTION) == IDYES) {
				simple_ans a("yes", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
				send = 1;
			}
			else {
				simple_ans a("no", 0);
				AContext->Connection->IOHandler->WriteLn(a.ToJson());
			}
		}
	}

	if (ans.result == "turn_off") {
		send = 0;
		ShowMessage("Отправка данных выключена");
	}

	AContext->Connection->Disconnect();
}

// ---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action) {
	try {
		send_mes(com("", "close", 0, "").ToJson());
	}
	catch (...) {
	}
}
// ---------------------------------------------------------------------------
