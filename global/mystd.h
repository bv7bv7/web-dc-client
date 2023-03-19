#pragma once
#include <Windows.h>

// Debug of WG
//#define _TESTWG 1
// Debug of HCL
//#define _TESTHC 1
// Debug of Console
//#define _TESTCO 1
#define V2 31
#define V1  0

#if VER2 == 1
#define V0 2
#elif VER3 == 1
#define V0 3
#elif VER4 == 1
#define V0 4
#elif VER5 == 1
#define V0 5
#elif VER6 == 1
#define V0 6
#endif

/*
#ifdef _DEBUG
#ifndef _TESTWG
#define _TESTWG 1
#endif
#ifndef _TESTHC
#define _TESTHC 1
#endif
#ifndef _TESTCO
#define _TESTCO 1
#endif
#endif
*/
#if !_TEST && _TESTWG || _TESTHC || _TESTCO
#define _TEST 1
#endif

#ifdef _M_X64
#define PLATFORM _T("64")
#else
#define PLATFORM _T("32")
#endif

#define ARRAY_LEN(Array) (sizeof(Array) / sizeof((Array)[0]))
#define WCSNLEN_S(Str) wcsnlen_s(Str, ARRAY_LEN(Str))
#define _TCH(x) _T(CH2(x))
#define _TCSCPY_S(Dest, Source) _tcscpy_s(Dest, ARRAY_LEN(Dest), Source)
#define _TCSCAT_S(Dest, Source) _tcscat_s(Dest, ARRAY_LEN(Dest), Source)
#define STRNLEN(Source) strnlen(Source, ARRAY_LEN(Source))
#define STRNCPY_S(Dest, Source) strncpy_s(Dest, ARRAY_LEN(Dest), Source, strlen(Source) + 1)
#define MEMCPY_S(Dest, Source) memcpy_s(&(Dest), sizeof (Dest), &(Source), sizeof (Source))
#define CH2(x) #x
#define CH(x) CH2(x)
#define LEN10_SIZE(cb) ((cb)*2+(cb)/2+((cb)&1)) // Максимальное кол-во десятичных цифр в байтах
// Максимальное кол-во десятичных цифр в переменной
#define LEN10_VAR(var) LEN10_SIZE(sizeof (var))
// Преобразует число int в строку в дестичном виде
#define _ITOT_S10(Val, Dest) _itot_s(Val, Dest, ARRAY_LEN(Dest), 10)
// Преобразует число __int64 в строку в дестичном виде
#define _I64TOW_S10(Val, Dest) _i64tow_s(Val, Dest, ARRAY_LEN(Dest), 10)
#define _UI64TOT_S10(Val, Dest) _ui64tot_s(Val, Dest, ARRAY_LEN(Dest), 10)
#ifdef _UNICODE
extern inline int TSTR_S(LPWSTR Dest, int _SizeInWords, LPCSTR Source);
extern inline int TUTF8_S(LPWSTR Dest, int _SizeInWords, LPCSTR Source);
extern inline int ASTR_S(LPSTR Dest, int _SizeInWords, LPCWSTR Source);
#else
#define TSTR_S(Dest, _SizeInWords, Source) strcpy_s(Dest, _SizeInWords, Source)
#define ASTR_S(Dest, _SizeInWords, Source) strcpy_s(Dest, _SizeInWords, Source)
#endif
#define TSTR(Dest, Source) TSTR_S(Dest, _countof(Dest),Source)
#define ASTR(Dest, Source) ASTR_S(Dest, _countof(Dest),Source)
#define TUTF8(Dest, Source) TUTF8_S(Dest, _countof(Dest), Source)

// Возвращает указатель на последнее вхождение строки Req в строку Str или NULL
char * StrStrR(char * Str, char * Reg);

BOOL CreateProcess2(int Path, LPTSTR Cmd, LPTSTR Pars, PROCESS_INFORMATION &PI, LPTSTR lpCurrentDirectory = NULL, DWORD dwCreationFlags = 0);

__int32 GetOSVersion();

#define OS_FROM_VISTA() (GetOSVersion() >= NTDDI_VISTA)

inline HRESULT GetSystem32Path(LPTSTR System32Path);
HRESULT GetSysPath(int nPath, LPTSTR tPath);

#define MIN(a, b) ((a) <= (b)? (a): (b))

void KillProcess(LPTSTR Name, bool Tree = false);

// Функция преобразует строку данных st в нормальное представление.
// Результат помещается в ту же строку, что была передана в параметрах.
// Возвращает количество символов в результатате без лидирующего нуля.
DWORD UrlDecode(char * st);

void KillProcessChildTree(DWORD ProcessId);

DWORD CreateProcess10024(int Path, LPTSTR Cmd, LPTSTR Pars, LPTSTR lpCurrentDirectory = NULL, DWORD dwCreationFlags = 0, bool Wait = false, bool SystemAffinity = false);
#define CreateProcessNoWait(Path, Cmd, Pars, lpCurrentDirectory, dwCreationFlags) (CreateProcess10024(Path, Cmd, Pars, lpCurrentDirectory, dwCreationFlags, false) == 0)
#define CreateProcessWait(Path, Cmd, Pars) CreateProcess10024(Path, Cmd, Pars, NULL, 0, true)

HRESULT GetCommonAppPath(LPTSTR Path);

typedef TCHAR FILE_NAME[MAX_PATH];
typedef FILE_NAME * LPFILE_NAME;
typedef FILE_NAME & RFFILE_NAME;
void NotepadEdit(FILE_NAME FileName);
bool ScriptRun(FILE_NAME FileName, bool Wait = false, bool SystemAffinity = false);
bool ExeRun(FILE_NAME FileName, bool Wait = false, bool SystemAffinity = false);
bool myRegGetValue(LPCTSTR lpValue, DWORD dwFlags, PVOID pvData, DWORD cbData = 0, LPCTSTR lpCryptEntropy = NULL, LPDWORD lpcbNeed = NULL); // Получить значение ключа реестра
bool myRegSetValue(LPCTSTR lpValue, DWORD dwFlags, PVOID pvData, DWORD cbData, LPCTSTR lpCryptEntropy = NULL); // Записать значение ключа реестра
extern inline unsigned AHEX(const char chr); // Преобразует символ 16-ричной цифры в число
