#pragma once
#pragma comment(lib, "Psapi.lib")

#define PSAPI_VERSION 1

#include <windows.h>
#include <Shlobj.h>
#include <psapi.h>
#include <TCHAR.h>
#include <Strsafe.h>
#include <TlHelp32.h>
#include <Shlwapi.h>
#include "mystd.h"
#include "..\CalcService\SERVICE_CALC_CLASS.h"


BOOL CreateProcess2(int Path, LPTSTR Cmd, LPTSTR Pars, PROCESS_INFORMATION &PI, LPTSTR lpCurrentDirectory, DWORD dwCreationFlags) {
	STARTUPINFO SI;
	ZeroMemory(&SI, sizeof(SI));
	SI.cb = sizeof SI;
	TCHAR Name[MAX_PATH] = _T("");
	TCHAR CmdLine[MAX_PATH *2] = _T("\"");
	if (Path != -1 && GetSysPath(Path, Name) == S_OK){
		_tcscat_s(Name, _T("\\"));
		_tcscat_s(CmdLine, Name);
	}
	_tcscat_s(Name, Cmd);
	_tcscat_s(CmdLine, Cmd);
	_tcscat_s(CmdLine, _T("\""));
	if (Pars) {
		_tcscat_s(CmdLine, _T(" "));
		_tcscat_s(CmdLine, Pars);
	}
	BOOL Result = CreateProcess(Name, CmdLine, NULL, NULL, TRUE, dwCreationFlags, NULL, lpCurrentDirectory, &SI, &PI);
	if(Result)
		CloseHandle(PI.hThread);
	else
		SET_ERROR;
	return Result;
}

// Возвращает указатель на последнее вхождение строки Req в строку Str или NULL
char * StrStrR(char * Str, char * Reg){
	char * Res = NULL;
	for (char * Str2 = strstr(Str, Reg); Str2; Str2 = strstr(Str2 + 1, Reg)) Res = Str2;
	return Res;
}

__int32 GetOSVersion(){
	__int32 Result;
	OSVERSIONINFOEX VersionInfo;
	VersionInfo.dwOSVersionInfoSize = sizeof VersionInfo;
	if (GetVersionEx((LPOSVERSIONINFO) &VersionInfo))
		Result = (VersionInfo.dwMajorVersion << 24) | (VersionInfo.dwMinorVersion << 16) | (VersionInfo.wServicePackMajor << 8) | VersionInfo.wServicePackMinor;
	else
		Result = 0;
	return Result;
}

HRESULT GetSysPath(int nPath, LPTSTR tPath){
	HRESULT Result;
#ifdef _M_X64
	if (OS_FROM_VISTA()) {
		KNOWNFOLDERID gPath;
		switch(nPath){
		case CSIDL_SYSTEM:
			gPath = FOLDERID_System;
			break;
		case CSIDL_APPDATA:
			gPath = FOLDERID_LocalAppData;
			break;
		case CSIDL_COMMON_APPDATA:
			gPath = FOLDERID_ProgramData;
			break;
		case CSIDL_WINDOWS:
			gPath = FOLDERID_Windows;
			break;
		}
		LPTSTR KnownPath;
		if (FAILED(Result = SHGetKnownFolderPath(gPath, 0, NULL, &KnownPath)))
			SET_ERROR_MSG("SHGetKnownFolderPath Error");
		else {
			wcscpy_s(tPath, MAX_PATH, KnownPath);
			CoTaskMemFree(KnownPath);
		}
	}
	else
#endif
		if (FAILED(Result = SHGetFolderPath(NULL, nPath | CSIDL_FLAG_CREATE, NULL, 0, tPath)))
			SET_ERROR_MSG("SHGetFolderPath Error");
	return Result;
}

inline HRESULT GetSystem32Path(LPTSTR System32Path) {
	return GetSysPath(CSIDL_SYSTEM, System32Path);
}

