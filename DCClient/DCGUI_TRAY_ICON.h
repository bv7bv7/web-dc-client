#pragma once
#include "tray_icon.h"

class DCGUI_TRAY_ICON :
	public TRAY_ICON
{
public:
	DCGUI_TRAY_ICON(void);
	~DCGUI_TRAY_ICON(void);
protected:
	virtual void LButtonDown();
};

