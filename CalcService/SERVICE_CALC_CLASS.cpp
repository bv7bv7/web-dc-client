#pragma once
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment( lib, "Wininet.lib")
#include <Windows.h>
#include <stdio.h>
#include <WinInet.h>
#include <time.h>
#include <WinCrypt.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <strsafe.h>
#include <ShlObj.h>
#include "SERVICE_CALC_CLASS.h"

DWORD          SERVICE_CALC_CLASS::ErrorCode = 0;
TCHAR          SERVICE_CALC_CLASS::TempPath[_countof(SERVICE_CALC_CLASS::TempPath)];
TCHAR          SERVICE_CALC_CLASS::PrgPath[_countof(SERVICE_CALC_CLASS::PrgPath)];
FILE_NAME      SERVICE_CALC_CLASS::tErrorLogFile;
TCHAR          SERVICE_CALC_CLASS::LastErrorMsg[_countof(SERVICE_CALC_CLASS::LastErrorMsg)];
DCGUI_DLG_INFO SERVICE_CALC_CLASS::DlgInfo;              // Диалог статистики

DWORD SERVICE_CALC_CLASS::ErrorT(char * Func, DWORD Line, DWORD Code, TCHAR * Message) {
	char Msg[ERROR_MAX];
	ASTR_S(Msg, _countof(Msg), Message);
	return Error(Func, Line, Code, Msg);
}

DWORD SERVICE_CALC_CLASS::Error(char * Func, DWORD Line, DWORD Code, char * Message) {
	ErrorCode = Code;
	if (Code != ERROR_SUCCESS || Message) {
		TCHAR s[ERROR_MAX] = _T("");
		time_t t;
		TCHAR s2[32];
		tm    Tm;
		if (time(&t) != (time_t) -1 && localtime_s(&Tm, &t) == 0)
			_tcsftime(s, _countof(s), _T("%Y-%m-%d %H:%M:%S "), &Tm);
		TSTR(s2, Func);
		_tcscat_s(s, s2);
		_tcscat_s(s, _T(" ("));
		_ui64tot_s(Line, s2, ARRAY_LEN(s2), 10);
		_tcscat_s(s, s2);
		_tcscat_s(s, _T(") "));
		if (Code != ERROR_SUCCESS) {
			_ui64tot_s(Code, s2, ARRAY_LEN(s2), 10);
			_tcscat_s(s, s2);
			_tcscat_s(s, _T(" "));
		}
		size_t l = _tcslen(s);
		if (Message) {
			TSTR_S(s + l, _countof(s) - (int) l - _countof(ERROR_EOL) + 1, Message);
			if (Code == ERROR_SUCCESS)
				ErrorCode = -1;
		} else {
			LPTSTR lpBuff;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, Code, 0, (LPTSTR) &lpBuff, 0, NULL);
			_tcscat_s(s, lpBuff);
			LocalFree(lpBuff);
		}
		_tcscat_s(s, ERROR_EOL);
		l = _tcslen(s);
		DWORD sl = (DWORD) l * sizeof(s[0]);
		HANDLE hFile = CreateFile(tErrorLogFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			SetFilePointer(hFile, 0, NULL, FILE_END);
			DWORD dw;
			WriteFile(hFile, s, (DWORD) sl, &dw, NULL);
			CloseHandle(hFile);
		}
		if (l >= _countof(LastErrorMsg))
			sl = (DWORD) (l = _countof(LastErrorMsg) - 1) * sizeof(LastErrorMsg[0]);
		LastErrorMsg[_countof(LastErrorMsg) - 1 - l] = _T('\0');
			
		memmove_s(LastErrorMsg + l, sizeof(LastErrorMsg) - sl, LastErrorMsg, (_tcslen(LastErrorMsg) + 1) * sizeof(LastErrorMsg[0]));
		memcpy_s(LastErrorMsg, sizeof(LastErrorMsg), s, sl);
		DlgInfo.ShowData();
	}
	return ErrorCode;
}

DWORD WINAPI SERVICE_CALC_CLASS::StaticDo(LPVOID lpService) {
	((SERVICE_CALC_CLASS *) lpService)->Do();
	PostQuitMessage(0);
	return TRUE;
}

