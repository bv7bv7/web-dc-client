#include "DIALOG.h"
#include "DCGUI_DLG_SETTINGS.h"
#include "DCGUI_SLIDER_PAUSE.h"
#include "..\CalcService\SERVICE_CALC_CLASS.h"

void DCGUI_DLG_SETTINGS::HScroll(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (lParam == (LPARAM) SliderPause.hControl)
		SliderPause.Proc(message, wParam, lParam);
	else if (lParam == (LPARAM) SliderStat.hControl)
		SliderStat.Proc(message, wParam, lParam);
	else
		if (lParam == (LPARAM) SliderWorkload.hControl)
			SliderWorkload.Proc(message, wParam, lParam);
		else if (lParam == (LPARAM) SliderWatchdog.hControl)
			SliderWatchdog.Proc(message, wParam, lParam);
#if V0 >= 4
		else if (lParam == (LPARAM) SliderGPUTempRetain.hControl)
			SliderGPUTempRetain.Proc(message, wParam, lParam);
#endif
}

void DCGUI_DLG_SETTINGS::Move() {
	SliderWorkload.ToolTip.Move();
	SliderWatchdog.ToolTip.Move();
	SliderPause.ToolTip.Move();
	SliderStat.ToolTip.Move();
#if V0 >= 4
	SliderGPUTempRetain.ToolTip.Move();
#endif
}

void DCGUI_DLG_SETTINGS::Command(UINT Id){
	switch(Id){
	case IDC_CHECK_PROXY:
		CommandCheckProxy();
		break;
	case IDC_CHECK_WATCHDOG:
		CommandCheckWatchdog();
		break;
	case IDC_CHECK_WORKLOAD:
		CommandCheckWorkload();
		break;
	case IDC_RADIO_ON_CU:
	case IDC_RADIO_ON_MN:
		CommandCheckOnCU();
		break;
	case IDC_KILL_CHECK:
		CommandCheckKill();
		break;
#if V0 >= 4
	case IDC_RADIO_OCL:
	case IDC_RADIO_CUDA:
		CommandCheckGPUArchitecture();
#endif
		break;
	case IDC_CHECK_GPUTEMP:
		CommandCheckGPUTemp();
		break;
#if V0 >= 4
	case IDC_CHECK_GPUTEMPRETAIN:
		CommandCheckGPUTempRetain();
		break;
#endif
	default:
		if (! FilerRun.Command(Id) && ! FilerStop.Command(Id))
			DIALOG::Command(Id);
	}
}

void DCGUI_DLG_SETTINGS::CommandCheckProxy(){
	UINT Checked = IsDlgButtonChecked(hWnd, IDC_CHECK_PROXY);
	EnableWindow(GetDlgItem(hWnd, IDC_STATIC_PROXY_HTTP), Checked);
	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_PROXY_HTTP), Checked);
	EnableWindow(GetDlgItem(hWnd, IDC_STATIC_PROXY_USER), Checked);
	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_PROXY_USER), Checked);
	EnableWindow(GetDlgItem(hWnd, IDC_STATIC_PROXY_PASSWORD), Checked);
	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_PROXY_PASSWORD), Checked);
}

void DCGUI_DLG_SETTINGS::CommandCheckWatchdog(){
	SliderWatchdog.Enable(IsDlgButtonChecked(hWnd, IDC_CHECK_WATCHDOG) == BST_CHECKED);
}

void DCGUI_DLG_SETTINGS::CommandCheckWorkload(){
	SliderWorkload.Enable(IsDlgButtonChecked(hWnd, IDC_CHECK_WORKLOAD) == BST_CHECKED);
}

void DCGUI_DLG_SETTINGS::CommandCheckOnCU(){
	SliderPause.Enable(IsDlgButtonChecked(hWnd, IDC_RADIO_ON_CU) == BST_CHECKED);
}

void DCGUI_DLG_SETTINGS::CommandCheckKill(){
	EnableWindow(GetDlgItem(hWnd, IDC_KILL_EDIT), IsDlgButtonChecked(hWnd, IDC_KILL_CHECK) == BST_CHECKED);
}

