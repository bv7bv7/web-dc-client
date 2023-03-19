#pragma once
#include <Windows.h>
#include <Lmcons.h>
#include "mystd.h"

#define PIPE_TO_GUI_NAME   "\\\\.\\pipe\\xn39NhruyMwCMk1" // ��� ������ ��� ������ ����������� � ���������������� �����������
#define PIPE_FROM_GUI_NAME "\\\\.\\pipe\\Ck5oGh5KZpSt32t" // ��� ������ ��� ������ ����������� � ���������������� �����������
#define DCGUI_ID           "RduT6ywfVTtmuAb"              // ������������� ����������������� ����������
#define MUTEX_UPDATE       _T("Global\\DCUpadte")
#define MUTEX_UPDATENOW    _T("Global\\DCUpdateNow")
#define MAX_URL  30
#define MAX_USER 30
#define MAX_PASS 30
#define MAX_PURSE25  63
#define MAX_WM6      13
#define MAX_CPU      32
#define MANUFACTURER _T("bv7")
#define MAX_GPU10017     5
#define MAX_GPU10018     8
#define MAX_GPU          MAX_GPU10018
#define WORKLOAD_DEFAULT 1 // 1, 8, 40, 80, 160, 400, 800
#define MAX_GPU7         4
#define WORKLOAD_COUNT   7
#define WORKLOAD_MAX   800
#define WATCHDOG_MAX     150 // ������������ ����������� �����������
#define WATCHDOG_MIN      60 // ����������� ����������� �����������
#define WATCHDOG_DEFAULT  90 // ����������� ����������� �� ���������
#define PAUSE_MIN       5 // ����������� ����� ����� ��������
#define PAUSE_MAX     180 // ������������ ����� ����� ��������
#define PAUSE_DEFAULT   5 // ����� ����� �������� �� ���������

#define STATE_UNCHECK         0x000 // ��������� ��������� ����� ���������
#define STATE_CHECKS          0x001 // ���� ����������� ��������� ���������� ��������
#define STATE_UNSUPPORTED     0x002 // ����� ���� �������� ��������� �������
#define STATE_SRV_UNAVAILABLE 0x003 // ������ ����������
#define STATE_REGISTRATION    0x004 // ��������������� �� �������
#define STATE_WAIT_JOB        0x005 // �������� �������
#define STATE_JOB_UNAVAILABLE 0x006 // ��� �������
#define STATE_CALC            0x007 // ���� ����������
#define STATE_SRV_UNAUTH      0x008 // ����������� ��������������
#define STATE_UPDATE          0x009 // ���� ����������
#define STATE_MASK1           0x00F // ����� ���������������� ���������
#define STATE_PAUSED          0x020 // ���, �����������, ��� ������ ������������� �������������
#define STATE_WATCHDOG        0x040 // ���, �����������, ��� ������ �������� ������������� ��� ���������
#define STATE_ERROR_TMP       0x080 // ������ ������� � ��������� ������
#define STATE_ERROR_CALC      0x100 // �� ��������� ������ � �����������
#define STATE_BEFORE_RUN      0x200 // ����� ����� �������� ����� �������� ����������
#define STATE_MASK2           0x2F0 // ����� ������ (���������� ��� ����������)
#define STATE_MASK3           0x28F
#define STATE_MASK_STOPPED    0x1E0
#define STATE_MASK_PAUSE_TIME (STATE_ERROR_TMP | STATE_BEFORE_RUN)

#define TIME_CHECK_SNDGUI   1000 // ������ ������ �� �������� ��������� ����������������� ����������
#define TIME_COOLING        5000 // ������������ ��� ��������� �����������
#define TIME_COOLING_MAX  250000 // ������������ ���-�� ��. ��� ��������� �����������
#define TIME_CLOSE_MAX      5000
#define APP_NAME          _T("DCClient")
#define APP_TEMP_SUBPATH  APP_NAME

#define LOCALE   _T("ru")                            // ����

#define FILE_UPD APP_NAME PLATFORM LOCALE _T(".msi") // ���� ����������
#define URL_UPD  _T("http://87.117.255.35/")          // ����� ������� ���������� ��

#define UPDATER_WINDOW_CLASS _T("DCUpdaterWindowClass") // ����� ���� - ����������� �������

typedef DWORD USER_STATE;

#define PUBLIC_DATA PUBLIC_DATA10018

typedef DWORD           TEMP;
typedef unsigned __int8 UTIL;
typedef unsigned __int8 FAN;