SERVICE_CALC_CLASS::SERVICE_CALC_CLASS() {
	DlgInfo.lpApp     = this;
	DlgSettings.lpApp = this;
	DlgAbout.lpApp    = this;
	Servers.lpApp     = this;
	hcl.lpApp         = this;
	LastErrorMsg[0]   = _T('\0');
	ErrorCode         = ERROR_SUCCESS;
	if (! GetModuleFileName(NULL, PrgPath, MAX_PATH)) 
		SET_ERROR;
	else {
		// Папка размещения службы и программ
		PathRemoveFileSpec(PrgPath);
		SetCurrentDirectory(PrgPath);
	}

	if (! GetTempPath(MAX_PATH, TempPath)) SET_ERROR;
	else if (! CreateDirectory(TempPath, NULL) && (ErrorCode = GetLastError()) != ERROR_ALREADY_EXISTS) SET_ERROR_CODE(ErrorCode);
	else if (! PathAppend(TempPath, MANUFACTURER)) SET_ERROR_MSG("PathAppend Error");
	else if (! CreateDirectory(TempPath, NULL) && (ErrorCode = GetLastError()) != ERROR_ALREADY_EXISTS) SET_ERROR_CODE(ErrorCode);
	else if (! PathAppend(TempPath, APP_NAME _TCH(V0))) SET_ERROR_MSG("PathAppend Error");
	else if (! CreateDirectory(TempPath, NULL) && (ErrorCode = GetLastError()) != ERROR_ALREADY_EXISTS) SET_ERROR_CODE(ErrorCode);
	if (SUCCEEDED(GetSysPath(CSIDL_COMMON_APPDATA, CommonAppDataFolder))) {
		if (! PathAppend(CommonAppDataFolder, MANUFACTURER)) SET_ERROR_MSG("PathAppend Error");
		else if (! CreateDirectory(CommonAppDataFolder, NULL) && (ErrorCode = GetLastError()) != ERROR_ALREADY_EXISTS) SET_ERROR_CODE(ErrorCode);
		else if (! PathAppend(CommonAppDataFolder, APP_NAME _TCH(V0))) SET_ERROR_MSG("PathAppend Error");
		else if (! CreateDirectory(CommonAppDataFolder, NULL) && (ErrorCode = GetLastError()) != ERROR_ALREADY_EXISTS) SET_ERROR_CODE(ErrorCode);
		// Инициализация имени лог-файла
		if (FAILED(StringCchCopy(tErrorLogFile, _countof(tErrorLogFile), CommonAppDataFolder)))
			SET_ERROR_MSG("StringCchCopy Error");
		else {
			if (! PathAppend(tErrorLogFile, _T("Errors.log")))
				SET_ERROR_MSG("PathAppend Error");
		}
	};
	Exit                  = FALSE;
	CalcErrorMessage[0] = _T('\0');
	ScriptAltState = SCRIPT_ALT_UNDEF;
	InitScript(&RegData10021.ScriptAltRun);
	InitScript(&RegData10021.ScriptAltStop);
	RegData10021.ProcessesAlt.Enable = false;
	_tcscpy_s(RegData10021.ProcessesAlt.Name, _T("poclbm phoenix guiminer "));
	RegData10023.SecondsStat = STAT_DEFAULT;
	if (GetProcessAffinityMask(GetCurrentProcess(), &RegData25.CPUAffinityMask, &SystemAffinityMask) == 0) {
		SystemAffinityMask        = 0;
		RegData25.CPUAffinityMask = 0;
	}
	Total.mh            = 0;
	Total.TimeBegin     = 0;
	Total.TimeBeginCalc = 0;
	mhWaited            = 0;
	mhAccounted         = 0;
	mhPrev              = 0;
	moneyAccounted      = 0;
	moneyPrev           = 0;
	PriceGhs            = 0;
	EventReconfig = CreateEvent(NULL, FALSE, FALSE, NULL);
	EventStop = CreateEvent(NULL, FALSE, FALSE, NULL);
	Update.State  = UPDATE_OFF;
	Update.Url[0] = _T('\0');
}

void ConverPublicDataTo7(PUBLIC_DATA7 &PublicData) {
	if (PublicData.ver < 7) {
		PUBLIC_DATA6 PublicData6;
		memcpy_s(&PublicData6, sizeof PublicData6, (const void *) &PublicData, MIN(sizeof PublicData6, sizeof PublicData));
		if (PublicData6.RunType == PublicData6.RUN_TYPE_CU || PublicData6.RunType == PublicData6.RUN_TYPE_MN) {
			PublicData.Proxy.dwEnable = PublicData6.Proxy.dwEnable;
			_tcscpy_s(PublicData.Proxy.tHTTP, PublicData6.Proxy.tHTTP);
			_tcscpy_s(PublicData.Proxy.tUser, PublicData6.Proxy.tUser);
			_tcscpy_s(PublicData.Proxy.tPass, PublicData6.Proxy.tPass);
			PublicData.User.dwState = PublicData6.User.dwState;
			_tcscpy_s(PublicData.User.tEmail, PublicData6.User.tEmail);
			_tcscpy_s(PublicData.User.tWM,    PublicData6.User.tWM);
			PublicData.Watchdog            = PublicData6.Watchdog; // Максимальная температура вычислителя
			PublicData.RunType             = (PUBLIC_DATA7::RUN_TYPE) PublicData6.RunType;
			_tcscpy_s(PublicData.UserName, PublicData6.UserName);
			for (__int8 i = 0; i < MAX_GPU7; i++) {
				PublicData.GPU[i].Use  = PublicData6.UseGPU[i];
			}
		} else {
			PublicData.RunType  = PublicData.RUN_TYPE_MN;
			PublicData.Watchdog = WATCHDOG_DEFAULT;
		}
		PublicData.WatchdogUse = TRUE;
		PublicData.ver         = 7;
	}
}

void ConverPublicDataTo8(PUBLIC_DATA8 &PublicData) {
	if (PublicData.ver < 8) {
		PUBLIC_DATA7 PublicData7;
		memcpy_s(&PublicData7, sizeof PublicData7, (const void *) &PublicData, MIN(sizeof PublicData7, sizeof PublicData));
		ConverPublicDataTo7(PublicData7);
		PublicData.Proxy.dwEnable = PublicData7.Proxy.dwEnable;
		memcpy_s(&PublicData.Proxy, sizeof PublicData.Proxy, (const void *) &PublicData7.Proxy, sizeof PublicData7.Proxy);
		memcpy_s(&PublicData.User,  sizeof PublicData.User,  (const void *) &PublicData7.User,  sizeof PublicData7.User);
		PublicData.Watchdog            = PublicData7.Watchdog; // Максимальная температура вычислителя
		PublicData.RunType             = (PUBLIC_DATA8::RUN_TYPE) PublicData7.RunType;
		memcpy_s(&PublicData.UserName, sizeof PublicData.UserName, (const void *) &PublicData7.UserName, sizeof PublicData7.UserName);
		for (__int8 i = 0; i < MAX_GPU7; i++) {
			PublicData.GPU[i].Use  = PublicData7.GPU[i].Use;
		}
		PublicData.WatchdogUse = PublicData7.WatchdogUse;
		PublicData.ver = 8;
	}
}

void ConverPublicDataTo9(PUBLIC_DATA9 &PublicData) {
	if (PublicData.ver < 9) {
		PUBLIC_DATA7 PublicData7;
		memcpy_s(&PublicData7, sizeof PublicData7, (const void *) &PublicData, MIN(sizeof PublicData7, sizeof PublicData));
		ConverPublicDataTo8(* (PUBLIC_DATA8 *) &PublicData);
		PublicData.Test          = FALSE;
		PublicData.ChangeServers = TRUE;
		PublicData.ver           = 9;
	}
}

