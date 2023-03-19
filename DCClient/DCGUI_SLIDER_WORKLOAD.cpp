#pragma once
#include "resource.h"
#include "..\global\global.h"
#include "DCGUI_SLIDER_WORKLOAD.h"

DCGUI_SLIDER_WORKLOAD::DCGUI_SLIDER_WORKLOAD(void):SLIDER_ARRAY(IDC_SLIDER_WORKLOAD, WORKLOAD_COUNT) {
	this->Values[0] = 1;
	this->Values[1] = 8;
	this->Values[2] = 40;
	this->Values[3] = 80;
	this->Values[4] = 160;
	this->Values[5] = 400;
	this->Values[6] = 800;
}
