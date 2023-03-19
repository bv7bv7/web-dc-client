#pragma once
#include <tchar.h>
#include "DCGUI_TRAY_ICON.h"
#include "resource.h"


DCGUI_TRAY_ICON::DCGUI_TRAY_ICON(void):TRAY_ICON(_T("Distributed Calculations\nРаспределенные вычисления"))
{
}


DCGUI_TRAY_ICON::~DCGUI_TRAY_ICON(void)
{
}

void DCGUI_TRAY_ICON::LButtonDown(){
	SendMessage(NotificationData.hWnd, WM_COMMAND, MAKELONG(IDM_OPEN, 0), 0);
}