DWORD WINAPI SERVICE_CALC_CLASS::StaticUpdater(LPVOID lpService) {
	((SERVICE_CALC_CLASS *) lpService)->Updater();
	return TRUE;
}
void SERVICE_CALC_CLASS::Updater(){
	FILE_NAME Url;
	WGET_DATA WgetData;
	FILE_NAME temp;
	DWORD cbSize;
	cbSize = _countof(Url);
	InternetCanonicalizeUrl(Update.Url, Url, &cbSize, ICU_DECODE);
	WgetData.lpUrl = Url;
	WgetData.UseCookies = false;
	_stprintf_s(WgetData.CommandPars, _countof(WgetData.CommandPars), _T("-N -P \"%s\""), TempPath);
	Update.State = UPDATE_DOWNLOAD;
	DlgInfo.ShowData();
	if (wg(WgetData)) {
		Update.State = UPDATE_RUN;
		DlgInfo.ShowData();
		_tcscpy_s(temp, TempPath);
		PathAppend(temp, PathFindFileName(Url));
		if(DoScript(temp)){
			if(PostMessage(hWnd, WM_COMMAND, IDM_EXIT, NULL) == 0)
				SET_ERROR_MSG("ERROR: Error close client");
		}else{
			SET_ERROR_MSG("ERROR:: Error run update");
			Update.State = UPDATE_ENABLE;
			DlgInfo.ShowData();
		}
	} else {
		SET_ERROR_MSG("ERROR: Error download update");
		Update.State = UPDATE_ENABLE;
		DlgInfo.ShowData();
	}
}
void SERVICE_CALC_CLASS::UpdateStart() {
	hThreadUpdater = CreateThread(NULL, 0, StaticUpdater, this, 0, NULL);
}


void SERVICE_CALC_CLASS::InitScript(LPFILER_DATA lpData)
{
	lpData->Enable = false;
	lpData->Name[0] = _T('\0');
}

bool SERVICE_CALC_CLASS::DoScript(FILE_NAME FileName, bool Wait, bool SystemAffinity)
{
	LPTSTR s = _tcsrchr(FileName, _T('.'));
	bool p = s != 0;
	s++;
	return p && _tcsicmp(s, _T("exe")) == 0? ExeRun(FileName, Wait, SystemAffinity): ScriptRun(FileName, Wait, SystemAffinity);
}

void SERVICE_CALC_CLASS::DoScriptAltRun() {
	if ((ScriptAltState != SCRIPT_ALT_RUNNING) && RegData10021.ScriptAltRun.Enable) {
#if V2 >= 30
		DoScript(RegData10021.ScriptAltRun.Name, false, true);
#else
		DoScript(RegData10021.ScriptAltRun.Name, true, true);
#endif
		ScriptAltState = SCRIPT_ALT_RUNNING;
	}
}

void SERVICE_CALC_CLASS::DoScriptAltStop() {
	if (ScriptAltState != SCRIPT_ALT_STOPPED) {
		if (RegData10021.ScriptAltRun.Enable)
			RunProcessesKill();
		if (RegData10021.ScriptAltStop.Enable && DoScript(RegData10021.ScriptAltStop.Name, true, true))
			ScriptAltState = SCRIPT_ALT_STOPPED;
		if (RegData10021.ProcessesAlt.Enable)
			ProcessesKill();
	}
}

void SERVICE_CALC_CLASS::ProcessesKill() {
	FILE_NAME process;
	LPTSTR ps = RegData10021.ProcessesAlt.Name;
	LPTSTR p;
	do {
		for(p = process; *ps >= _T('0') && *ps <= _T('9') || *ps >= _T('A') && *ps <= _T('Z') || *ps >= _T('a') && *ps <= _T('z') || *ps == _T('_') || *ps == _T('.'); p++, ps++)
			* p = * ps;
		*p = _T('\0');
		if (process[0] != _T('\0')) {
			KillProcess(process, true);
			_tcscat_s(process, _T(".exe"));
			KillProcess(process, true);
		}
	} while (* ps++ != _T('\0'));
	ScriptAltState = SCRIPT_ALT_STOPPED;
}

void SERVICE_CALC_CLASS::RunProcessesKill() {
	LPTSTR s = _tcsrchr(RegData10021.ScriptAltRun.Name, _T('.'));
	if (s) {
		s++;
		if (_tcsicmp(s, _T("exe")) == 0) {
			s = _tcsrchr(RegData10021.ScriptAltRun.Name, _T('\\'));
			if (s)
				s++;
			else
				s = RegData10021.ScriptAltRun.Name;
			KillProcess(s, true);
		}
	}
	ScriptAltState = SCRIPT_ALT_STOPPED;
}

// i - ID, e - EMail, p - Purse, d - Version of List of Servers
inline void SERVICE_CALC_CLASS::WGFillPars2(WG_PARS & Pars) {
	union {
		TCHAR SrvVer[LEN10_VAR(Servers.Reg.Ver)];
	} tTmp;
	_tcscpy_s(Pars, _T("--post-data=\"i="));
	_tcscat_s(Pars, Servers.Reg.Items[Servers.Current].IDData.tID);

	if (Servers.Reg.Items[Servers.Current].ChangeUser) {
		if (PublicData.User.tEmail[0]) {
			_tcscat_s(Pars, _T("&e="));
			_tcscat_s(Pars, PublicData.User.tEmail);
		}
		if (RegData25.Purse[0]) {
			_tcscat_s(Pars, _T("&p="));
			_tcscat_s(Pars, RegData25.Purse);
		}
	}
	_itot_s(Servers.Reg.Ver, tTmp.SrvVer, _countof(tTmp.SrvVer), 10);
	_tcscat_s(Pars, _T("&d="));
	_tcscat_s(Pars, tTmp.SrvVer);
}

// ..., w - Password
inline void SERVICE_CALC_CLASS::WGFillPars1(WG_PARS & Pars){
	WGFillPars2(Pars);
	_tcscat_s(Pars, _T("&w="));
	_tcscat_s(Pars, Servers.Reg.Items[Servers.Current].IDData.tPass);
}

