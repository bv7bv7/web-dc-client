#include "AS.h"
#include <TCHAR.h>
#include <Sddl.h>
#include <Wtsapi32.h>

#ifdef _DEBUG
#include <stdio.h>
#endif

#include "global.h"

#pragma comment( lib, "Wtsapi32.lib")

AS::AS(void)
{
}


AS::~AS(void)
{
}

BOOL AS:: Init(){
	BOOL Result = FALSE;
	Token  = 0;
	CurDst = 0;
	Dst0   = 0;
	CurWst = GetProcessWindowStation();
	if (CurWst) {
		Wst0   = OpenWindowStation(_T("WinSta0"), false, MAXIMUM_ALLOWED);
		if (Wst0) {
			CurDst = GetThreadDesktop(GetCurrentThreadId());
			if (CurDst) {
				SetProcessWindowStation(Wst0);
				Dst0 = OpenDesktop(_T("Default"), 0, false, MAXIMUM_ALLOWED);
				if (Dst0) {
					SetThreadDesktop(Dst0);
					DWORD Cons = WTSGetActiveConsoleSessionId();
					DWORD err = 0;
					if (Cons != 0xFFFFFFFF) {
						if (! WTSQueryUserToken(Cons, &Token))
							err = GetLastError();
					}
#ifdef _DEBUG
				FILE * fLog;
				DWORD Err = _wfopen_s(&fLog, _T("c:\\test\\sid2.log"), _T("at"));
				fwprintf(fLog, _T("\n%s\nCons\terr\n%I32X\t%I32X\n"), _T("WTSQueryUserToken"), (__int32)Cons, (__int32)err);
				fclose(fLog);
#endif



					Result = Token != 0;
				}
			}
		}
	}
	if (! Result)
		Done();
	return Result;
}

void AS::Done(){
	if (CurWst)	{
		if (Wst0) {
			if (CurDst) {
				if (Dst0) {
					if (Token) {
						CloseHandle(Token);
						Token = 0;
					}
					SetThreadDesktop(CurDst);
					CloseDesktop(Dst0);
					Dst0 = 0;
				}
				SetProcessWindowStation(CurWst);
				CloseDesktop(CurDst);
				CurDst = 0;
			}
			CloseWindowStation(Wst0);
			Wst0 = 0;
		}
		CloseWindowStation(CurWst);
		CurWst = 0;
	}
}

void AS::Execute(LPTSTR PrcName){
	STARTUPINFO SI;
	PROCESS_INFORMATION PI;
	if (Init()){
		ZeroMemory(&SI, sizeof(SI));
		if (CreateProcessAsUser(Token, NULL, PrcName, NULL, NULL, FALSE, 0, NULL, NULL, &SI, &PI)) {
			CloseHandle(PI.hProcess);
			CloseHandle(PI.hThread);
		}
		Done();
	}
}

BOOL WINAPI AS::EnumDskWindows(HWND Wnd, LPARAM lpToken) {
	DWORD ID = 0;
	HANDLE hPrc;
	LPVOID Sid;
	HANDLE &Token = * (LPHANDLE) lpToken;
	GetWindowThreadProcessId(Wnd, &ID);
	hPrc = OpenProcess(MAXIMUM_ALLOWED, false, ID);
	BOOL Result = hPrc == 0;
	if (! Result) {
		Result = ! OpenProcessToken(hPrc, TOKEN_ALL_ACCESS, &Token);
		if (! Result) {
			Sid = AS::GetLogonSid(Token);
			Result = Sid == NULL;
			if (! Result) 
				CoTaskMemFree(Sid);
			else {
				CloseHandle(Token);
				Token = 0;
			}
		}
		CloseHandle(hPrc);
	}
	return Result;
}

LPVOID WINAPI AS::GetLogonSid(HANDLE Token) {
	DWORD SidLen;
	PTOKEN_GROUPS Groups;
	LPVOID Result = NULL;

	PSID InteractSid;
	if(ConvertStringSidToSid(SID_INTERACTIVE, &InteractSid)) {
		if (CheckTokenInteractive(Token)) {

			Groups = (PTOKEN_GROUPS) AllocateTokenInformation(Token, TokenGroups);
			if (Groups) {


#ifdef _DEBUG
				FILE * fLog;
				DWORD Err = _wfopen_s(&fLog, _T("c:\\test\\sid.log"), _T("at"));
				fwprintf(fLog, _T("\n%s\n#\tAttr\tSid ID\tSid Str\n"), _T("SIDS"));
				for (DWORD n = 0; n < Groups->GroupCount; n++) {
					LPTSTR s;
					ConvertSidToStringSid(Groups->Groups[n].Sid, &s);
					fwprintf(fLog, _T("%I32u\t%I64X\t%I64X\t%s\n"), (__int32)n, (__int64)Groups->Groups[n].Attributes, (__int64)Groups->Groups[n].Sid, s);
					LocalFree(s);
				}
				fclose(fLog);
#endif

				for (DWORD n = 0; n < Groups->GroupCount; n++) {
					if (Groups->Groups[n].Attributes & SE_GROUP_LOGON_ID) {
						SidLen = GetLengthSid(Groups->Groups[n].Sid);
						Result = CoTaskMemAlloc(SidLen);
						if (! Result) break;
						if (! CopySid(SidLen, Result, Groups->Groups[n].Sid)) {
							CoTaskMemFree(Result);
							Result = NULL;
						}
						break;
					}
				}
				CoTaskMemFree(Groups);
			}
		}
	}
	return Result;
}

LPVOID WINAPI AS::AllocateTokenInformation(HANDLE Token, TOKEN_INFORMATION_CLASS TokenClass) {
	DWORD Size = 0;
	DWORD Res;
	LPVOID Result = NULL;
	do {
		Result = CoTaskMemRealloc(Result, Size);
		if (GetTokenInformation(Token, TokenClass, Result, Size, &Size))
			Res = ERROR_SUCCESS;
		else
			Res = GetLastError();
	} while (Res == ERROR_INSUFFICIENT_BUFFER);
	if ((Res != ERROR_SUCCESS) && Result) {
		CoTaskMemFree(Result);
		Result = NULL;
	}
	return Result;
}

BOOL WINAPI AS::CheckTokenInteractive(HANDLE Token) {
	PSID InteractSid;
	BOOL Result = FALSE;
	PTOKEN_GROUPS Groups = (PTOKEN_GROUPS) AllocateTokenInformation(Token, TokenGroups);
	if (Groups) {
		Result = ConvertStringSidToSid(SID_INTERACTIVE, &InteractSid);
		if (Result) {
			Result = FALSE;

#ifdef _DEBUG
				FILE * fLog;
				DWORD Err = _wfopen_s(&fLog, _T("c:\\test\\sid.log"), _T("at"));
				fwprintf(fLog, _T("\n%s\n#\tAttr\tSid ID\tSid Str\n"), _T("SIDS"));
				for (DWORD n = 0; n < Groups->GroupCount; n++) {
					LPTSTR s;
					ConvertSidToStringSid(Groups->Groups[n].Sid, &s);
					fwprintf(fLog, _T("%I32u\t%I64X\t%I64X\t%s\n"), (__int32)n, (__int64)Groups->Groups[n].Attributes, (__int64)Groups->Groups[n].Sid, s);
					LocalFree(s);
				}
				fclose(fLog);
#endif

			for(DWORD n = 0; n < Groups->GroupCount; n++) {
				Result = EqualSid(InteractSid, Groups->Groups[n].Sid);
				if (Result)
					break;
			}
			LocalFree(InteractSid);
		}
		CoTaskMemFree(Groups);
	}
	return Result;
}