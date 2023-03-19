// MyCustomAction.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "..\global\mystd.h"
#include <tchar.h>
#include <Shlwapi.h>

// 09FB6914 8FB2 60B4 6BFE AA294CBEEFFB {4196BF90-2BF8-4B06-B6EF-AA92C4EBFEBF}
// 01234567 8901 2345 6789 012345678901 01234567890123456789012345678901234567
// 0          1            2         3  0         1         2         3
void ToProductCode(LPTSTR ProductCode, LPTSTR ProductCodeGUID) {
	__int8 cnv[] = {
		8, 7, 6, 5, 4, 3, 2, 1,
		13, 12, 11, 10,
		18, 17, 16, 15,
		21, 20, 23, 22,
		26, 25, 28, 27, 30, 29, 32, 31, 34, 33, 36, 35
	};
	for(__int8 i = 0; i < ARRAY_LEN(cnv); i++)
		ProductCode[i] = ProductCodeGUID[cnv[i]];
	ProductCode[ARRAY_LEN(cnv)] = _T('\0');
}

UINT __stdcall SampleFunction ( MSIHANDLE hModule )
{
	TCHAR s1[40];
	DWORD size;

	KillProcess(_T("ohl.exe"));
	KillProcess(_T("chl.exe"));
	KillProcess(_T("ohl64.exe"));
	KillProcess(_T("chl64.exe"));
	KillProcess(_T("CCC.exe"));
	KillProcess(_T("Fuel.Service.exe"));
	KillProcess(_T("DCClient.exe"));
	KillProcess(_T("DCUpdater.exe"));
	size = ARRAY_LEN(s1);
	MsiGetProperty(hModule, _T("REINSTALLMODE"), s1, &size);
#if _DEBUG
	MessageBox(NULL, s1, _T("REINSTALLMODE"), MB_OK);
	TCHAR s2[40];
//	for(__int8 i = 0; s1[i] != _T('\0'); i++)
//		MessageBox(NULL, _itow(s1[i], s2, 10), _T("REINSTALLMODE"), MB_OK);
	MessageBox(NULL, _itow(_tcsicmp(s1, _T("vamus")), s2, 10), _T("Compare REINSTALLMODE"), MB_OK);
#endif
	if(! _tcsicmp(s1, _T("vamus"))) {

		TCHAR SubKey[256] = _T("SOFTWARE\\Classes\\Installer\\Products\\");
		size = ARRAY_LEN(s1);
		MsiGetProperty(hModule, _T("ProductCode"), s1, &size);
		ToProductCode(&SubKey[wcslen(SubKey)], s1);
		_tcscat_s(SubKey, _T("\\SourceList"));
#if _DEBUG
		MessageBox(NULL, SubKey, _T("SubKey"), MB_OK);
#endif

		TCHAR FullMsiRun[MAX_PATH];
		size = ARRAY_LEN(FullMsiRun);
		MsiGetProperty(hModule, _T("OriginalDatabase"), FullMsiRun, &size);
		LPTSTR MsiRun   = PathFindFileName(FullMsiRun);
#if _DEBUG
		MessageBox(NULL, FullMsiRun, _T("OriginalDatabase"), MB_OK);
#endif

		TCHAR FullMsiInReg[MAX_PATH];
		_tcscpy_s(FullMsiInReg, FullMsiRun);
		LPTSTR MsiInReg = PathFindFileName(FullMsiInReg);

		HKEY hKey;
		if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, SubKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS){
			size = sizeof(FullMsiInReg) - (DWORD) (((BYTE *) MsiInReg) - ((BYTE *) FullMsiInReg));
#if _DEBUG
			MessageBox(NULL, _itow(size, s1, 10), _T("SizeMsiInReg"), MB_OK);
#endif
			if (::RegQueryValueEx(hKey, _T("PackageName"), NULL, NULL, (BYTE *) MsiInReg, &size) == ERROR_SUCCESS
				&& _wcsicmp(MsiRun, MsiInReg)) {
					CopyFile(FullMsiRun, FullMsiInReg, FALSE);
					::RegSetValueEx(hKey, _T("PackageName"), 0, REG_SZ, (BYTE *) MsiRun, ((DWORD)wcslen(MsiRun) + 1) * sizeof(TCHAR));
				}
			::RegCloseKey(hKey);
		}
	}
	return ERROR_SUCCESS;
}
