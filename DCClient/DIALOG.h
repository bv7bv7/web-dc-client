#pragma once
#include <Windows.h>

class DIALOG
{
public:
	BOOL Running;
	DIALOG(UINT Id);
	~DIALOG(void);
	void Run(HWND hWnd);
	void End(INT_PTR nResult = 0);
	virtual void Move() {};
protected:
	static DIALOG * First;
	static DIALOG * Last;
	DIALOG * Next;
	DIALOG * Prev;
	UINT Id;
	HWND hWnd, hWndParent;
	virtual INT_PTR Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void Init();
	virtual void VScroll(UINT message, WPARAM wParam, LPARAM lParam) {};
	virtual void HScroll(UINT message, WPARAM wParam, LPARAM lParam) {};
	virtual void Command(UINT Id);
	virtual void SysCommand(UINT Command);
	virtual void CommandOk() {};
	virtual void Timer(WPARAM wParam) {};
private:
	static INT_PTR CALLBACK StaticProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};

