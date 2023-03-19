#include "..\global\mystd.h"
#include "DIALOG.h"

DIALOG * DIALOG::First = NULL;
DIALOG * DIALOG::Last  = NULL;

DIALOG::DIALOG(UINT Id){
	this->Id = Id;
	Running = FALSE;
	Next = NULL;
	Prev = Last;
	if (Last) Last->Next = this;
	Last = this;
	if (! First) First = this;
}

DIALOG::~DIALOG(void)
{
	if (Prev) Prev->Next = Next;
	else First = Next;
	if (Next) Next->Prev = Prev;
	else Last = Prev;
}

INT_PTR CALLBACK DIALOG::StaticProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	if (message == WM_INITDIALOG && lParam) {
		((DIALOG *) lParam)->hWnd = hDlg;
	}
	for(DIALOG * Dialog = First; Dialog; Dialog = Dialog->Next) if (Dialog->hWnd == hDlg) {
		return Dialog->Proc(hDlg, message, wParam, lParam);
	}
	return FALSE;
}

INT_PTR DIALOG::Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
	case WM_INITDIALOG:
		Init();
		break;
	case WM_VSCROLL:
		VScroll(message, wParam, lParam);
		break;
	case WM_HSCROLL:
		HScroll(message, wParam, lParam);
		break;
	case WM_COMMAND:
		Command(LOWORD(wParam));
		break;
	case WM_SYSCOMMAND:
		SysCommand(wParam & 0xFFF0);
		break;
	case WM_TIMER:
		Timer(wParam);
		break;
	case WM_MOVE:
		Move();
		break;
	}
	return FALSE;
}


void DIALOG::Init() {
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, GetClassLongPtr(hWndParent, GCLP_HICONSM));
	SwitchToThisWindow(hWnd, TRUE);
}

void DIALOG::Run(HWND hWnd){
	if (Running) SetActiveWindow(this->hWnd);
	else{
		Running = TRUE;
		hWndParent = hWnd;
		DialogBoxParam((HINSTANCE) GetWindowLongPtr(hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(Id), hWnd, StaticProc, (LPARAM) this);
	}
}

void DIALOG::End(INT_PTR nResult){
	Running = FALSE;
	EndDialog(hWnd, nResult);
}

void DIALOG::Command(UINT Id){
	switch(Id){
	case IDOK:
		CommandOk();
	case IDCANCEL:
		End((INT_PTR) Id);
	}
}

void DIALOG::SysCommand(UINT Command){
	switch(Command) {
	case SC_CLOSE:
		End();
	}
}
