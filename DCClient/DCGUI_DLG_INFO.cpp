#pragma once
#include <time.h>
#include "DCGUI_DLG_INFO.h"
#include "..\CalcService\SERVICE_CALC_CLASS.h"

void DCGUI_DLG_INFO::Command(UINT Id){
	switch(Id){
	case IDC_BUTTON_SETTINGS:
		SendMessage(hWndParent, WM_COMMAND, MAKELONG(IDM_SETTINGS, 0), 0);
		break;
	case IDC_RADIO_START:
		if (IsDlgButtonChecked(hWnd, IDC_RADIO_START) == BST_CHECKED)
			SendMessage(hWndParent, WM_COMMAND, MAKELONG(IDM_START, 0), 0);
		break;
	case IDC_RADIO_STOP:
		if (IsDlgButtonChecked(hWnd, IDC_RADIO_STOP) == BST_CHECKED)
			SendMessage(hWndParent, WM_COMMAND, MAKELONG(IDM_STOP, 0), 0);
		break;
	case IDC_BUTTON_UPD:
		lpApp->UpdateStart();
		break;
	case IDC_BUTTON_RESET_TS:
		lpApp->Total.TimeBegin = 0;
		break;
	default:
		if (! LogEdit.Command(Id))
			DIALOG::Command(Id);
	}
}

void DCGUI_DLG_INFO::Timer(WPARAM wParam) {
	switch(wParam){
	case IDT_WAIT:
		TimerShow();
	}
}

void DCGUI_DLG_INFO::TimerShow()
{
	TCHAR s[LEN10_VAR(time_t) + 1];
	time_t Now = time(NULL);
	_i64tot_s((unsigned __int64) (lpApp->TimeLeft > Now? lpApp->TimeLeft - Now: 0), s, ARRAY_LEN(s), 10);
	SetDlgItemText(hWnd, IDC_TIME_LEFT, s);
}

void DCGUI_DLG_INFO::Init(){
	DIALOG::Init();
	LogEdit.Init(&lpApp->tErrorLogFile);
	SetWindowText(hWnd, _T("DC Client ") _TCH(V0) _T(".") _TCH(V1) _T(".") _TCH(V2));
	ShowData(TRUE);
}


#define DCGUI_DLG_INFO_SHOW_GPU_CTRL(Indicator,IdArray) \
	hControl = GetDlgItem(hWnd, IdArray[i]); \
	CurValueNeed = lpApp->GPU[i].Indicator > 0; \
	if (CurValueNeed) { \
		_ITOT_S10(lpApp->GPU[i].Indicator, s); \
		SetDlgItemText(hWnd, IdArray[i], s); \
	}; \
	ShowWindow(hControl, CurValueNeed? SW_SHOW: SW_HIDE);