// h - Temps Average, mh - MH/s, m - Temps Max, b - Fan Max, c - Fan Average, f - Util Max, c - Fan Average
inline void SERVICE_CALC_CLASS::WGFillPars3(WG_PARS & Pars){
	union {
		TCHAR Speed[LEN10_VAR(Speed)];
		TCHAR Temp[LEN10_VAR(TEMP) + 1];
		TCHAR Fan[LEN10_VAR(FAN) + 1];
		TCHAR Util[LEN10_VAR(UTIL) + 1];
	} tTmp;
	// h - Temps Average
	_tcscat_s(Pars, _T("&h="));
	_itot_s(GPU[0].Temp.Average, tTmp.Temp, 10);
	_tcscat_s(Pars, tTmp.Temp);
	tTmp.Temp[0] = _T('z');
	for (__int8 i = 1; i < MAX_GPU; i++) {
		_itot_s(GPU[i].Temp.Average, tTmp.Temp + 1, _countof(tTmp.Temp) - 1, 10);
		_tcscat_s(Pars, tTmp.Temp);
	}
	// j - MH/s
	_tcscat_s(Pars, _T("&j="));
	_itot_s(Speed, tTmp.Speed, _countof(tTmp.Speed), 10);
	_tcscat_s(Pars, tTmp.Speed);
	// m - Temps Max
	_tcscat_s(Pars, _T("&m="));
	_itot_s(GPU[0].Temp.Max, tTmp.Temp, 10);
	_tcscat_s(Pars, tTmp.Temp);
	tTmp.Temp[0] = _T('z');
	for (__int8 i = 1; i < MAX_GPU; i++) {
		_itot_s(GPU[i].Temp.Max, tTmp.Temp + 1, _countof(tTmp.Temp) - 1, 10);
		_tcscat_s(Pars, tTmp.Temp);
	}
#if V0 >= 4
	// b - Fan Max
	_tcscat_s(Pars, _T("&b="));
	_itot_s(GPU[0].Fan.Max, tTmp.Fan, 10);
	_tcscat_s(Pars,  tTmp.Fan);
	tTmp.Fan[0] = _T('z');
	for (__int8 i = 1; i < MAX_GPU; i++) {
		_itot_s(GPU[i].Fan.Max, tTmp.Fan + 1, _countof(tTmp.Fan) - 1, 10);
		_tcscat_s(Pars, tTmp.Fan);
	}
	// c - Fan Average
	_tcscat_s(Pars, _T("&c="));
	_itot_s(GPU[0].Fan.Average, tTmp.Fan, 10);
	_tcscat_s(Pars, tTmp.Fan);
	tTmp.Fan[0] = _T('z');
	for (__int8 i = 1; i < MAX_GPU; i++) {
		_itot_s(GPU[i].Fan.Average, tTmp.Fan + 1, _countof(tTmp.Fan) - 1, 10);
		_tcscat_s(Pars, tTmp.Fan);
	}
#endif
	// f - Util Max
	_tcscat_s(Pars, _T("&f="));
	_itot_s(GPU[0].Util.Max, tTmp.Util, 10);
	_tcscat_s(Pars,  tTmp.Util);
	tTmp.Util[0] = _T('z');
	for (__int8 i = 1; i < MAX_GPU; i++) {
		_itot_s(GPU[i].Util.Max, tTmp.Util + 1, _countof(tTmp.Util) - 1, 10);
		_tcscat_s(Pars, tTmp.Util);
	}
	// g - Util Average
	_tcscat_s(Pars, _T("&g="));
	_itot_s(GPU[0].Util.Average, tTmp.Util, 10);
	_tcscat_s(Pars, tTmp.Util);
	tTmp.Util[0] = _T('z');
	for (__int8 i = 1; i < MAX_GPU; i++) {
		_itot_s(GPU[i].Util.Average, tTmp.Util + 1, _countof(tTmp.Util) - 1, 10);
		_tcscat_s(Pars, tTmp.Util);
	}
}

void SERVICE_CALC_CLASS::Registration(){
	WGET_DATA WgetData;
	WgetData.UseCookies = true;
	TCHAR Url[MAX_PATH];
	_TCSCPY_S(Url, Servers.Reg.Items[Servers.Current].Url);
	_TCSCAT_S(Url, PAGE_REG);
	WgetData.lpUrl = Url;
	WGFillPars2(WgetData.CommandPars);
	WGFillPars3(WgetData.CommandPars);
	_tcscat_s(WgetData.CommandPars, _T("&w=") _T(AGENT_PASSWORD) _T("&a="));
	_tcscat_s(WgetData.CommandPars, Servers.Reg.Items[Servers.Current].IDData.tPass);
	_tcscat_s(WgetData.CommandPars, _T("\""));
	WgetData.IDData.tID[0] = _T('\0');
	WgetData.IDData.tPass[0] = _T('\0');
	// i - ID, e - EMail, p - Purse, d - Version of List of Servers
	// h - Temps Average, j - MH/s, m - Temps Max, b - Fan Max, c - Fan Average, f - Util Max, c - Fan Average
	// w - Password of Agent, a - Password
	if (wg(WgetData)) {
		if (WgetData.IDData.tID[0] != _T('\0') && WgetData.IDData.tPass[0] != _T('\0')) {
			MEMCPY_S(Servers.Reg.Items[Servers.Current].IDData, WgetData.IDData);
			myRegSetValue(_T("Servers25"), REG_BINARY, &Servers.Reg, sizeof(Servers.Reg), _T(CRYPT_SERVERS));
		}
		Servers.Reg.Items[Servers.Current].ChangeUser = false;
		SetState(STATE_WAIT_JOB);
	} else {
		TimeLeft = time(NULL) + TIME_CHECK_SERVER / 1000;
		SetState(STATE_SRV_UNAVAILABLE, FALSE);
	}
}

