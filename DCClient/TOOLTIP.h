#pragma once
#include <CommCtrl.h>

#if (_WIN32_WINNT >= 0x0501)
#define TOOLTIPINFO_SIZE TTTOOLINFO_V1_SIZE;
#else
#define TOOLTIPINFO_SIZE sizeof(TOOLINFO);
#endif

class TOOLTIP
{
public:
	TOOLTIP(void);
	void UpdateTipText(LPCTSTR Message);
	void Init(HWND hWnd, HWND hTool);
	HWND hControl;
	TOOLINFO ToolInfo;
};