void DCGUI_DLG_INFO::ShowData(BOOL Init){
	HWND hControl;
	BOOL Paused = (lpApp->PublicData.User.dwState & STATE_PAUSED) != 0;
	BOOL Watchdog = !Paused && (lpApp->PublicData.User.dwState & STATE_WATCHDOG);
	TCHAR s[100];
	SetDlgItemText(hWnd, IDC_STATIC_SERVER, lpApp->Servers.Reg.Items[lpApp->Servers.Current].Name);
	_TCSCPY_S(s, _T("ID: "));
	_TCSCAT_S(s, lpApp->Servers.Reg.Items[lpApp->Servers.Current].IDData.tID);
	SetDlgItemText(hWnd, IDC_STATIC_ID, s);

	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_WAC), lpApp->mhAccounted != 0? SW_SHOW: SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_WAM), lpApp->mhAccounted != 0? SW_SHOW: SW_HIDE);
	if (lpApp->mhAccounted != 0) {
		_ui64tot_s(lpApp->mhAccounted / 1000, s, _countof(s), 10);
		SetDlgItemText(hWnd, IDC_STATIC_WAC, s);
		_stprintf_s(s, _countof(s), _T("%-.6f$"), (double) lpApp->moneyAccounted);
		SetDlgItemText(hWnd, IDC_STATIC_WAM, s);
	}

	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_WWC), lpApp->mhWaited != 0? SW_SHOW: SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_WWM), lpApp->mhWaited != 0? SW_SHOW: SW_HIDE);
	if (lpApp->mhWaited != 0) {
		_ui64tot_s(lpApp->mhWaited / 1000, s, _countof(s), 10);
		SetDlgItemText(hWnd, IDC_STATIC_WWC, s);
		_stprintf_s(s, _countof(s), _T("%-.6f$"), MH2$(lpApp->mhWaited, lpApp->PriceGhs));
		SetDlgItemText(hWnd, IDC_STATIC_WWM, s);
	}

	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_WPC), lpApp->mhPrev != 0? SW_SHOW: SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_WPM), lpApp->mhPrev != 0? SW_SHOW: SW_HIDE);
	if (lpApp->mhPrev != 0) {
		_ui64tot_s(lpApp->mhPrev / 1000, s, _countof(s), 10);
		SetDlgItemText(hWnd, IDC_STATIC_WPC, s);
		_stprintf_s(s, _countof(s), _T("%-.6f$"), (double) lpApp->moneyPrev);
		SetDlgItemText(hWnd, IDC_STATIC_WPM, s);
	}

	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_PRICE), lpApp->PriceGhs != 0? SW_SHOW: SW_HIDE);
	if (lpApp->PriceGhs != 0) {
		_stprintf_s(s, _countof(s), _T("%-.6f$"), (double) lpApp->PriceGhs);
		SetDlgItemText(hWnd, IDC_STATIC_PRICE, s);
	}

	if ((IsDlgButtonChecked(hWnd, IDC_RADIO_START) == BST_CHECKED) == Paused)
		CheckDlgButton(hWnd, IDC_RADIO_START, Paused? BST_UNCHECKED: BST_CHECKED);
	if ((IsDlgButtonChecked(hWnd, IDC_RADIO_STOP) == BST_CHECKED) != Paused)
		CheckDlgButton(hWnd, IDC_RADIO_STOP, Paused? BST_CHECKED: BST_UNCHECKED);
	BOOL TimerNeed = FALSE;
	if (Watchdog) {
		TimerNeed = TRUE;
		SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Suspended due to overheating\nПриостановлено из-за перегрева"));
	} else
		if (lpApp->PublicData.User.dwState & STATE_BEFORE_RUN) {
			TimerNeed = TRUE;
			SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Pause before starting ...\nПауза перед запуском..."));
		} else
	{
		if (Paused)
		{
			if (lpApp->PublicData.User.dwState & STATE_ERROR_CALC) {
				if (lpApp->CalcErrorMessage[0])
					SetDlgItemText(hWnd, IDC_STATIC_STATE, lpApp->CalcErrorMessage);
				else
					SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Unexpected error GPU\nНепредвиденная ошибка GPU"));
			} else
				if ((lpApp->PublicData.User.dwState & STATE_MASK1) == STATE_UNSUPPORTED)
					SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Hardware is not supported\nОборудование не поддерживается"));
				else
					SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Stopped\nОстановлено"));
		}
		else
			if (lpApp->PublicData.User.dwState & STATE_ERROR_TMP) {
				TimerNeed = TRUE;
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("No access to the temporary folder\nНет доступа к папке временных файлов"));
			} else
			switch (lpApp->PublicData.User.dwState & STATE_MASK1)
			{
			case STATE_ERROR_TMP:
				TimerNeed = TRUE;
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("No access to the temporary folder\nНет доступа к папке временных файлов"));
				break;
			case STATE_UNCHECK:
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("The first launch\nПервый запуск"));
				break;
			case STATE_CHECKS:
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Test\nТест"));
				break;
			case STATE_SRV_UNAVAILABLE:
				TimerNeed = TRUE;
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Server unavailable. Wait...\nСервер недоступен. Ожидание..."));
				break;
			case STATE_REGISTRATION:
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Registered on the server\nРегистрация на сервере"));
				break;
			case STATE_WAIT_JOB:
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Waiting for jobs\nОжидание задания"));
				break;
			case STATE_JOB_UNAVAILABLE:
				TimerNeed = TRUE;
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Jobs unavailable. Wait...\nЗадания недоступны. Ожидание..."));
				break;
			case STATE_CALC:
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("The job runs\nЗадание выполняется"));
				break;
			case STATE_SRV_UNAUTH:
				TimerNeed = TRUE;
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("Authorization is suspended. Wait...\nАвторизация приостановлена. Ожидание..."));
				break;
			case STATE_UPDATE:
				SetDlgItemText(hWnd, IDC_STATIC_STATE, _T("The program is updated\nПрограмма обновляется"));
				break;
			}
	}
	const int IdStaticTemp[MAX_GPU] = {
		IDC_STATIC_TEMP0
		, IDC_STATIC_TEMP1
		, IDC_STATIC_TEMP2
		, IDC_STATIC_TEMP3
		, IDC_STATIC_TEMP4
		, IDC_STATIC_TEMP5
		, IDC_STATIC_TEMP6
		, IDC_STATIC_TEMP7
	};
	DEFINE_CONST_IDC_PROGRESS_TEMP;
	const int IdStaticUtil[MAX_GPU] = {IDC_STATIC_UTIL0, IDC_STATIC_UTIL1, IDC_STATIC_UTIL2, IDC_STATIC_UTIL3, IDC_STATIC_UTIL4, IDC_STATIC_UTIL5, IDC_STATIC_UTIL6, IDC_STATIC_UTIL7};
#if V0 >= 3
	const int IdStaticSpeed[MAX_GPU] = {IDC_STATIC_SPEED0, IDC_STATIC_SPEED1, IDC_STATIC_SPEED2, IDC_STATIC_SPEED3, IDC_STATIC_SPEED4, IDC_STATIC_SPEED5, IDC_STATIC_SPEED6, IDC_STATIC_SPEED7};