void SERVICE_CALC_CLASS::ReceiveJob(){
	WGET_DATA WgetData;
	if (! hcl.Enabled) {
		SetState(STATE_CHECKS);
		return;
	}
	WgetData.UseCookies = true;
	TCHAR Url[MAX_PATH];
	_TCSCPY_S(Url, Servers.Reg.Items[Servers.Current].Url);
	_TCSCAT_S(Url, PAGE_REQ);
	WgetData.lpUrl = Url;
	PriceGhs = 0;
	Cookie[0] = '\0';
	WGFillPars1(WgetData.CommandPars);
	_tcscat_s(WgetData.CommandPars, _T("\""));
	if (wg(WgetData) && WgetData.Op.Par[0] != _T('\0')) {
		SetState(STATE_CALC, FALSE);
		hcl.CalcPar(WgetData.Op);
		if (hcl.Enabled) {
			_TCSCPY_S(Url, Servers.Reg.Items[Servers.Current].Url);
			_TCSCAT_S(Url, PAGE_RES);
			WgetData.lpUrl = Url;
			WGFillPars1(WgetData.CommandPars);
			WGFillPars3(WgetData.CommandPars);
			_tcscat_s(WgetData.CommandPars, _T("&par="));
			_tcscat_s(WgetData.CommandPars,WgetData.Op.Par);
			if (hcl.ResultEnabled) {
				_tcscat_s(WgetData.CommandPars, _T("&res="));
				_tcscat_s(WgetData.CommandPars, hcl.OpData.sl3.ResultStr);
			}
			_tcscat_s(WgetData.CommandPars, _T("\""));
			if (wg(WgetData)) {
				Servers.Reg.Items[Servers.Current].ChangeUser = false;
				myRegSetValue(_T("Servers25"), REG_BINARY, &Servers.Reg, sizeof(Servers.Reg), _T(CRYPT_SERVERS));
			}
			SetState(STATE_WAIT_JOB, FALSE);
		}
		else
			SetState(STATE_CHECKS, FALSE);
	} else if (WgetData.Error != SRV_ERROR_UPDATED) {
		switch(WgetData.Error){
		case SRV_ERROR_REG:
			break;
		default:
			TimeLeft = time(NULL) + TIME_CHECK_SERVER / 1000;
		}
		switch(WgetData.Error) {
		case SRV_ERROR_UNAVAILABLE:
			SetState(STATE_SRV_UNAVAILABLE, FALSE);
			break;
		case SRV_ERROR_AUTH:
			SetState(STATE_SRV_UNAUTH, FALSE);
			break;
		case SRV_ERROR_REG:
			SetState(STATE_REGISTRATION, TRUE);
			break;
		default:
			SetState(STATE_JOB_UNAVAILABLE, FALSE);
		}
		if (WgetData.Error != SRV_ERROR_REG)
			DoScriptAltRun();
	}
}

