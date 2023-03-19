#pragma once
#include "..\global\mystd.h"
#include "..\global\global.h"
#include "..\DCClient\DCGUI_DLG_INFO.h"
#include "..\DCClient\DCGUI_DLG_SETTINGS.h"
#include "..\DCClient\DCGUI_DLG_ABOUT.h"
#include "..\DCClient\SERVER.h"
#include "CALCULATOR.h"
#include "HASHCAT.h"

#define DC_HKEY HKEY_CURRENT_USER

// ������ ���������
#define APP_VER                       _TCH(V0) _TCH(V1) _TCH(V2) PLATFORM
#define ERROR_EOL                     _T("\r\n\r\n")
#define SET_ERROR_MSG(msg)            SERVICE_CALC_CLASS::Error(__FUNCTION__, __LINE__, 0, (msg))
#define SET_ERROR_CODE(code)          (SERVICE_CALC_CLASS::ErrorCode = (code)) == ERROR_SUCCESS? ERROR_SUCCESS: SERVICE_CALC_CLASS::Error(__FUNCTION__, __LINE__, SERVICE_CALC_CLASS::ErrorCode, NULL)
#define SET_ERROR                     SET_ERROR_CODE(::GetLastError())
#define SET_ERROR_TMSG(msg)           SERVICE_CALC_CLASS::ErrorT(__FUNCTION__, __LINE__, 0, (msg))
#define SET_ERROR_CODE_MSG(code, msg) SERVICE_CALC_CLASS::Error(__FUNCTION__, __LINE__, (code), (msg))

#ifdef _TEST
#define ERROR_MAX 8096
#else
#define ERROR_MAX 512
#endif

#define AGENT_NAME     APP_NAME APP_VER // ��� ������ ��� ����� � ��������
#define AGENT_PASSWORD "YqqQCyERFveh8hJ"        // ��������� ������ ��� ����������� �� �������
#define REG_PATH       _T("SOFTWARE\\bv7\\DCClient\\")
#define CRYPT_PROXY    "NLDG4sVFwke7Ya3" // ���� ��� ���������� ������ ������ �������
#define CRYPT_USER     "e9G2WaM4fYDMLH4" // ���� ��� ���������� ���������������� ������
#define CRYPT_ID       "Y9g0CEofvyaNc0H" // ���� ��� ���������� ����������������� ������
#define CRYPT_OPERATOR "DV8N2qJHhnC3Srn" // ���� ��� ���������� ���������� ����������
#define CRYPT_SERVERS  "0ydeLM4cbijZ7C7" // ���� ��� ���������� ���������� ��������
#define CONSOLE_TITLE  _T("sdsS8230HBasasS")
#define PIPE_GUI_OUT_SIZE (sizeof BUFFER_PUBLIC_DATA) // ������ ���������� ������ ������
#define SEND_PUBLIC_DATA    0x01 // �������� ����������������� ���������� ��������� ������ ���������
#define SEND_REMOVE         0x02 // �������� ����������������� ���������� ������� �� �����������
#define TIME_CHECK_PAUSE    1000 // ������ ������ �� ����� �� ��������� ������������
#define TIME_CHECK_SERVER  15000 // ������ ������ ����������� ������� ��.
#define PAGE_REG          _T("clreg.php") // C������� �����������
#define PAGE_REQ          _T("clreq.php") // C������� ��������� �������
#define PAGE_RES          _T("clres.php") // C������� ��������� ���������� ����������
#define POST_SEP_OPERATOR "-"             // ����������� ������ ������������� ���������� ��� ����������� �� �������
#define POST_SEP_SPEED    "-"             // ����������� ������ ��������� ���������� ��� ����������� �� �������
#define MAX_COOKIE        2048            // ������ ������� ��� �������� cookie
#define MAX_COOKIE_STR     256            // ������������ ������ ������ �� cookie
#define MAX_COOKIE_VAR       8            // ������������ ������ ������ �� cookie
#define MAX_COOKIE_VAL     256            // ������������ ������ ������ �� cookie
#define APPNAME_WGET "wget.exe"
#define APPNAME_MSIEXEC "msiexec.exe"

enum SRV_ERROR {
	SRV_ERROR_SUCCESS     = 0,          // �������� ��������� ��������� � �������
	SRV_ERROR_UPDATED     = 2,          // ��������� ���������� �������
	SRV_ERROR_AUTH        = 5,          // ������ ����������� (����������� ��������������)
	SRV_ERROR_REG         = 6,          // ������ ����������� (����������� - ��� ������ � ��)
	SRV_ERROR_UNAVAILABLE = 10,         // ������ ����������
	SRV_ERROR_COUNT
};

typedef TCHAR WG_PARS[MAX_PATH * 4];

struct WGET_DATA {
	WG_PARS CommandPars;
	ID_DATA IDData;
	SRV_ERROR  Error;
	OP_STR_PAR Op;
	bool    UseCookies;
	LPTSTR  lpUrl;   
};

