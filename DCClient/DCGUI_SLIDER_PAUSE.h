#pragma once
#include "..\global\global.h"
#include "resource.h"
#include "slider.h"
class DCGUI_SLIDER_PAUSE :
	public SLIDER
{
public:
	DCGUI_SLIDER_PAUSE(void):SLIDER(IDC_SLIDER_PAUSE, PAUSE_MIN, PAUSE_MAX, 10) {};
};