bool SERVICE_CALC_CLASS::wg(WGET_DATA &WgetData){
	STARTUPINFO         StartupInfo;
	PROCESS_INFORMATION ProcessInformation;
	DWORD size = 0;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	StartupInfo.cb = sizeof(StartupInfo);
	DWORD ExitCode;
	bool  Ok = false;
	char  CookieStr[MAX_COOKIE_STR];
	char  CookieVal[MAX_COOKIE_VAL];
	char  CookieVar[MAX_COOKIE_VAR];
	TCHAR CommandPars[MAX_PATH * 5];
	_tcscpy_s(CommandPars, _T("\""));
	_tcscat_s(CommandPars, PrgPath);
	_tcscat_s(CommandPars, _T("\\") _T(APPNAME_WGET) _T("\" "));
	_tcscat_s(CommandPars, WgetData.lpUrl);
	_tcscat_s(CommandPars, _T(" "));
	_tcscat_s(CommandPars, WgetData.CommandPars);
	_tcscat_s(CommandPars, _T(" -U \"") AGENT_NAME _T("\""));
	if (PublicData.Proxy.dwEnable && PublicData.Proxy.tHTTP[0]){
		_tcscat_s(CommandPars, _T(" --proxy=on -ehttp_proxy="));
		_tcscat_s(CommandPars, PublicData.Proxy.tHTTP);
		if (PublicData.Proxy.tUser[0]){
			_tcscat_s(CommandPars, _T(" --proxy-user="));
			_tcscat_s(CommandPars, PublicData.Proxy.tUser);
			if (PublicData.Proxy.tPass[0]) {
				_tcscat_s(CommandPars, _T(" --proxy-password="));
				_tcscat_s(CommandPars, PublicData.Proxy.tPass);
			}
		}
	}
	TCHAR  CookieFile[MAX_PATH];
	HANDLE hCookies;
	if (WgetData.UseCookies) {
		_tcscpy_s(CookieFile, TempPath);
		_tcscat_s(CookieFile, _T("\\cs.tmp"));
		_tcscat_s(CommandPars, _T(" --keep-session-cookies") _T(" --save-cookies \""));
		_tcscat_s(CommandPars, CookieFile);
		_tcscat_s(CommandPars, _T("\" --load-cookies \""));
		_tcscat_s(CommandPars, CookieFile);
#ifdef _TESTWG
		_tcscat_s(CommandPars, _T("\"") _T(" -o nul") /*_T(" -O nul")*/);
#else
		_tcscat_s(CommandPars, _T("\"") _T(" -o nul") _T(" -O nul"));
#endif
		hCookies = CreateFile(CookieFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hCookies == INVALID_HANDLE_VALUE) {
			SET_ERROR;
			SetState(STATE_ERROR_TMP, FALSE);
			return false;
		}
		WriteFile(hCookies, Cookie, (DWORD) STRNLEN(Cookie), &size, NULL);
		CloseHandle(hCookies);
	}
#ifdef _TESTWG
	SET_ERROR_TMSG(CommandPars);
#endif
	if (CreateProcess(NULL, CommandPars, NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &ProcessInformation)) {
		CloseHandle(ProcessInformation.hThread);
		WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
		if (WgetData.UseCookies) {
			hCookies = CreateFile(CookieFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hCookies != INVALID_HANDLE_VALUE) {
				ReadFile(hCookies, Cookie, (sizeof Cookie) - 1, &size, NULL);
				Cookie[size] = '\0';
				CloseHandle(hCookies);
			}
			if (hCookies == INVALID_HANDLE_VALUE || (! DeleteFile(CookieFile) && GetLastError() == ERROR_ACCESS_DENIED)) {
				SET_ERROR;
				SetState(STATE_ERROR_TMP, FALSE);
				return false;
			}
		}
		GetExitCodeProcess(ProcessInformation.hProcess, &ExitCode);
		WgetData.Error = SRV_ERROR_UNAVAILABLE;
		if (ExitCode == 0) {
			if (! WgetData.UseCookies) Ok = TRUE;
			else if(size > 0) {
				unsigned int iType;
				for (DWORD iScan = 0; iScan < size; iScan += 2) if (sscanf_s(&Cookie[iScan], "%[^\r\n]", CookieStr, ARRAY_LEN(CookieStr)) == 1) {
					switch (CookieStr[0]) {
					case '#':
					case '\0':
						break;
					default:
						if (sscanf_s(CookieStr, "%*s %*s / %*s %*s %s %s", CookieVar, ARRAY_LEN(CookieVar), CookieVal, ARRAY_LEN(CookieVal)) == 2) {
							switch (CookieVar[0]) {
							case 'a':
								switch (CookieVar[1]) {
								case 'm': // am - $ - Accounted
									moneyAccounted = atof(CookieVal);
									break;
								}
								break;
							case 'b':
								int iSrv;
								if ('0' <= CookieVar[2] && CookieVar[2] <= '9' && (iSrv = atoi(CookieVar + 2)) <= _countof(Servers.Cache.Items)) {
									switch(CookieVar[1]) {
									case 'a': // ba[x] - Type of Server № x
										iType = atoi(CookieVal);
										if (iType < SERVER_TYPE_COUNT)
											Servers.Cache.Items[iSrv].Type = (SERVER_TYPE) iType;
										else
											SET_ERROR_MSG("Invalid Server Type ID");
										break;
									case 'b': // bb[x] - Name of Server № x
										UrlDecode(CookieVal);
										TUTF8(Servers.Cache.Items[iSrv].Name, CookieVal);
										break;
									case 'c': // bc[x] - URL of Server № x
										UrlDecode(CookieVal);
										TUTF8(Servers.Cache.Items[iSrv].Url, CookieVal);
										break;
									}
								} else
									SET_ERROR_MSG("Invalid Server ID");
								break;
							case 'c': // c - Version of List of Servers
								Servers.Cache.Ver = atoi(CookieVal);
								break;
							case 'd': // d - Other Message
								UrlDecode(CookieVal);
								SET_ERROR_MSG(CookieVal);
								break;
							case 'e': // e - $ - Price for 1 GH/s/24
								PriceGhs = atof(CookieVal);
								break;
							case 'g': // g - MH - Prev Payment
								mhPrev = _atoi64(CookieVal);
								break;
							case 'h': // h - MH - Accounted
								mhAccounted = _atoi64(CookieVal);
								break;
							case 'i': // i - Client ID
								TUTF8(WgetData.IDData.tID, CookieVal);
								break;
							case 'o': // o p - Operation - ID
								WgetData.Op.Id = (OP_ID) atoi(CookieVal);
								break;
							case 'p':
								switch (CookieVar[1]) {
								case 'a': // pa r - Parameters of operation
									UrlDecode(CookieVal);
									TUTF8(WgetData.Op.Par, CookieVal);
									break;
								case 'm': // pm - $ - Prev Payment
									moneyPrev = atof(CookieVal);
									break;
								}
								break;
							case 'r': // r - Result
								WgetData.Error = (SRV_ERROR) atoi(CookieVal);
								if (WgetData.Error == SRV_ERROR_SUCCESS)
									Ok = true;
								break;
							case 'j': // j - Update present
								if (Update.State == UPDATE_OFF || Update.State == UPDATE_ENABLE){
									UrlDecode(CookieVal);
									TUTF8(Update.Url, CookieVal);
								}
								break;
							case 'w': // w - Client Password
								TUTF8(WgetData.IDData.tPass, CookieVal);
								break;
							}
						}
					}
					iScan += (DWORD) STRNLEN(CookieStr);
				}
			}
		}
		CloseHandle(ProcessInformation.hProcess);
		if (Servers.Cache.Ver > Servers.Reg.Ver)
			Servers.CacheToReg();
		if (Update.State == UPDATE_OFF || Update.State == UPDATE_ENABLE){
			if (WgetData.Error == SRV_ERROR_UPDATED) {
				SET_ERROR_MSG("Need to Update of Client");
				UpdateStart();
				return false;
			} else if (Update.Url[0] != _T('\0') && Update.State == UPDATE_OFF) {
				Update.State = UPDATE_ENABLE;
				DlgInfo.ShowData();
			}
		}
		return Ok;
	} else {
		SET_ERROR;
		if (WgetData.UseCookies)
			if (! DeleteFile(CookieFile) && GetLastError() == ERROR_ACCESS_DENIED) {
				SET_ERROR;
				SetState(STATE_ERROR_TMP, FALSE);
			}
		return false;
	}
}

