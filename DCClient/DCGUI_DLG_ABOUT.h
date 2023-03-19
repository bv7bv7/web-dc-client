#pragma once
#include "..\global\mystd.h"
#include "dialog.h"
#include "resource.h"

class SERVICE_CALC_CLASS;

class DCGUI_DLG_ABOUT :
	public DIALOG
{
public:
	virtual void Init();
	DCGUI_DLG_ABOUT(void):DIALOG(IDD_ABOUTBOX){};
	SERVICE_CALC_CLASS * lpApp;
};