void KillProcess(LPTSTR Name, bool Tree) {
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	TCHAR szProcessName[MAX_PATH + 1];
	HMODULE hMod;
 	if (EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		// Calculate how many process identifiers were returned.
		cProcesses = cbNeeded / sizeof(DWORD);
  		for ( i = 0; i < cProcesses; i++ ) {
			// Get a handle to the process.
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, aProcesses[i]);
			// Get the process name.
			if (NULL != hProcess && EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded) && GetModuleBaseName(hProcess, hMod, szProcessName, ARRAY_LEN(szProcessName)) != 0 && _tcsicmp(Name, szProcessName) == 0)
			{
				if (Tree)
					KillProcessChildTree(aProcesses[i]);
				TerminateProcess(hProcess, ERROR_SUCCESS);
			}
			CloseHandle(hProcess);
		}
	}
}

// Функция преобразует строку данных st в нормальное представление.
// Результат помещается в ту же строку, что была передана в параметрах.
// Возвращает количество символов в результатате без лидирующего нуля.
DWORD UrlDecode(char * st)
{
	BYTE * p = (BYTE *) st;
	BYTE * Start = (BYTE *) st;
	BYTE code;
	do {
		switch (* st) {
		case '%':
			st++;
			code = (* st - (* st >= '0' && * st <='9'? '0': (* st >= 'a' && * st <='f'? 'a' - 10: 'A' - 10))) << 4;
			st++;
			* p++ = code + * st - (* st >= '0' && * st <='9'? '0': (* st >= 'a' && * st <='f'? 'a' - 10: 'A' - 10));
			break;
		case '+':
			* p++ = ' ';
			break;
		default:
			* p++ = * st;
		}
	}
	while(* st++ != '\0');
	return (DWORD) (p - Start - 1);
}

void KillProcessChildTree(DWORD ProcessId)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		SET_ERROR;
	else {
		DWORD err;
		PROCESSENTRY32 pe = { 0 };
		pe.dwSize = sizeof(PROCESSENTRY32);
		if(Process32First(hSnapshot, &pe))
		{
			do {
				if (pe.th32ParentProcessID == ProcessId)
				{
					KillProcessChildTree(pe.th32ProcessID);
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
					if (hProcess == NULL)
						SET_ERROR;
					else {
						if (! TerminateProcess(hProcess, ERROR_SUCCESS))
							SET_ERROR;
						if (! CloseHandle(hProcess))
							SET_ERROR;
					}
				}
			} while(Process32Next(hSnapshot, &pe));
			if ((err = GetLastError()) != ERROR_NO_MORE_FILES)
				SET_ERROR_CODE(err);
		} else if ((err = GetLastError()) != ERROR_NO_MORE_FILES)
			SET_ERROR_CODE(err);
		if (! CloseHandle(hSnapshot))
			SET_ERROR;
	}
}

DWORD CreateProcess10024(int Path, LPTSTR Cmd, LPTSTR Pars, LPTSTR lpCurrentDirectory, DWORD dwCreationFlags, bool Wait, bool SystemAffinity){
	PROCESS_INFORMATION PI;
	DWORD ExitCode = -1;
	DWORD_PTR CPUAffinityMask;
	DWORD_PTR SystemAffinityMask;
	if (CreateProcess2(Path, Cmd, Pars, PI, lpCurrentDirectory, dwCreationFlags)){
		if (SystemAffinity && GetProcessAffinityMask(GetCurrentProcess(), &CPUAffinityMask, &SystemAffinityMask) != 0 && CPUAffinityMask != SystemAffinityMask)
			SetProcessAffinityMask(PI.hProcess, SystemAffinityMask);
		if (Wait) {
			WaitForSingleObject(PI.hProcess, INFINITE);
			GetExitCodeProcess(PI.hProcess, &ExitCode);
		} else
			ExitCode = 0;
		CloseHandle(PI.hProcess);
	}
	return ExitCode;
}

HRESULT GetCommonAppPath(LPTSTR Path) {
	return GetSysPath(CSIDL_COMMON_APPDATA, Path);
}

