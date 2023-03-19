#pragma once
#include <Windows.h>
#include <Commctrl.h>
#include "..\global\mystd.h"
#include "SLIDER.h"


SLIDER::SLIDER(int Id, WORD Min, WORD Max, WORD PageSize)
{
	this->Id         = Id;
	this->Min        = Min;
	this->Max        = Max;
	this->Value      = Max;
	this->PageSize   = PageSize;
}

void SLIDER::Init(HWND hWnd, WORD Value) {
	RECT Rect;
	this->Value = Value;
	hControl = GetDlgItem(hWnd, Id);
	SendMessage(hControl, TBM_GETCHANNELRECT, 0, (LPARAM) &Rect);
	Horizontal = Rect.right - Rect.left > Rect.bottom - Rect.top;
	SendMessage(hControl, TBM_SETRANGE,    (WPARAM) FALSE,  (LPARAM) MAKELONG(Min, Max));  // min. & max. positions
    SendMessage(hControl, TBM_SETTICFREQ, PageSize, NULL);
    SendMessage(hControl, TBM_SETPOS, (WPARAM) TRUE,  (LPARAM) ToControlValue());
	ToolTip.Init(hWnd, hControl, Value);
}

void SLIDER::Proc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_HSCROLL:
	case WM_VSCROLL:
		if (lParam == (LPARAM) hControl) {
		    Value = FromControlValue((WORD) SendMessage(hControl, TBM_GETPOS, NULL,  NULL));
			switch(LOWORD(wParam)) {
			case TB_THUMBTRACK:
			case TB_BOTTOM:
			case TB_LINEDOWN:
			case TB_LINEUP:
			case TB_PAGEDOWN:
			case TB_PAGEUP:
			case TB_THUMBPOSITION:
			case TB_TOP:
				ToolTip.UpdateTipText(Value);
				break;
			}
		}
	}

}

inline WORD SLIDER::ToControlValue(){
	if (Horizontal)
		return Value;
	else
		return Max - Value + Min;
}

inline WORD SLIDER::FromControlValue(WORD Value){
	if (Horizontal)
		return Value;
	else
		return Max - Value + Min;
}

void SLIDER::Enable(bool Enable) {
	EnableWindow(hControl, Enable);
	ToolTip.Show(Enable);
}

inline WORD SLIDER_ARRAY::ToControlValue(){
	for (WORD i = 0; i < this->Index.Count; i++)
		if (Values[i] == Value)
			return i;
	return 0;
}

inline WORD SLIDER_ARRAY::FromControlValue(WORD Value){
	return Values[Value];
}

SLIDER_ARRAY::SLIDER_ARRAY(int Id, WORD Count):SLIDER(Id, 0, (WORD) Count - 1, 1)
{
	this->Index.Count   = Count;
}
