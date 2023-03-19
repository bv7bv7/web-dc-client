#pragma once
#include <tchar.h>
#include "..\global\global.h"
#include "resource.h"
#include "DCGUI_SLIDER_STAT.h"

DCGUI_SLIDER_STAT::DCGUI_SLIDER_STAT(void):SLIDER_ARRAY(IDC_SLIDER_STAT, STAT_COUNT)
{
// 2, 5, 10, 15, 30, 60, 120, 180, 240, 300, ~600
#if V2 >= 27
	this->Values[0] = STAT_MIN;
#else
	this->Values[0] = 1;
#endif
	this->Values[1] = 5;
	this->Values[2] = 10;
	this->Values[3] = 15;
	this->Values[4] = 30;
	this->Values[5] = 60;
	this->Values[6] = 120;
	this->Values[7] = 180;
	this->Values[8] = 240;
	this->Values[9] = 300;
	this->Values[10] = -1;
	_tcscpy_s(this->ToolTip.Special.Message, _T("~600"));
	this->ToolTip.Special.Value = this->Values[10];
	this->ToolTip.Special.Enable = true;
}
