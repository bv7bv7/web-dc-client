#pragma once
#include <Windows.h>
#include <tchar.h>
#include "..\global\mystd.h"
#include "TOOLTIP_SLIDER.h"
#include "..\CalcService\SERVICE_CALC_CLASS.h"

void TOOLTIP_SLIDER::UpdateTipText(WORD Value)
{
	TCHAR Message[6];
	if (Special.Enable && Special.Value == Value)
		TOOLTIP::UpdateTipText(Special.Message);
	else {
		_itot_s((int) Value, Message, 10);
		TOOLTIP::UpdateTipText(Message);
	}
	Move();
}

TOOLTIP_SLIDER::TOOLTIP_SLIDER():TOOLTIP()
{
	Special.Enable  = false;
}

void TOOLTIP_SLIDER::Init(HWND hWnd, HWND hTool, WORD Value) {
	TOOLTIP::Init(hWnd, hTool);
	UpdateTipText(Value);
	Show(true);
}

void TOOLTIP_SLIDER::Show(bool Show) {
	SendMessage(hControl, TTM_TRACKACTIVATE, Show == true, (LPARAM) &ToolInfo);
	if (Show)
		Move();
}

void TOOLTIP_SLIDER::Move() {
	LPARAM lp;
	RECT Rect, ThumbRect, ChannelRect, SliderRect;
	if (!GetWindowRect((HWND) ToolInfo.uId, &SliderRect) || ! GetWindowRect(hControl, &Rect))
		SET_ERROR;
	else {
		SendMessage((HWND) ToolInfo.uId, TBM_GETTHUMBRECT,   0, (LPARAM) &ThumbRect);
		SendMessage((HWND) ToolInfo.uId, TBM_GETCHANNELRECT, 0, (LPARAM) &ChannelRect);
		if (ChannelRect.right - ChannelRect.left > ChannelRect.bottom - ChannelRect.top)
			lp = (LPARAM)MAKELONG(SliderRect.left + (ThumbRect.left + ThumbRect.right - Rect.right + Rect.left) / 2, SliderRect.top + ThumbRect.top - (Rect.bottom - Rect.top));
		else
			lp = (LPARAM)MAKELONG(SliderRect.left + ThumbRect.left - (Rect.right - Rect.left), SliderRect.top + (ThumbRect.top + ThumbRect.bottom - Rect.bottom + Rect.top) / 2);
		SendMessage(hControl, TTM_TRACKPOSITION, 0, lp);
	}
}