void DCGUI_DLG_SETTINGS::Init() {
	DEFINE_CONST_IDC_CHECK_GPU;
	DEFINE_CONST_IDC_CHECK_CPU;
	DIALOG::Init();
	SliderWatchdog.Init(hWnd, (WORD) lpApp->PublicData.Watchdog);
	SetDlgItemText(hWnd, IDC_WATCHDOG_MAX,        _TCH(WATCHDOG_MAX));
	SetDlgItemText(hWnd, IDC_WATCHDOG_MIN,        _TCH(WATCHDOG_MIN));
	SetDlgItemText(hWnd, IDC_EDIT_EMAIL,          lpApp->PublicData.User.tEmail);
	SetDlgItemText(hWnd, IDC_EDIT_WM,             lpApp->RegData25.Purse);
	CheckDlgButton(hWnd, IDC_CHECK_PROXY,         lpApp->PublicData.Proxy.dwEnable);
	SetDlgItemText(hWnd, IDC_EDIT_PROXY_HTTP,     lpApp->PublicData.Proxy.tHTTP);
	SetDlgItemText(hWnd, IDC_EDIT_PROXY_USER,     lpApp->PublicData.Proxy.tUser);
	SetDlgItemText(hWnd, IDC_EDIT_PROXY_PASSWORD, lpApp->PublicData.Proxy.tPass);
	CheckDlgButton(hWnd, IDC_RADIO_ON_CU,         lpApp->PublicData.RunType == lpApp->PublicData.RUN_TYPE_CU);
	CheckDlgButton(hWnd, IDC_RADIO_ON_MN,         lpApp->PublicData.RunType == lpApp->PublicData.RUN_TYPE_MN);
	CheckDlgButton(hWnd, IDC_CHECK_SERVER_CHANGE, lpApp->PublicData.ChangeServers);
	CheckDlgButton(hWnd, IDC_CHECK_SERVER_TEST,   lpApp->PublicData.Test);
	for (__int8 i = 0; i < MAX_GPU; i++)
		CheckDlgButton(hWnd, IdcCheckGPU[i], lpApp->PublicData.GPU[i].Use);
	CheckDlgButton(hWnd, IDC_CHECK_WATCHDOG, lpApp->PublicData.WatchdogUse);
	CommandCheckProxy();
	SliderWorkload.Init(hWnd, lpApp->PublicData.Workload.Value);
	CheckDlgButton(hWnd, IDC_CHECK_WORKLOAD, lpApp->PublicData.Workload.Use? BST_CHECKED: BST_UNCHECKED);
	CommandCheckWorkload();
	SliderPause.Init(hWnd, (WORD) lpApp->RegData10020.SecondsBeforeRun);
	CommandCheckOnCU();
	FilerRun.Init(hWnd, &lpApp->RegData10021.ScriptAltRun);
	FilerStop.Init(hWnd, &lpApp->RegData10021.ScriptAltStop);
	SetDlgItemText(hWnd, IDC_KILL_EDIT, lpApp->RegData10021.ProcessesAlt.Name);
	CheckDlgButton(hWnd, IDC_KILL_CHECK, lpApp->RegData10021.ProcessesAlt.Enable? BST_CHECKED: BST_UNCHECKED);
	CommandCheckKill();
	SliderStat.Init(hWnd, lpApp->RegData10023.SecondsStat);
	DWORD dw = 1;
	for (unsigned __int8 i = 0; i < MAX_CPU; i++, dw <<= 1) {
		CheckDlgButton(hWnd, IdcCheckCPU[i], lpApp->RegData25.CPUAffinityMask & dw? BST_CHECKED: BST_UNCHECKED);
		EnableWindow(GetDlgItem(hWnd, IdcCheckCPU[i]), lpApp->SystemAffinityMask & dw);
	}
	CheckDlgButton(hWnd, IDC_CHECK_GPUTEMP, lpApp->RegData25.GPU.Temp.Use? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hWnd, IDC_RADIO_OCL,  lpApp->RegData25.GPU.Architecture == GPU_OCL? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hWnd, IDC_RADIO_CUDA, lpApp->RegData25.GPU.Architecture == GPU_CUDA? BST_CHECKED: BST_UNCHECKED);
#if V0 >= 4
	CheckDlgButton(hWnd, IDC_CHECK_GPUTEMPRETAIN, lpApp->RegData25.GPU.Temp.Retain.Use? BST_CHECKED: BST_UNCHECKED);
	SliderGPUTempRetain.Init(hWnd, (WORD) lpApp->RegData25.GPU.Temp.Retain.Value);
	CommandCheckGPUTempRetain();
	CommandCheckGPUArchitecture();
#endif
	CommandCheckGPUTemp();
}

