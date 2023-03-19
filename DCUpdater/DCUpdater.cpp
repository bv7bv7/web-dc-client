//#include <WinDef.h>
#include <tchar.h>
#include <Windows.h>
#include <Shlobj.h>
#include "..\global\global.h"
#include "..\global\mystd.h"
#include "..\global\AS.h"
#include "DCUpdater.h"

HWND hWndStatic;
BOOL Exit;
#define CL_FVAMUS _T("/fvamus \"")
void Update(MESSAGE_UPDATE &Upd){
	TCHAR par[MAX_PATH + 64] = CL_FVAMUS;
	_tcscat_s(par, Upd.msi);
	_tcscat_s(par, _T("\"") /*_T(" /qn") */_T(" /norestart"));

	CreateProcessNoWait(CSIDL_SYSTEM, _T("msiexec.exe"), par, NULL, 0);
}
#undef CL_FVAMUS

DWORD WINAPI ThreadFromGUI(LPVOID lphWnd) {
	MESSAGE_UPDATE Upd;
	DWORD size = MAX_PATH;
	SECURITY_DESCRIPTOR SecurityDescriptor;
	SECURITY_ATTRIBUTES SecurityAttributes;
	SecurityAttributes.nLength = sizeof(SecurityAttributes);
	SecurityAttributes.bInheritHandle = TRUE;
	SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
	InitializeSecurityDescriptor(&SecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&SecurityDescriptor, TRUE, (PACL) 0, FALSE);
	HANDLE hPipeFromGUI = CreateNamedPipe(
				_T(PIPE_FROM_GUI_NAME),
				PIPE_ACCESS_INBOUND,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES,
				0,
				size,
				NMPWAIT_WAIT_FOREVER,
				&SecurityAttributes
			);
	DWORD err;
	if (hPipeFromGUI != INVALID_HANDLE_VALUE) {
		for(;;) {
			ConnectNamedPipe(hPipeFromGUI, NULL);
			if (ReadFile(hPipeFromGUI, &Upd, sizeof Upd, &size, NULL) && size == sizeof Upd) {
				Update(Upd);
				break;
			}
			DisconnectNamedPipe(hPipeFromGUI);
		}
	} else
		err = GetLastError();
	CloseHandle(hPipeFromGUI);
	SendMessage(hWndStatic, WM_DESTROY, 0, NULL);
	Exit = TRUE;
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message)
	{
	case WM_CREATE:
		hWndStatic = hWnd;
		CreateThread(NULL, 0, ThreadFromGUI, NULL, 0, NULL);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int       nCmdShow)
{
	
	HANDLE MutexUpdater;
	MutexUpdater = OpenMutex(MUTEX_ALL_ACCESS, false, MUTEX_UPDATER);
	if (MutexUpdater){
	ReleaseMutex(MutexUpdater);
		CloseHandle(MutexUpdater);
		return -1;
	}
	MutexUpdater = CreateMutex(NULL, FALSE, MUTEX_UPDATER);
	
	Exit = FALSE;

	WNDCLASS WndClass;
	ZeroMemory(&WndClass, sizeof WndClass);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = UPDATER_WINDOW_CLASS;
	DWORD err;
	ATOM Atom = RegisterClass(&WndClass);
	if (!Atom)
		err = GetLastError();
	HWND hWnd = CreateWindowEx(0, UPDATER_WINDOW_CLASS, NULL, 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_MESSAGE, NULL, hInstance, NULL);
	if (!hWnd)
		err = GetLastError();
	MSG msg;
	while ((! Exit) && GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ReleaseMutex(MutexUpdater);
	CloseHandle(MutexUpdater);
	return 0;
}