enum SCRIPT_ALT_STATE
{
	SCRIPT_ALT_UNDEF,
	SCRIPT_ALT_STOPPED,
	SCRIPT_ALT_RUNNING
};

enum UPDATE_STATE
{
	UPDATE_OFF,
	UPDATE_ENABLE,
	UPDATE_DOWNLOAD,
	UPDATE_RUN
};

class SERVICE_CALC_CLASS {
public:
	DCGUI_DLG_SETTINGS DlgSettings; // ������ ��������
	DCGUI_DLG_ABOUT    DlgAbout;
	BOOL Exit;
	TCHAR CalcErrorMessage[256];
	struct {
		DWORD Speed;
		struct {
			TEMP         Current;
			TEMP         Max;
			TEMP         Average;
			unsigned __int16 Count;
		} Temp;

		struct {
			UTIL  Current;
			UTIL  Average;
			UTIL  Max;
			unsigned __int16 Count;
		} Util;

#if V0 >=4
		struct {
			FAN  Current;
			FAN  Average;
			FAN  Max;
			unsigned __int16 Count;
		} Fan;
#endif
	} GPU[MAX_GPU];
	struct {
		DWORD SecondsBeforeRun;
	} RegData10020;
	SERVICE_CALC_CLASS();
	~SERVICE_CALC_CLASS(void);
	HANDLE hConsoleInput;
	SERVERS Servers;
	HANDLE hThreadUpdater;
	static DWORD WINAPI StaticUpdater(LPVOID lpService);
	void UpdateStart();       // ����������.
	void Updater();
	void RegSetUserData();
	virtual DWORD Init();
	virtual void Do();
	static DWORD WINAPI StaticDo(LPVOID lpService);
	HANDLE hThreadDo;
	char Cookie[MAX_COOKIE];
	HWND hWndConsole;
	void Registration(); // ����������� ������� �� �������.
	void ReceiveJob();   // ��������� �������.
	struct {
		FILER_DATA ScriptAltRun;
		FILER_DATA ScriptAltStop;
		FILER_DATA ProcessesAlt;
	} RegData10021;
	SCRIPT_ALT_STATE ScriptAltState;  // ��������� ��������������� ��������
	void InitScript(LPFILER_DATA lpData/*, LPCTSTR File*/); // ������������� ��������� �������
	void ProcessesKill();                               // �������� �������������� ���������
	void RunProcessesKill();
	void DoScriptAltRun();                              // ������ ������� ��������������� ��������
	void DoScriptAltStop();                             // ������ ������� ��������� ��������������� ��������
	struct {
		WORD SecondsStat;
	} RegData10023;
	HANDLE EventStop;
	static DCGUI_DLG_INFO DlgInfo;              // ������ ����������
	static DWORD          ErrorCode;
	static FILE_NAME      TempPath;
	static FILE_NAME      PrgPath;
	static FILE_NAME      tErrorLogFile;
	static TCHAR          LastErrorMsg[2048];
	struct {
		TCHAR Purse[MAX_PURSE25 + 1];
		struct {
			GPU_ARCHITECTURE Architecture;
			struct {
				bool Use;
				struct {
					bool             Use;
					unsigned __int16 Value;
				} Retain;
			} Temp;
		} GPU;
		DWORD_PTR CPUAffinityMask;
	} RegData25;
	DWORD_PTR SystemAffinityMask;
	HANDLE EventReconfig;
	struct {
		time_t           TimeBegin;
		time_t           TimeBeginCalc;
		unsigned __int64 mh;
	} Total;
	FILE_NAME   CommonAppDataFolder;
	HCL         hcl;
	time_t      TimeLeft;
	HWND        hWnd;                    // ������� ����
	PUBLIC_DATA PublicData;
	double      PriceGhs, moneyPrev, moneyAccounted;
	MH          mhWaited, mhPrev, mhAccounted;
	SPEED       Speed;  // �������� ��������� ���������� �� ������� ����������� (���� ������� �� ���������� ��������� ��������, �� - 0). � ����� ���������� ����� ��������� ��� MH / sec
	static DWORD Error(char * Func, DWORD Line, DWORD Code, char * Message);
	inline static DWORD ErrorT(char * Func, DWORD Line, DWORD Code, TCHAR * Message);
	bool         wg(WGET_DATA &WgetData);          // ����� � ��������
	void         ChangeMenuState();
	void         SetState(USER_STATE State, bool RegSet = true); // ������������� ���������, �������� ��� ����������������� ���������� � ���������� � ������
	void         ResetState(USER_STATE State, bool RegSet = true); // �������� ���������, �������� ����� ��������� ����������������� ���������� � ���������� � ������
	inline void  WGFillPars1(WG_PARS & Pars);
	inline void  WGFillPars2(WG_PARS & Pars);
	inline void  WGFillPars3(WG_PARS & Pars);
	bool         DoScript(FILE_NAME lpFileName, bool Wait = false, bool SystemAffinity = false);               // ������ �������
	struct{
		UPDATE_STATE State;
		TCHAR        Url[MAX_PATH];
	} Update;
	inline void InitConsole();
};