void DCGUI_DLG_SETTINGS::CommandOk(){
	GetDlgItemText(hWnd, IDC_EDIT_EMAIL, lpApp->PublicData.User.tEmail, ARRAY_LEN(lpApp->PublicData.User.tEmail));
	DEFINE_CONST_IDC_CHECK_CPU;
	DWORD dw = 1, dw2 = 0;
	for (unsigned __int8 i = 0; i < MAX_CPU; i++, dw <<= 1)
		if (IsDlgButtonChecked(hWnd, IdcCheckCPU[i]) == BST_CHECKED)
			dw2 |= dw;
	lpApp->RegData25.CPUAffinityMask = dw2 == 0? lpApp->SystemAffinityMask: dw2;
	if (lpApp->RegData25.CPUAffinityMask)
		SetProcessAffinityMask(GetCurrentProcess(), lpApp->RegData25.CPUAffinityMask);
	GetDlgItemText(hWnd, IDC_EDIT_WM, lpApp->RegData25.Purse, ARRAY_LEN(lpApp->RegData25.Purse));
	lpApp->RegData25.GPU.Architecture = IsDlgButtonChecked(hWnd, IDC_RADIO_OCL) == BST_CHECKED? GPU_OCL: GPU_CUDA;
	lpApp->RegData25.GPU.Temp.Use     = IsDlgButtonChecked(hWnd, IDC_CHECK_GPUTEMP) == BST_CHECKED;
#if V0 >= 4
	lpApp->RegData25.GPU.Temp.Retain.Value = SliderGPUTempRetain.Value;
	lpApp->RegData25.GPU.Temp.Retain.Use   = IsDlgButtonChecked(hWnd, IDC_CHECK_GPUTEMPRETAIN) == BST_CHECKED;
#endif
	myRegSetValue(_T("UserData25"), REG_BINARY, &lpApp->RegData25, sizeof(lpApp->RegData25), _T(CRYPT_USER));
	lpApp->PublicData.Watchdog = SliderWatchdog.Value;
	lpApp->PublicData.Proxy.dwEnable = IsDlgButtonChecked(hWnd, IDC_CHECK_PROXY);
	GetDlgItemText(hWnd, IDC_EDIT_PROXY_HTTP,     lpApp->PublicData.Proxy.tHTTP, ARRAY_LEN(lpApp->PublicData.Proxy.tHTTP));
	GetDlgItemText(hWnd, IDC_EDIT_PROXY_USER,     lpApp->PublicData.Proxy.tUser, ARRAY_LEN(lpApp->PublicData.Proxy.tUser));
	GetDlgItemText(hWnd, IDC_EDIT_PROXY_PASSWORD, lpApp->PublicData.Proxy.tPass, ARRAY_LEN(lpApp->PublicData.Proxy.tPass));
	if(IsDlgButtonChecked(hWnd, IDC_RADIO_ON_CU)) {
		DWORD cbSize = ARRAY_LEN(lpApp->PublicData.UserName);
		GetUserName(lpApp->PublicData.UserName, &cbSize);
		lpApp->PublicData.RunType = lpApp->PublicData.RUN_TYPE_CU;
	} else
		if(IsDlgButtonChecked(hWnd, IDC_RADIO_ON_MN)) lpApp->PublicData.RunType = lpApp->PublicData.RUN_TYPE_MN;
	lpApp->PublicData.ChangeServers = IsDlgButtonChecked(hWnd, IDC_CHECK_SERVER_CHANGE);
	lpApp->PublicData.Test          = IsDlgButtonChecked(hWnd, IDC_CHECK_SERVER_TEST);
	DEFINE_CONST_IDC_CHECK_GPU;
	BOOL UseAny = FALSE;
	for (__int8 i = 0; i < MAX_GPU; i++) {
		lpApp->PublicData.GPU[i].Use = IsDlgButtonChecked(hWnd, IdcCheckGPU[i]);
		UseAny = UseAny || lpApp->PublicData.GPU[i].Use;
	}
	if (! UseAny)
		for (__int8 i = 0; i < MAX_GPU; i++)
			lpApp->PublicData.GPU[i].Use = TRUE;
	lpApp->PublicData.WatchdogUse        = IsDlgButtonChecked(hWnd, IDC_CHECK_WATCHDOG);
	lpApp->PublicData.Workload.Value     = SliderWorkload.Value;
	lpApp->PublicData.Workload.Use       = IsDlgButtonChecked(hWnd, IDC_CHECK_WORKLOAD) != 0;
	lpApp->RegData10020.SecondsBeforeRun = SliderPause.Value;
	FilerRun.Ok();
	FilerStop.Ok();
	GetDlgItemText(hWnd, IDC_KILL_EDIT, lpApp->RegData10021.ProcessesAlt.Name, ARRAY_LEN(lpApp->RegData10021.ProcessesAlt.Name));
	lpApp->RegData10021.ProcessesAlt.Enable = IsDlgButtonChecked(hWnd, IDC_KILL_CHECK) == BST_CHECKED;
	myRegSetValue(_T("UserData10020"), REG_BINARY, &lpApp->RegData10020, sizeof(lpApp->RegData10020), _T(CRYPT_USER));
	myRegSetValue(_T("UserData10021"), REG_BINARY, &lpApp->RegData10021, sizeof(lpApp->RegData10021), _T(CRYPT_USER));
	lpApp->RegData10023.SecondsStat = SliderStat.Value;
	myRegSetValue(_T("UserData10023"), REG_BINARY, &lpApp->RegData10023, sizeof(lpApp->RegData10023), _T(CRYPT_USER));
	for(__int8 i = 0; i < _countof(lpApp->Servers.Reg.Items); i++)
		lpApp->Servers.Reg.Items[i].ChangeUser = true;
	myRegSetValue(_T("Servers25"), REG_BINARY, &lpApp->Servers.Reg, sizeof(lpApp->Servers.Reg), _T(CRYPT_SERVERS));
	lpApp->PublicData.User.dwState = (lpApp->PublicData.User.dwState | STATE_WATCHDOG) ^ STATE_WATCHDOG;
	lpApp->RegSetUserData();
	SetEvent(lpApp->EventReconfig);
	lpApp->DlgInfo.ShowData();
	DIALOG::CommandOk();
}

