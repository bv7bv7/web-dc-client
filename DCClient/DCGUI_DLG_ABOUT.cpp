#pragma once
#include "DCGUI_DLG_ABOUT.h"
#include "..\CalcService\SERVICE_CALC_CLASS.h"

void DCGUI_DLG_ABOUT::Init() {
	DIALOG::Init();
	SetDlgItemText(hWnd, IDC_STATIC_ID, lpApp->Servers.Reg.Items[lpApp->Servers.Current].IDData.tID);
	SetDlgItemText(hWnd, IDC_STATIC_VER, _T("DC Client, ver. ") _TCH(V0) _T(".") _TCH(V1) _T(".") _TCH(V2));
}