struct GPU_STRUCT
{
	BOOL Use;
	DWORD NOTUSE_Temp;
};
struct WORKLOAD_STRUCT
{
	bool             Use;
	unsigned __int16 Value;
};
struct PUBLIC_DATA10018ADD
{
	GPU_STRUCT      GPU[MAX_GPU10018 - MAX_GPU10017];
	WORKLOAD_STRUCT Workload;
};
struct PUBLIC_DATA10018{  // ��������� ������������ ���������
	DWORD ver;
	struct PROXY {          // ��������� ������
		DWORD dwEnable;
		TCHAR tHTTP[MAX_URL + 1];
		TCHAR tUser[MAX_USER + 1];
		TCHAR tPass[MAX_PASS + 1];
	} Proxy;
	struct USER{ // ��������� ������������
		USER_STATE dwState;
		TCHAR tEmail[MAX_URL + 1];
		TCHAR tWM[MAX_WM6 + 1];
	} User;
	TEMP Watchdog; // ������������ ����������� �����������
	typedef enum {
		RUN_TYPE_LM, // When the computer turns     (������������� ��� ��������� ����������)
		RUN_TYPE_AU, // After logon of any user     (������������� ����� ����� � ������� ������ ������������)
		RUN_TYPE_CU, // After logon of current user (������������� ����� ����� � ������� �������� ������������)
		RUN_TYPE_MN  // Manually                    (�������)
	} RUN_TYPE;
	RUN_TYPE RunType;
	struct NOTUSE_WORK {
		__int64 NOTUSE_Count;
		double NOTUSE_Money;
	} NOTUSE_WorkAccounted, NOTUSE_WorkWaited, NOTUSE_WorkPrev;
	TCHAR UserName[UNLEN + 1];
	time_t NOTUSE_TimeLeft;
	GPU_STRUCT GPU[MAX_GPU10018];
	BOOL WatchdogUse;
	BOOL Test;          // Use Test Servers
	BOOL ChangeServers; // Change Server when not Available
	WORKLOAD_STRUCT Workload;
};
struct PUBLIC_DATA9{  // ��������� ������������ ���������
	DWORD ver;
	struct PROXY {          // ��������� ������
		DWORD dwEnable;
		TCHAR tHTTP[MAX_URL + 1];
		TCHAR tUser[MAX_USER + 1];
		TCHAR tPass[MAX_PASS + 1];
	} Proxy;
	struct USER{ // ��������� ������������
		USER_STATE dwState;
		TCHAR tEmail[MAX_URL + 1];
		TCHAR tWM[MAX_WM6 + 1];
	} User;
	DWORD Watchdog; // ������������ ����������� �����������
	typedef enum {
		RUN_TYPE_LM, // When the computer turns     (������������� ��� ��������� ����������)
		RUN_TYPE_AU, // After logon of any user     (������������� ����� ����� � ������� ������ ������������)
		RUN_TYPE_CU, // After logon of current user (������������� ����� ����� � ������� �������� ������������)
		RUN_TYPE_MN  // Manually                    (�������)
	} RUN_TYPE;
	RUN_TYPE RunType;
	struct NOTUSR_WORK{
		__int64 NOTUSE_Count;
		double NOTUSE_Money;
	} NOTUSE_WorkAccounted, NOTUSE_WorkWaited, NOTUSE_WorkPrev;
	TCHAR UserName[UNLEN + 1];
	time_t NOT_USE_TimeLeft;
	struct {
		BOOL Use;
		DWORD NOTUSE_Temp;
	} GPU[MAX_GPU10017];
	BOOL WatchdogUse;
	BOOL Test;          // Use Test Servers
	BOOL ChangeServers; // Change Server when not Available
};
struct PUBLIC_DATA8{  // ��������� ������������ ���������
	DWORD ver;
	struct PROXY {          // ��������� ������
		DWORD dwEnable;
		TCHAR tHTTP[MAX_URL + 1];
		TCHAR tUser[MAX_USER + 1];
		TCHAR tPass[MAX_PASS + 1];
	} Proxy;
	struct USER{ // ��������� ������������
		USER_STATE dwState;
		TCHAR tEmail[MAX_URL + 1];
		TCHAR tWM[MAX_WM6 + 1];
	} User;
	DWORD Watchdog; // ������������ ����������� �����������
	typedef enum {
		RUN_TYPE_LM, // When the computer turns     (������������� ��� ��������� ����������)
		RUN_TYPE_AU, // After logon of any user     (������������� ����� ����� � ������� ������ ������������)
		RUN_TYPE_CU, // After logon of current user (������������� ����� ����� � ������� �������� ������������)
		RUN_TYPE_MN  // Manually                    (�������)
	} RUN_TYPE;
	RUN_TYPE RunType;
	struct NOTUSE_WORK {
		__int64 NOTUSE_Count;
		double NOTUSE_Money;
	} NOTUSE_WorkAccounted, NOTUSE_WorkWaited, NOTUSE_WorkPrev;
	TCHAR UserName[UNLEN + 1];
	time_t NOTUSE_TimeLeft;
	struct {
		BOOL Use;
		DWORD NOTUSE_Temp;
	} GPU[MAX_GPU10017];
	BOOL WatchdogUse;
};
struct PUBLIC_DATA7{  // ��������� ������������ ���������
	DWORD ver;
	struct PROXY {          // ��������� ������
		DWORD dwEnable;
		TCHAR tHTTP[MAX_URL + 1];
		TCHAR tUser[MAX_USER + 1];
		TCHAR tPass[MAX_PASS + 1];
	} Proxy;
	struct USER{ // ��������� ������������
		USER_STATE dwState;
		TCHAR tEmail[MAX_URL + 1];
		TCHAR tWM[MAX_WM6 + 1];
	} User;
	DWORD Watchdog; // ������������ ����������� �����������
	typedef enum {
		RUN_TYPE_LM, // When the computer turns     (������������� ��� ��������� ����������)
		RUN_TYPE_AU, // After logon of any user     (������������� ����� ����� � ������� ������ ������������)
		RUN_TYPE_CU, // After logon of current user (������������� ����� ����� � ������� �������� ������������)
		RUN_TYPE_MN  // Manually                    (�������)
	} RUN_TYPE;
	RUN_TYPE RunType;
	struct NOTUSE_WORK {
		__int64 NOTUSE_Count;
		double NOTUSE_Money;
	} NOTUSE_WorkAccounted, NOTUSE_WorkWaited, NOTUSE_WorkPrev;
	TCHAR UserName[UNLEN + 1];
	time_t NOTUSE_TimeLeft;
	struct {
		BOOL Use;
		DWORD NOTUSE_Temp;
	} GPU[MAX_GPU7];
	BOOL WatchdogUse;
};
struct PUBLIC_DATA6{  // ��������� ������������ ���������
	struct PROXY {          // ��������� ������
		DWORD dwEnable;
		TCHAR tHTTP[MAX_URL];
		TCHAR tUser[MAX_USER];
		TCHAR tPass[MAX_PASS];
	} Proxy;
	struct USER{ // ��������� ������������
		USER_STATE dwState;
		TCHAR tEmail[MAX_URL];
		TCHAR tWM[MAX_WM6];
	} User;
	DWORD Watchdog; // ������������ ����������� �����������
	DWORD Temperature; // ������� ����������� �����������;
	typedef enum {
		RUN_TYPE_LM, // When the computer turns     (������������� ��� ��������� ����������)
		RUN_TYPE_AU, // After logon of any user     (������������� ����� ����� � ������� ������ ������������)
		RUN_TYPE_CU, // After logon of current user (������������� ����� ����� � ������� �������� ������������)
		RUN_TYPE_MN  // Manually                    (�������)
	} RUN_TYPE;
	RUN_TYPE RunType;
	struct NOTUSE_WORK {
		__int64 NOTUSE_Count;
		double NOTUSE_Money;
	} NOTUSE_WorkAccounted, NOTUSE_WorkWaited, NOTUSE_WorkPrev;
	TCHAR UserName[UNLEN + 1];
	time_t NOTUSE_TimeLeft;
	BOOL     UseGPU[MAX_GPU7];
	DWORD NOTUSE_Temp[MAX_GPU7];
};

struct MESSAGE_UPDATE {
	TCHAR msi[MAX_PATH];
};

#define PIPE_GUI_IN_SIZE  (sizeof MESSAGE_UPDATE) // ������ ��������� ������ ������

BOOL GetTempSubPath(LPTSTR TempPath);

struct ID_DATA{ // ����������������� ������ �������
	TCHAR tID[MAX_USER];
	TCHAR tPass[MAX_PASS];
};
struct ID_DATA10012{ // ����������������� ������ ������� (� ��������� 1.0.12 ������)
	TCHAR tID[MAX_USER + 1];
	TCHAR tPass[MAX_PASS + 1];
};

#define STAT_COUNT   11
#define STAT_DEFAULT 5 // 2, 5, 10, 15, 30, 60, 120, 180, 240, 300, ~600
#define STAT_MIN     2
#define TOPUBLIC_DATA9(data) (* (PUBLIC_DATA9 *) &(data))

enum GPU_ARCHITECTURE {
	GPU_CUDA = 0,
	GPU_OCL  = 1
};

typedef unsigned __int64 MH;

struct NOTUSE_WORK {
	__int64 NOTUSE_Count;
	double NOTUSE_Money;
};

