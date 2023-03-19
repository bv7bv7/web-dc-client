// v.25
#pragma once
#include "slider.h"
class DCGUI_SLIDER_GPUTEMPRETAIN :
	public SLIDER
{
public:
	DCGUI_SLIDER_GPUTEMPRETAIN(void):SLIDER(IDC_SLIDER_GPUTEMPRETAIN, WATCHDOG_MIN, WATCHDOG_MAX) {};
};