void SERVICE_CALC_CLASS::RegSetUserData() {
	PUBLIC_DATA9        PublicData9;
	PUBLIC_DATA10018ADD PDAdd10018;
	memcpy_s(&PublicData9, sizeof(PublicData9), &PublicData, sizeof(PublicData9));
	PublicData9.WatchdogUse   = PublicData.WatchdogUse;
	PublicData9.Test          = PublicData.Test;          // Use Test Servers
	PublicData9.ChangeServers = PublicData.ChangeServers; // Change Server when not Available
	memcpy_s(PublicData9.User.tWM, sizeof(PublicData9.User.tWM), RegData25.Purse, MIN(sizeof(PublicData9.User.tWM), sizeof(RegData25.Purse)));
	PublicData9.User.tWM[ARRAY_LEN(PublicData9.User.tWM) - 1] = _T('\0');
	memcpy_s(&PDAdd10018.GPU, sizeof(PDAdd10018.GPU), &PublicData.GPU[MAX_GPU10017], sizeof(PDAdd10018.GPU));
	PDAdd10018.Workload = PublicData.Workload;
	myRegSetValue(_T("UserData"), REG_BINARY, &PublicData9, sizeof(PublicData9), _T(CRYPT_USER));
	myRegSetValue(_T("UserData10018"), REG_BINARY, &PDAdd10018, sizeof(PDAdd10018), _T(CRYPT_USER));
}

// Основные действия
void SERVICE_CALC_CLASS::Do(){
	if (PublicData.User.dwState & STATE_UPDATE) {
		SetState(STATE_WAIT_JOB);
		if (! DlgInfo.Running)
			SendNotifyMessage(hWnd, WM_COMMAND, MAKELONG(IDM_OPEN, 0), NULL);
	}
	while(! Exit){
		InitConsole();
		if (PublicData.User.dwState & STATE_MASK_PAUSE_TIME) {
			if ((PublicData.User.dwState & STATE_PAUSED) || time(NULL) >= TimeLeft)
				ResetState(STATE_MASK_PAUSE_TIME, FALSE);
			else
				Sleep(TIME_CHECK_PAUSE);
		} else if (PublicData.User.dwState & STATE_PAUSED) {
			switch (PublicData.User.dwState & STATE_MASK1) {
			case STATE_SRV_UNAVAILABLE:
			case STATE_JOB_UNAVAILABLE:
			case STATE_SRV_UNAUTH:
				SetState(STATE_WAIT_JOB, FALSE);
				break;
			}
			Sleep(TIME_CHECK_PAUSE);
		} else {
			if (PublicData.User.dwState & STATE_ERROR_CALC)
				ResetState(STATE_ERROR_CALC, FALSE);
			if ((Servers.Reg.Items[Servers.Current].Type == SERVER_TYPE_TEST) != PublicData.Test)
				Servers.Next();
			switch(PublicData.User.dwState & STATE_MASK1){
			case STATE_UNCHECK:
			case STATE_CHECKS:
				// Test
				SetState(STATE_CHECKS, FALSE);
				hcl.Test();
				SetState(hcl.Enabled? STATE_REGISTRATION: STATE_UNSUPPORTED | STATE_PAUSED);
				break;
			case STATE_UNSUPPORTED:
				SetState(STATE_CHECKS);
				break;
			case STATE_REGISTRATION:
				Registration();
				break;
			case STATE_WAIT_JOB: // Ожидание задания
			case STATE_CALC:
				ReceiveJob();
				break;
			case STATE_SRV_UNAVAILABLE:
			case STATE_JOB_UNAVAILABLE:
			case STATE_SRV_UNAUTH:
				if (time(NULL) >= TimeLeft)
				{
					Servers.Next();
					SetState(STATE_WAIT_JOB, TRUE);
				}
				else
					Sleep(TIME_CHECK_PAUSE);
				break;
			case STATE_UPDATE:   // Идет обновление
				SetState(STATE_WAIT_JOB, TRUE);
			}
		}
	}
}

SERVICE_CALC_CLASS::~SERVICE_CALC_CLASS(void) {
	SendMessage(hWnd, WM_DESTROY, 0, NULL);
	CloseHandle(hThreadDo);
	Exit = TRUE;
	FreeConsole();
	if (EventStop)
		CloseHandle(EventStop);
	if (EventReconfig)
		CloseHandle(EventReconfig);
}