void NotepadEdit(FILE_NAME FileName)
{
	FILE_NAME par, dir;
	_tcscpy_s(par, FileName);
	_tcscpy_s(dir, FileName);
	PathQuoteSpaces(par);
	PathRemoveFileSpec(dir);
	CreateProcessNoWait(CSIDL_WINDOWS, _T("notepad.exe"), par, dir, 0);
}

bool ScriptRun(FILE_NAME FileName, bool Wait, bool SystemAffinity) {
	FILE_NAME par = _T("/C ");
	FILE_NAME dir;
	bool Ok = false;
	if (FAILED(StringCchCopy(dir, _countof(dir), FileName)))
		SET_ERROR_MSG("StrCchCopy Error");
	else {
		PathQuoteSpaces(dir);
		if (FAILED(StringCchCat(par, _countof(par), dir)))
			SET_ERROR_MSG("StringCchCat Error");
		else {
			if (FAILED(StringCchCopy(dir, _countof(dir), FileName)))
				SET_ERROR_MSG("StrCchCopy Error");
			else {
				PathRemoveFileSpec(dir);
				Ok = CreateProcess10024(CSIDL_SYSTEM, _T("cmd.exe"), par, dir, CREATE_NEW_CONSOLE, Wait, SystemAffinity) == 0;
			}
		}
	}
	return Ok;
}

bool ExeRun(FILE_NAME FileName, bool Wait, bool SystemAffinity) {
	FILE_NAME dir;
	bool Ok = false;
	if (FAILED(StringCchCopy(dir, _countof(dir), FileName)))
		SET_ERROR_MSG("StrCchCopy Error");
	else {
		PathRemoveFileSpec(dir);
		Ok = CreateProcess10024(-1, FileName, NULL, dir, CREATE_NEW_CONSOLE, Wait, SystemAffinity) == 0;
	}
	return Ok;
}



