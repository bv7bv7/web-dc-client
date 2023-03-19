#pragma once
#include "dialog.h"
#include "DCGUI_BEF_LOG.h"

#define DEFINE_CONST_IDC_PROGRESS_TEMP const int IdProgressTemp[] = {IDC_PROGRESS_TEMP0, IDC_PROGRESS_TEMP1, IDC_PROGRESS_TEMP2, IDC_PROGRESS_TEMP3, IDC_PROGRESS_TEMP4, IDC_PROGRESS_TEMP5, IDC_PROGRESS_TEMP6, IDC_PROGRESS_TEMP7}
#define CAPTION_TMHS     _T("Total MH/s from ")
#define LEN_CAPTION_TMHS (ARRAY_LEN(CAPTION_TMHS) - 1)

class SERVICE_CALC_CLASS;

#define IDT_WAIT     1


class DCGUI_DLG_INFO :
	public DIALOG
{
public:
	SERVICE_CALC_CLASS * lpApp;
	DCGUI_BEF_LOG        LogEdit;
	DCGUI_DLG_INFO():DIALOG(IDD_INFO){};
	void ShowData(BOOL Init = FALSE);
private:
	void Init();
	void Command(UINT Id);
	void Timer(WPARAM wParam);
	void TimerShow();
};

