#pragma once
#include <Windows.h>

#define	SID_INTERACTIVE _T("S-1-5-4")

class AS
{
public:
	AS(void);
	~AS(void);
	void Execute(LPTSTR PrcName);
private:
	HANDLE  Token;
	HWINSTA CurWst;
	HDESK   CurDst;
	HWINSTA Wst0;
	HDESK   Dst0;
	static BOOL   WINAPI EnumDskWindows(HWND Wnd, LPARAM Token);
	static LPVOID WINAPI GetLogonSid(HANDLE Token);
	static LPVOID WINAPI AllocateTokenInformation(HANDLE Token, TOKEN_INFORMATION_CLASS TokenClass);
	static BOOL   WINAPI CheckTokenInteractive(HANDLE Token);
	BOOL Init();
	void Done();
};