// Получить значение ключа реестра
bool myRegGetValue(LPCTSTR lpValue, DWORD dwFlags, PVOID pvData, DWORD cbData, LPCTSTR lpCryptEntropy, LPDWORD lpcbNeed) {
	HKEY hKey;
	DWORD Err;
	Err = ::RegCreateKeyEx(DC_HKEY, REG_PATH, NULL, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (FAILED(Err))
		SET_ERROR_CODE(Err);
	else {
		if (lpCryptEntropy) {
			DATA_BLOB DataIn;
			DataIn.cbData = 0;
			Err = ::RegQueryValueEx(hKey, lpValue, NULL, NULL, NULL, &DataIn.cbData);
			switch(Err) {
			case ERROR_SUCCESS:
				DataIn.pbData = (LPBYTE) LocalAlloc(LMEM_FIXED, DataIn.cbData);
				if (! DataIn.pbData)
					Err = SET_ERROR;
				else {
					Err = ::RegQueryValueEx(hKey, lpValue, NULL, &dwFlags, DataIn.pbData, &DataIn.cbData);
					if(FAILED(Err))
						SET_ERROR_CODE(Err);
					else {
						DATA_BLOB Entropy;
						Entropy.cbData = (DWORD) wcslen(lpCryptEntropy) * sizeof TCHAR;
						Entropy.pbData = (LPBYTE) lpCryptEntropy;
						DATA_BLOB DataOut;
						if (! ::CryptUnprotectData(&DataIn, NULL, &Entropy, NULL, NULL, 0, &DataOut)) 
							Err = SET_ERROR;
						else {
							if (memcpy_s(pvData, cbData, DataOut.pbData, MIN(DataOut.cbData, cbData))) {
								SET_ERROR_MSG("ERROR memcpy_s");
								Err = -1;
							} else if(lpcbNeed)
								* lpcbNeed = DataOut.cbData;
							SecureZeroMemory(DataOut.pbData, DataOut.cbData);
							if (LocalFree(DataOut.pbData) != NULL)
								Err = SET_ERROR;
						}
					}
					if (LocalFree(DataIn.pbData) != NULL)
						Err = SET_ERROR;
				}
				break;
			case ERROR_FILE_NOT_FOUND:
				if(lpcbNeed)
					* lpcbNeed = 0;
				Err = ERROR_SUCCESS;
				break;
			default:
				SET_ERROR_CODE(Err);
			}
		} else {
			if (dwFlags == REG_DWORD) cbData = sizeof DWORD;
			Err = ::RegQueryValueEx(hKey, REG_PATH, NULL, NULL, (BYTE *) pvData, &cbData);
			switch(Err) {
			case ERROR_SUCCESS:
				if(lpcbNeed)
					* lpcbNeed = cbData;
				break;
			case ERROR_FILE_NOT_FOUND:
				if(lpcbNeed)
					* lpcbNeed = 0;
				Err = ERROR_SUCCESS;
				break;
			default:
				SET_ERROR_CODE(Err);
			}
		}
		SET_ERROR_CODE(::RegCloseKey(hKey));
	}
	return SUCCEEDED(Err);
}

// Записать значение ключа реестра
bool myRegSetValue(LPCTSTR lpValue, DWORD dwFlags, PVOID pvData, DWORD cbData, LPCTSTR lpCryptEntropy) {
	DWORD Err;
	if (lpCryptEntropy) {
		DATA_BLOB DataIn;
		DataIn.cbData = cbData;
		DataIn.pbData = (BYTE *) pvData;
		DATA_BLOB Entropy;
		Entropy.cbData = (DWORD) wcslen(lpCryptEntropy) * sizeof TCHAR;
		Entropy.pbData = (BYTE *) lpCryptEntropy;
		DATA_BLOB DataOut;
		if(! ::CryptProtectData(&DataIn, _T(""), &Entropy, NULL, NULL, CRYPTPROTECT_LOCAL_MACHINE, &DataOut)) 
			Err = SET_ERROR;
		else {
			Err = myRegSetValue(lpValue, REG_BINARY, DataOut.pbData, DataOut.cbData)? ERROR_SUCCESS: -1;
			if (LocalFree(DataOut.pbData) != NULL)
				Err = SET_ERROR;
		}
	} else{
		HKEY hKey;
		Err = ::RegOpenKeyEx(DC_HKEY, REG_PATH, 0, KEY_ALL_ACCESS, &hKey);
		if (FAILED(Err))
			SET_ERROR_CODE(Err);
		else {
			if (!cbData && dwFlags == REG_DWORD) cbData = sizeof (__int32);
			Err = ::RegSetValueEx(hKey, lpValue, 0, dwFlags, (BYTE *) pvData, cbData);
			if (FAILED(Err))
				SET_ERROR_CODE(Err);
			Err = ::RegCloseKey(hKey);
			if (FAILED(Err))
				SET_ERROR_CODE(Err);
		}
	}
	return SUCCEEDED(Err);
}

#ifdef _UNICODE
inline int TSTR_S(LPWSTR Dest, int _SizeInWords, LPCSTR Source){
	int Count = MultiByteToWideChar(CP_ACP, 0, Source, -1, Dest, _SizeInWords - 1);
	Dest[Count] = _T('\0');
	return Count;
}

inline int ASTR_S(LPSTR Dest, int _SizeInWords, LPCWSTR Source) {
	BOOL udc;
	int Count = WideCharToMultiByte(CP_ACP, 0, Source, -1, Dest, _SizeInWords, NULL, &udc);
	Dest[Count - 1] = '\0';
	return Count;
}

inline int TUTF8_S(LPWSTR Dest, int _SizeInWords, LPCSTR Source){
	int Count = MultiByteToWideChar(CP_UTF8, 0, Source, -1, Dest, _SizeInWords - 1);
	Dest[Count] = _T('\0');
	return Count;
}
#endif

 // Преобразует символ 16-ричной цифры в число
inline unsigned AHEX(const char chr){
	return chr >= '0' && chr <= '9'? chr - '0': (chr >= 'A' && chr <= 'F'? chr - 'A' + 10: chr - 'a' + 10);
}
