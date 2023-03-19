#pragma once

#include "TOOLTIP_SLIDER.h"

class SLIDER
{
public:
	bool Horizontal;
	void Proc(UINT message, WPARAM wParam, LPARAM lParam);
	WORD  Value;
	DWORD PageSize;
	int Id;
	HWND hControl;
	WORD Min, Max;
	TOOLTIP_SLIDER ToolTip;
	virtual WORD ToControlValue();
	virtual WORD FromControlValue(WORD Value);
	SLIDER(int Id, WORD Min, WORD Max, WORD PageSize = 2);
	void Init(HWND hWnd, WORD Value);
	void Enable(bool Enable);
};

#define SLIDER_ARRAY_VALUES_COUNT 11

struct SLIDER_ARRAY_INDEX_STRUCT
{
	WORD Count;
};

class SLIDER_ARRAY :
	public SLIDER
{
public:
	WORD                      Values[SLIDER_ARRAY_VALUES_COUNT];
	SLIDER_ARRAY_INDEX_STRUCT Index;
	virtual WORD ToControlValue();
	virtual WORD FromControlValue(WORD Value);
	SLIDER_ARRAY(int Id, WORD Count);
};
