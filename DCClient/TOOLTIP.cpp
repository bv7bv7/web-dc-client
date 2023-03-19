#pragma once
#include <Windows.h>
#include <tchar.h>
#include "..\global\mystd.h"
#include "TOOLTIP.h"


TOOLTIP::TOOLTIP(void) {
	ToolInfo.cbSize = TOOLTIPINFO_SIZE;
}

void TOOLTIP::Init(HWND hWnd, HWND hTool)
{
	TCHAR Text[] = _T("");
	HINSTANCE hInstance = (HINSTANCE) GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	hControl = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, hInstance, NULL);
	ToolInfo.uFlags = TTF_IDISHWND | TTF_TRACK | /*TTF_SUBCLASS | */TTF_ABSOLUTE;
	ToolInfo.uId    = (UINT_PTR) hTool;
	ToolInfo.hwnd = hWnd;
	ToolInfo.hinst = hInstance;
	ToolInfo.lpszText = Text;
	SendMessage(hControl, TTM_ADDTOOL, 0, (LPARAM) &ToolInfo);
}

void TOOLTIP::UpdateTipText(LPCTSTR Message){
	ToolInfo.lpszText = (LPTSTR) Message;
	SendMessage(hControl, TTM_UPDATETIPTEXT, 0, (LPARAM) &ToolInfo);
}

