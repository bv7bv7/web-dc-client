#pragma once
#include "filer.h"


class DCGUI_FILER_RUN : public FILER {
public:
	DCGUI_FILER_RUN(void):FILER(IDC_RUN_CHECK, IDC_RUN_EDIT, IDC_RUN_BUTTON, IDC_RUN_EDT){};
};

class DCGUI_FILER_STOP : public FILER {
public:
	DCGUI_FILER_STOP(void):FILER(IDC_STOP_CHECK, IDC_STOP_EDIT, IDC_STOP_BUTTON, IDC_STOP_EDT){};
};

