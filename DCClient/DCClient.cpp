// dcgui.cpp: ���������� ����� ����� ��� ����������.
//
#pragma once
#include <ShlObj.h>
#include "..\global\mystd.h"
#include "..\global\AS.h"
#include "DCGUI_TRAY_ICON.h"
#include "..\CalcService\SERVICE_CALC_CLASS.h"

#define DC_WINDOW_CLASS  _T("DCClientWindowClass")

// ���������� ����������:
HINSTANCE hInst;								   // ������� ���������
DCGUI_TRAY_ICON    TrayIcon;                       // ������ � ��������� ����
HANDLE             hThreadFromService, hThreadToService;
SERVICE_CALC_CLASS App;
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL install;


void Install(){
	TCHAR Cmd[MAX_PATH];
	int pNumArgs;
	AS As;
	LPTSTR *ppArgv = CommandLineToArgvW(GetCommandLine(), &pNumArgs);
	_tcscpy_s(Cmd, ppArgv[0]);
	_tcscat_s(Cmd, _T(" settings"));
	As.Execute(Cmd);
	HeapFree( GetProcessHeap(), 0, ppArgv);
}

void Remove(){
	HWND hWnd = FindWindow(DC_WINDOW_CLASS, NULL);
	if (hWnd)
		SendMessage(hWnd, WM_DESTROY, 0, NULL);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int       nCmdShow)
{
	if (wcscmp(lpCmdLine, _T("install")) == 0) {
		Install();
		return 0;
	} else if (wcscmp(lpCmdLine, _T("remove")) == 0) {
		Remove();
		return 0;
	}
	HANDLE mutex;
	TCHAR *mtxname = _T(DCGUI_ID);
	if (OpenMutex(MUTEX_ALL_ACCESS, false, mtxname) != 0)
		return -1;
	mutex = CreateMutex(NULL, FALSE, mtxname);

	install = wcscmp(lpCmdLine, _T("settings")) == 0;
	InitCommonControls(); 
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ���������� ��� �����.
	MSG msg;

	// ������������� ���������� �����
	ATOM atom = MyRegisterClass(hInstance);

	// ��������� ������������� ����������:
	if (!InitInstance (hInstance)) return FALSE;

	// ���� ��������� ���������:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

    ReleaseMutex(mutex);
	CloseHandle(mutex);
	PostQuitMessage(0);
	return (int) 0;
}



//
//  �������: MyRegisterClass()
//
//  ����������: ������������ ����� ����.
//
//  �����������:
//
//    ��� ������� � �� ������������� ���������� ������ � ������, ���� �����, ����� ������ ���
//    ��� ��������� � ��������� Win32, �� �������� ������� RegisterClassEx'
//    ������� ���� ��������� � Windows 95. ����� ���� ������� ����� ��� ����,
//    ����� ���������� �������� "������������" ������ ������ � ���������� �����
//    � ����.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DCGUI));
	wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DCGUISM));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DCGUI);
	wcex.lpszClassName	= DC_WINDOW_CLASS;

	return RegisterClassEx(&wcex);
}

//
//   �������: InitInstance(HINSTANCE, int)
//
//   ����������: ��������� ��������� ���������� � ������� ������� ����.
//
//   �����������:
//
//        � ������ ������� ���������� ���������� ����������� � ���������� ����������, � �����
//        ��������� � ��������� �� ����� ������� ���� ���������.
//
BOOL InitInstance(HINSTANCE hInstance)
{
   HWND hWnd;
   hInst = hInstance; // ��������� ���������� ���������� � ���������� ����������
   hWnd = CreateWindowEx(0, DC_WINDOW_CLASS, NULL /*szTitle*/, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
   if (!hWnd) return FALSE;
   UpdateWindow(hWnd);
   return TRUE;
}

void Start(){
	if (App.PublicData.User.dwState & STATE_PAUSED) {
		App.PublicData.User.dwState = (App.PublicData.User.dwState | STATE_WATCHDOG) ^ (STATE_WATCHDOG | STATE_PAUSED);
		App.RegSetUserData();
	}
	ResetEvent(App.EventStop);
}

void Stop() {
	if (! (App.PublicData.User.dwState & STATE_PAUSED)) {
		App.PublicData.User.dwState = (App.PublicData.User.dwState | STATE_PAUSED | STATE_WATCHDOG) ^ STATE_WATCHDOG;
		App.RegSetUserData();
	}
	SetEvent(App.EventStop);
}



//
//  �������: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����������:  ������������ ��������� � ������� ����.
//
//  WM_COMMAND	- ��������� ���� ����������
//  WM_PAINT	-��������� ������� ����
//  WM_DESTROY	 - ������ ��������� � ������ � ���������.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFYICON:
		TrayIcon.Proc(lParam);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_OPEN:
			if (App.DlgInfo.Running)
				App.DlgInfo.End();
			else
				App.DlgInfo.Run(hWnd);
			break;
		case IDM_START:
			Start();
			break;
		case IDM_STOP:
			Stop();
			break;
		case IDM_SETTINGS:
			App.DlgSettings.Run(hWnd);
			break;
		case IDM_ABOUT:
			App.DlgAbout.Run(hWnd);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		Stop();
		App.Exit = TRUE;
		WaitForSingleObject(App.hThreadDo, TIME_CLOSE_MAX);
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		TrayIcon.Create(hWnd);
		App.hWnd = hWnd;
		App.Init();
		if (install) {
			install = FALSE;
			SendMessage(hWnd, WM_COMMAND, IDM_SETTINGS, NULL);
		}
		if (App.PublicData.User.tEmail[0] == _T('\0'))
			SET_ERROR_MSG("Not Set email");
		if (App.RegData25.Purse[0] == _T('\0'))
			SET_ERROR_MSG("Not Set Purse");
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