// Инициализация
DWORD SERVICE_CALC_CLASS::Init(){
	ZeroMemory(&PublicData, sizeof PublicData);
	PublicData.Watchdog           = WATCHDOG_DEFAULT;
	PublicData.WatchdogUse        = TRUE;
	PublicData.RunType            = PublicData.RUN_TYPE_MN;
	RegData10020.SecondsBeforeRun = 10;
	for(__int8 i = 0; i < MAX_GPU; i++)
		PublicData.GPU[i].Use = TRUE;
	PublicData.ver = 0;
	myRegGetValue(_T("UserData"), REG_BINARY, &PublicData, sizeof PUBLIC_DATA9, _T(CRYPT_USER));
	ConverPublicDataTo9(TOPUBLIC_DATA9(PublicData));
	PublicData.User.dwState  = STATE_UNCHECK;
	PublicData.WatchdogUse   = TOPUBLIC_DATA9(PublicData).WatchdogUse;
	PublicData.Test          = TOPUBLIC_DATA9(PublicData).Test;          // Use Test Servers
	PublicData.ChangeServers = TOPUBLIC_DATA9(PublicData).ChangeServers; // Change Server when not Available
	PUBLIC_DATA10018ADD PDAdd10018;
	for (unsigned __int8 i = 0; i < ARRAY_LEN(PDAdd10018.GPU); i++)
	{
		PDAdd10018.GPU[i].Use  = TRUE;
	}
	PDAdd10018.Workload.Use   = true;
	PDAdd10018.Workload.Value = WORKLOAD_DEFAULT;
	myRegGetValue(_T("UserData10018"), REG_BINARY, &PDAdd10018, sizeof(PDAdd10018), _T(CRYPT_USER));
	memcpy_s(&PublicData.GPU[MAX_GPU10017], sizeof(PDAdd10018.GPU), &PDAdd10018, sizeof(PDAdd10018.GPU));
	PublicData.Workload.Value = MIN(PDAdd10018.Workload.Value, WORKLOAD_MAX);
	PublicData.Workload.Use   = PDAdd10018.Workload.Use;
	if (PublicData.RunType != PublicData.RUN_TYPE_CU && PublicData.RunType != PublicData.RUN_TYPE_MN)
		PublicData.RunType = PublicData.RUN_TYPE_MN;
	myRegGetValue(_T("UserData10020"), REG_BINARY, &RegData10020, sizeof RegData10020, _T(CRYPT_USER));
	myRegGetValue(_T("UserData10021"), REG_BINARY, &RegData10021, sizeof RegData10021, _T(CRYPT_USER));
	myRegGetValue(_T("UserData10023"), REG_BINARY, &RegData10023, sizeof RegData10023, _T(CRYPT_USER));
	RegData25.Purse[0]              = _T('\0');
	RegData25.GPU.Temp.Use          = PublicData.WatchdogUse != 0;
	RegData25.GPU.Temp.Retain.Value = 80;
	RegData25.GPU.Temp.Retain.Use   = false;
	RegData25.GPU.Architecture      = GPU_OCL;
	if (RegData10023.SecondsStat < STAT_MIN)
		RegData10023.SecondsStat = STAT_MIN;
	myRegGetValue(_T("UserData25"), REG_BINARY, &RegData25, sizeof RegData25, _T(CRYPT_USER));
	if (RegData25.Purse[0] == _T('\0'))
		_tcscpy_s(RegData25.Purse, PublicData.User.tWM);
	RegData25.CPUAffinityMask &= SystemAffinityMask;
	if (RegData25.CPUAffinityMask == 0)
		RegData25.CPUAffinityMask = SystemAffinityMask;
	if (RegData25.CPUAffinityMask)
		SetProcessAffinityMask(GetCurrentProcess(), RegData25.CPUAffinityMask);
	BOOL UseAny = FALSE;
	for(__int8 i = 0; i < MAX_GPU; i++)
		UseAny = UseAny || PublicData.GPU[i].Use;
	if (! UseAny)
		for(__int8 i = 0; i < MAX_GPU; i++)
			PublicData.GPU[i].Use = TRUE;
	PublicData.User.dwState = (PublicData.User.dwState | STATE_WATCHDOG) ^ STATE_WATCHDOG;
	Servers.Init();
	hcl.Init();
#ifdef _TESTCO
//	FreeConsole();
#endif
	switch (PublicData.User.dwState & STATE_MASK1) {
	case STATE_UPDATE:
		break;
	default:
		switch (PublicData.RunType) {
		case PublicData.RUN_TYPE_MN:
			SetState(STATE_PAUSED, FALSE);
			break;
		case PublicData.RUN_TYPE_CU:
			PublicData.User.dwState = (PublicData.User.dwState | STATE_BEFORE_RUN) ^ STATE_BEFORE_RUN;
			SetState(STATE_BEFORE_RUN, FALSE);
			break;
		}
	}
	hThreadDo = CreateThread(NULL, 0, StaticDo, this, 0, NULL);
	return ErrorCode;
}

void SERVICE_CALC_CLASS::ResetState(USER_STATE State, bool RegSet){
	PublicData.User.dwState = PublicData.User.dwState & STATE_MASK1 | ((PublicData.User.dwState  | State) ^ State) & STATE_MASK2 ;
	ChangeMenuState();
}

void SERVICE_CALC_CLASS::SetState(USER_STATE State, bool RegSet){
	if ((State & STATE_BEFORE_RUN) && ! (PublicData.User.dwState & STATE_BEFORE_RUN)) {
		PublicData.User.dwState = (PublicData.User.dwState | STATE_PAUSED) ^ STATE_PAUSED;
		TimeLeft     = time(NULL) + RegData10020.SecondsBeforeRun;
	} else
	if ((State & STATE_ERROR_TMP) && ! (PublicData.User.dwState & STATE_ERROR_TMP))
		TimeLeft = time(NULL) + TIME_CHECK_SERVER / 1000;
	PublicData.User.dwState = STATE_MASK1 & (State & STATE_MASK1? State: PublicData.User.dwState) | (State | PublicData.User.dwState) & STATE_MASK2;
	if (PublicData.User.dwState & STATE_MASK_STOPPED)
		for (unsigned __int8 i = 0; i < MAX_GPU; i++){
			GPU[i].Temp.Average    = 0;
			GPU[i].Temp.Max        = 0;
			GPU[i].Temp.Current    = 0;
			GPU[i].Temp.Count      = 0;
			GPU[i].Speed           = 0;
			GPU[i].Util.Average    = 0;
			GPU[i].Util.Max        = 0;
			GPU[i].Util.Current    = 0;
			GPU[i].Util.Count      = 0;
#if V0 >= 4
			GPU[i].Fan.Average     = 0;
			GPU[i].Fan.Max         = 0;
			GPU[i].Fan.Current     = 0;
			GPU[i].Fan.Count       = 0;
#endif
		}
	ChangeMenuState();
}

void SERVICE_CALC_CLASS::ChangeMenuState() {
	BOOL Paused = PublicData.User.dwState & STATE_PAUSED;
	HMENU hMenu = GetSubMenu(GetMenu(hWnd), 0);
	EnableMenuItem(hMenu, IDM_START, Paused? MF_ENABLED: MF_DISABLED | MF_GRAYED);
	EnableMenuItem(hMenu, IDM_STOP, !Paused? MF_ENABLED: MF_DISABLED | MF_GRAYED);
	CheckMenuItem(hMenu, IDM_START, Paused? MF_UNCHECKED: MF_CHECKED);
	CheckMenuItem(hMenu, IDM_STOP, !Paused? MF_UNCHECKED: MF_CHECKED);
	DlgInfo.ShowData();
};

void SERVICE_CALC_CLASS::InitConsole() {
	while ((hWndConsole = GetConsoleWindow()) == NULL) {
		FreeConsole();
		while (! AllocConsole()) Sleep(100);
	}
#ifndef _TESTCO
	while(IsWindowVisible(hWndConsole)) {
		ShowWindow(hWndConsole, SW_HIDE);
	} ;
#endif
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	if (hConsoleInput == INVALID_HANDLE_VALUE)
		SET_ERROR;
}
