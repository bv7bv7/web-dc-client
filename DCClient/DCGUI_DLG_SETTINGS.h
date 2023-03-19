#pragma once
#include "..\global\mystd.h"
#include "DCGUI_SLIDER_WORKLOAD.h"
#include "DCGUI_SLIDER_WATCHDOG.h"
#include "DCGUI_SLIDER_PAUSE.h"
#include "DCGUI_SLIDER_STAT.h"
#include "DCGUI_FILER.h"

#if V0 >= 4
#include "DCGUI_SLIDER_GPUTEMPRETAIN.h"
#endif

#define DEFINE_CONST_IDC_CHECK_GPU const int IdcCheckGPU[] = {IDC_CHECK_GPU0, IDC_CHECK_GPU1, IDC_CHECK_GPU2, IDC_CHECK_GPU3, IDC_CHECK_GPU4, IDC_CHECK_GPU5, IDC_CHECK_GPU6, IDC_CHECK_GPU7}
#define DEFINE_CONST_IDC_CHECK_CPU const int IdcCheckCPU[] = {IDC_CHECK_CPU0, IDC_CHECK_CPU1, IDC_CHECK_CPU2, IDC_CHECK_CPU3, IDC_CHECK_CPU4, IDC_CHECK_CPU5, IDC_CHECK_CPU6, IDC_CHECK_CPU7, IDC_CHECK_CPU8, IDC_CHECK_CPU9, IDC_CHECK_CPU10, IDC_CHECK_CPU11, IDC_CHECK_CPU12, IDC_CHECK_CPU13, IDC_CHECK_CPU14, IDC_CHECK_CPU15, IDC_CHECK_CPU16, IDC_CHECK_CPU17, IDC_CHECK_CPU18, IDC_CHECK_CPU19, IDC_CHECK_CPU20, IDC_CHECK_CPU21, IDC_CHECK_CPU22, IDC_CHECK_CPU23, IDC_CHECK_CPU24, IDC_CHECK_CPU25, IDC_CHECK_CPU26, IDC_CHECK_CPU27, IDC_CHECK_CPU28, IDC_CHECK_CPU29, IDC_CHECK_CPU30, IDC_CHECK_CPU31}

class SERVICE_CALC_CLASS;

class DCGUI_DLG_SETTINGS :
	public DIALOG
{
public:
	DCGUI_SLIDER_WORKLOAD SliderWorkload; // Регулятор агрессии
	DCGUI_SLIDER_WATCHDOG SliderWatchdog; // Ограничитель температуры
	DCGUI_SLIDER_PAUSE SliderPause;       // Регулятор паузы перед автозапуском
	void Init();
	void HScroll(UINT message, WPARAM wParam, LPARAM lParam);
	void Command(UINT Id);
	void CommandCheckProxy();
	void CommandCheckWatchdog();
	void CommandCheckWorkload();
	void CommandCheckOnCU();
	void CommandOk();
	DCGUI_FILER_RUN  FilerRun;  // Скрипт запускаемый при простое
	DCGUI_FILER_STOP FilerStop; // Скрипт запускаемый после завершения простоя
	void CommandCheckKill();
	DCGUI_SLIDER_STAT SliderStat; // Период получения статистики
	DCGUI_DLG_SETTINGS():DIALOG(IDD_USER_DATA){};
	virtual void Move();
	void CommandCheckGPUTemp();
#if V0 >= 4
	DCGUI_SLIDER_GPUTEMPRETAIN SliderGPUTempRetain; // Регулятор порога температуры для увеличения оборотов кулера
	void CommandCheckGPUArchitecture();
	void CommandCheckGPUTempRetain();
#endif
	SERVICE_CALC_CLASS * lpApp;
};

