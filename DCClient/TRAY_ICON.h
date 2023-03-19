#pragma once
#include <Windows.h>
#include <Shellapi.h>

#define WM_NOTIFYICON (WM_USER + 0x0100)

#define MAX_TIP ARRAY_LEN(NOTIFYICONDATA::szTip)

class TRAY_ICON
{
public:
	TRAY_ICON(LPCTSTR Tip);
	~TRAY_ICON(void);
	void Create(HWND hWnd);
	void Destroy();
	void Proc(LPARAM lParam);
protected:
	static DWORD Count;
	NOTIFYICONDATA NotificationData;
	HMENU hMenu;
	virtual void LButtonDown();
	virtual void RButtonDown();
	virtual void LButtonUp() {};
};

