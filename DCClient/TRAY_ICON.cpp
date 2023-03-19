#pragma once
#include <tchar.h>
#include "..\global\mystd.h"
#include "TRAY_ICON.h"
#include "resource.h"

DWORD TRAY_ICON::Count = 0;

TRAY_ICON::TRAY_ICON(LPCTSTR Tip) {
	NotificationData.cbSize = sizeof(NOTIFYICONDATA);
	NotificationData.uID    = Count;
	Count++;
	NotificationData.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	NotificationData.uCallbackMessage = WM_NOTIFYICON; // Сообщение, которое пошлется если курсор над иконкой или клик по ней
	_tcscpy_s(NotificationData.szTip, _countof(NotificationData.szTip), Tip);
}

TRAY_ICON::~TRAY_ICON(void) {
	Shell_NotifyIcon(NIM_DELETE, &NotificationData); 
	Count--;
}

void TRAY_ICON::Create(HWND hWnd){
	NotificationData.hWnd  = hWnd;
	NotificationData.hIcon = (HICON) GetClassLongPtr(hWnd, GCLP_HICONSM);
	Shell_NotifyIcon(NIM_ADD, &NotificationData);
	hMenu = GetSubMenu(GetMenu(hWnd), 0);
	SetMenuDefaultItem(hMenu, 0, TRUE);
}

void TRAY_ICON::Proc(LPARAM lParam) {
	switch (lParam) {
	case WM_LBUTTONDOWN:
		LButtonDown();
		break;
	case WM_RBUTTONDOWN:
		RButtonDown();
		break;
	case WM_LBUTTONUP:
		LButtonUp();
		break;
	case WM_LBUTTONDBLCLK:
		break;
	}
}

void TRAY_ICON::LButtonDown(){
	SendMessage(NotificationData.hWnd, WM_COMMAND, IDM_OPEN, NULL);
}

void TRAY_ICON::RButtonDown(){
	POINT curPoint ;
	GetCursorPos(&curPoint);
	UINT clicked = TrackPopupMenu(hMenu, 0, curPoint.x, curPoint.y, 0, NotificationData.hWnd, NULL);
}

