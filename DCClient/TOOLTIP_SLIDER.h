#pragma once
#include "tooltip.h"

#define MAX_TOOLTIP_SLIDER_SPECIAL_MESSAGE 4

typedef TCHAR TOOLTIP_SLIDER_SPECIAL_MESSAGE[MAX_TOOLTIP_SLIDER_SPECIAL_MESSAGE + 1];

struct TOOLTIP_SLIDER_SPECIAL_VALUE
{
	bool                           Enable;
	WORD                           Value;
	TOOLTIP_SLIDER_SPECIAL_MESSAGE Message;
};

class TOOLTIP_SLIDER :
	public TOOLTIP
{
public:
	void UpdateTipText(WORD Value);
	TOOLTIP_SLIDER_SPECIAL_VALUE Special;
	TOOLTIP_SLIDER();
	void Move();
	void Show(bool Show);
	void Init(HWND hWnd, HWND hTool, WORD Value);
};