void DCGUI_DLG_SETTINGS::CommandCheckGPUTemp() {
	UINT Checked = IsDlgButtonChecked(hWnd, IDC_CHECK_GPUTEMP);
#if V0 >= 4
	BOOL EnableGPUTempRetain = Checked == BST_CHECKED && IsDlgButtonChecked(hWnd, IDC_RADIO_OCL) == BST_CHECKED;
	if (! EnableGPUTempRetain) {
		CheckDlgButton(hWnd, IDC_CHECK_GPUTEMPRETAIN, BST_UNCHECKED);
		CommandCheckGPUTempRetain();
	}
	EnableWindow(GetDlgItem(hWnd, IDC_CHECK_GPUTEMPRETAIN), EnableGPUTempRetain);
	if (Checked == BST_UNCHECKED) {
		CheckDlgButton(hWnd, IDC_CHECK_WATCHDOG, BST_UNCHECKED);
		CommandCheckWatchdog();
	}
#else
	CheckDlgButton(hWnd, IDC_CHECK_WATCHDOG, Checked);
	CommandCheckWatchdog();
#endif
	EnableWindow(GetDlgItem(hWnd, IDC_CHECK_WATCHDOG), Checked == BST_CHECKED);
}

#if V0 >= 4

void DCGUI_DLG_SETTINGS::CommandCheckGPUArchitecture() {
	BOOL EnableGPUTempRetain = IsDlgButtonChecked(hWnd, IDC_RADIO_OCL) == BST_CHECKED && IsDlgButtonChecked(hWnd, IDC_CHECK_GPUTEMP) == BST_CHECKED;
	if (! EnableGPUTempRetain) {
		CheckDlgButton(hWnd, IDC_CHECK_GPUTEMPRETAIN, BST_UNCHECKED);
		CommandCheckGPUTempRetain();
	}
	EnableWindow(GetDlgItem(hWnd, IDC_CHECK_GPUTEMPRETAIN), EnableGPUTempRetain);
}

void DCGUI_DLG_SETTINGS::CommandCheckGPUTempRetain(){
	SliderGPUTempRetain.Enable(IsDlgButtonChecked(hWnd, IDC_CHECK_GPUTEMPRETAIN) == BST_CHECKED);
}

#endif