#endif
#if V0 >= 4
	const int IdStaticFan[MAX_GPU] = {IDC_STATIC_FAN0, IDC_STATIC_FAN1, IDC_STATIC_FAN2, IDC_STATIC_FAN3, IDC_STATIC_FAN4, IDC_STATIC_FAN5, IDC_STATIC_FAN6, IDC_STATIC_FAN7};
#endif
	bool CurValueNeed;
	for (__int8 i = 0; i < MAX_GPU; i++) {
		DCGUI_DLG_INFO_SHOW_GPU_CTRL(Temp.Current,IdStaticTemp)
		hControl = GetDlgItem(hWnd, IdProgressTemp[i]);
		if (CurValueNeed)
			SendMessage(hControl, PBM_SETRANGE, 0, MAKELPARAM(0, lpApp->PublicData.Watchdog));
		SendMessage(hControl, PBM_SETPOS, CurValueNeed? lpApp->GPU[i].Temp.Current: 0, 0);
		DCGUI_DLG_INFO_SHOW_GPU_CTRL(Util.Current,IdStaticUtil)
#if V0 >= 3
		DCGUI_DLG_INFO_SHOW_GPU_CTRL(Speed,IdStaticSpeed)
#endif
#if V0 >= 4
		DCGUI_DLG_INFO_SHOW_GPU_CTRL(Fan.Current,IdStaticFan)
#endif
	}
	hControl = GetDlgItem(hWnd, IDC_SPEED);
	if (! Paused)
	{
		_ITOT_S10(lpApp->Speed, s);
		SetDlgItemText(hWnd, IDC_SPEED, s);
	}
	ShowWindow(hControl, Paused? SW_HIDE: SW_SHOW);
	hControl = GetDlgItem(hWnd, IDC_PROGRESS_STATE);
	BOOL ProgressNeed = ! Paused && ! TimerNeed;
	BOOL ProgressVisible = IsWindowVisible(hControl);
	if (ProgressNeed != ProgressVisible || Init) {
		ShowWindow(hControl, ProgressNeed? SW_SHOW: SW_HIDE);
		SendMessage(hControl, PBM_SETMARQUEE, ProgressNeed, 1000);
	}
	hControl = GetDlgItem(hWnd, IDC_TIME_LEFT);
	BOOL TimerVisible = IsWindowVisible(hControl);
	if (TimerNeed) {
		if (! TimerVisible) {
			TimerShow();
			SetTimer(hWnd, IDT_WAIT, 1000, NULL);
			ShowWindow(hControl, SW_SHOW);
		}
	} else if (TimerVisible) {
			KillTimer(hWnd, IDT_WAIT);
			ShowWindow(hControl, SW_HIDE);
	}
	time_t t;
	if (lpApp->Total.TimeBegin == 0) {
		SetDlgItemText(hWnd, IDC_ST, _T(""));
		if (! Paused) {
			lpApp->Total.TimeBegin = lpApp->Total.TimeBeginCalc;
			lpApp->Total.mh        = 0;
			tm    Tm;
			if (localtime_s(&Tm, &lpApp->Total.TimeBegin) == 0)
				_tcsftime(s, _countof(s), CAPTION_TMHS _T(" %Y-%m-%d %H:%M:%S "), &Tm);
			SetDlgItemText(hWnd, IDC_CAPTION_TMHS, s);
		}
	}
	if (lpApp->Total.TimeBegin && lpApp->Total.TimeBegin != time(&t)) {
		_i64tot_s((lpApp->Total.mh + lpApp->mhWaited) / (t - lpApp->Total.TimeBegin), s, ARRAY_LEN(s), 10);
		SetDlgItemText(hWnd, IDC_ST, s);
	}
	SetDlgItemText(hWnd, IDC_EDIT_ERROR, lpApp->LastErrorMsg);
	switch(lpApp->Update.State){
	case UPDATE_OFF:
		break;
	case UPDATE_ENABLE:
		hControl = GetDlgItem(hWnd, IDC_BUTTON_UPD);
		if(! IsWindowVisible(hControl))
			ShowWindow(hControl, SW_SHOW);
		else if(! IsWindowEnabled(hControl)) {
			SetDlgItemText(hWnd, IDC_BUTTON_UPD, _T("Update\nОбновить"));
			EnableWindow(hControl, TRUE);
		}
		break;
	case UPDATE_DOWNLOAD:
		SetDlgItemText(hWnd, IDC_BUTTON_UPD, _T("Download...\nЗагрузка..."));
		EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_UPD), FALSE);
		break;
	case UPDATE_RUN:
		SetDlgItemText(hWnd, IDC_BUTTON_UPD, _T("Setup...\nУстановка..."));
		break;
	}
}

#undef DCGUI_DLG_INFO_SHOW_GPU_CTRL
