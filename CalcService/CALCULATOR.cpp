#pragma once
#include <time.h>
#include "CALCULATOR.h"
#include "SERVICE_CALC_CLASS.h"

void CCALCULATOR::Process(){
	STARTUPINFO         StartupInfo;
	OP_COUNT            Prev784Count;
	HANDLE              hpFromChildOut, hpChildOut;
	SECURITY_ATTRIBUTES PipeAttributes; 
	PipeAttributes.nLength              = sizeof(SECURITY_ATTRIBUTES); 
	PipeAttributes.bInheritHandle       = TRUE; 
	PipeAttributes.lpSecurityDescriptor = NULL;
	if(!CreatePipe(&hpFromChildOut, &hpChildOut,  &PipeAttributes, CALC_MAX_BUFF_OUT * 6))
		SET_ERROR;
	if(!SetHandleInformation(hpFromChildOut, HANDLE_FLAG_INHERIT, 0))
		SET_ERROR;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	StartupInfo.cb         = sizeof(StartupInfo);
	StartupInfo.hStdError  = hpChildOut;
	StartupInfo.hStdOutput = hpChildOut;
	StartupInfo.hStdInput  = lpApp->hConsoleInput;
	StartupInfo.dwFlags    = STARTF_USESTDHANDLES;
	PrevWatchdogCount = 0;
	HANDLE EventsHandles[3];
	EventsHandles[1] = lpApp->EventStop;
	EventsHandles[2] = lpApp->EventReconfig;
	TEMP Watchdog        = lpApp->PublicData.WatchdogUse? lpApp->PublicData.Watchdog: 0;
	ExitCode                 = EXIT_CODE_EMPTY;
	DWORD Kranch784Counter   = 0;
	Prev784Count             = 0;
	OP_COUNT Prev784ErrCount = 0;
	IsCalc                   = false;
	IsExhausted = false;
	ChangeCommandPars();
	if(CreateProcess(NULL, (LPTSTR) CommandPars, NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &ProcessInformation)) {
		DWORD ev1;
#if V0 < 3
		if (lpApp->RegData25.CPUAffinityMask && lpApp->RegData25.CPUAffinityMask != lpApp->SystemAffinityMask)
			SetProcessAffinityMask(ProcessInformation.hProcess, lpApp->RegData25.CPUAffinityMask);
#endif
		do {
			EventsHandles[0] = ProcessInformation.hProcess;
			ev1 = WaitForMultipleObjects(_countof(EventsHandles), EventsHandles, FALSE, (lpApp->RegData10023.SecondsStat == (WORD) -1) ? INFINITE : ((DWORD) lpApp->RegData10023.SecondsStat) * 1000);
			switch (ev1) {
			case WAIT_TIMEOUT:
				WriteCons(KeyStatus);
				break;
			case WAIT_OBJECT_0 + 1:
			case WAIT_OBJECT_0 + 2:
				WriteCons(KeyAbort);
				err = WaitForSingleObject(ProcessInformation.hProcess, CALC_TIME_STATE);
				if (err == WAIT_TIMEOUT) {
#if V0 >= 5
					TerminateProcess(ProcessInformation.hProcess, HASHCATLITE_ERROR_ABORTED);
#else
					TerminateProcess(ProcessInformation.hProcess, HASHCATLITE_ERROR_EXHAUSTED);
#endif
					err = WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
				}
				GetExitCodeProcess(ProcessInformation.hProcess, &CommandExitCode);
				ConvertExitCode();
				if (ExitCode == EXIT_CODE_EXHAUSTED)
					switch (ev1) {
					case WAIT_OBJECT_0 + 1:
						ExitCode = EXIT_CODE_ABORT;
						break;
					case WAIT_OBJECT_0 + 2:
						ExitCode = EXIT_CODE_RECONFIG;
						break;
					}
				break;
			case WAIT_OBJECT_0 + 0:
				GetExitCodeProcess(ProcessInformation.hProcess, &CommandExitCode);
#ifdef _TESTHC
				SET_ERROR_CODE_MSG(CommandExitCode, "Process exit code");
#endif
				break;
			}
			if (! PeekNamedPipe(hpFromChildOut, NULL, 0, 0, &size, NULL))
				SET_ERROR;
			if(size > 0 && ReadFile(hpFromChildOut, CommandOut, (sizeof CommandOut) - 1, &size, NULL) && size > 0) {
				CommandOut[size] = 0;
#ifdef _TESTHC
				SET_ERROR_MSG(CommandOut);
#endif
				ScanStatus(ev1 == WAIT_OBJECT_0 + 0);
				ChangeCount(Count);
				if (Prev784Count >= Count && ev1 == WAIT_TIMEOUT && ExitCode != EXIT_CODE_ERROR_CONSOLE)
				{
					if (Prev784ErrCount == Count) {
						Kranch784Counter++;
						if (Kranch784Counter > 2) {
							ExitCode = EXIT_CODE_KRANCH784;
							TerminateProcess(ProcessInformation.hProcess, EXIT_CODE_KRANCH784);
						}
					} else {
						Kranch784Counter = 0;
						Prev784ErrCount  = Count;
					}
					ChangeCount(Prev784Count);
				} else
					Prev784Count = Count;
			}
		} while(ev1 == WAIT_TIMEOUT && (ExitCode == EXIT_CODE_EMPTY || ExitCode == EXIT_CODE_KRANCH784));
		if (ExitCode == EXIT_CODE_EMPTY)
			ConvertExitCode();
		switch (ExitCode){
		case EXIT_CODE_WATCHDOG:
		case EXIT_CODE_EXHAUSTED:
		case EXIT_CODE_RECONFIG:
		case EXIT_CODE_ERROR_CONSOLE:
			if (Timeout > 0) {
				time_t TimeNow;
				if (time(&TimeNow) != (time_t) -1 && TimeFinish <= TimeNow) ExitCode = EXIT_CODE_TIMEOUT;
			}
			break;
		}
		switch (ExitCode){
		case EXIT_CODE_WATCHDOG:
			if (Count <= PrevWatchdogCount){
				TimeCooling += TIME_COOLING;
				if (TimeCooling > TIME_COOLING_MAX)
					TimeCooling = TIME_COOLING_MAX;
			} else
				PrevWatchdogCount = Count;
			lpApp->TimeLeft = time(NULL) + TimeCooling / 1000;
			lpApp->SetState(STATE_WATCHDOG, FALSE);
			Sleep((DWORD) TimeCooling);
			lpApp->ResetState(STATE_WATCHDOG, FALSE);
			break;
		case EXIT_CODE_KRANCH784:
			ChangeCount(Prev784Count);
			break;
		case EXIT_CODE_CRITICAL:
			lpApp->SetState(STATE_PAUSED, FALSE);
			break;
		}
		CloseHandle(ProcessInformation.hProcess);
	} else
		SET_ERROR;
	CloseHandle(hpFromChildOut);
	CloseHandle(hpChildOut);
	for (__int8 i = 0; i < MAX_GPU; i++) {
		lpApp->GPU[i].Temp.Current = 0;
		lpApp->GPU[i].Util.Current = 0;
		lpApp->GPU[i].Speed        = 0;
#if V0 >= 4
		lpApp->GPU[i].Fan.Current  = 0;
#endif
	}
}

CCALCULATOR::CCALCULATOR(TCHAR cKeyStatus, TCHAR cKeyAbort) {
	KeyStatus   = cKeyStatus;
	KeyAbort    = cKeyAbort;
	Enabled     = false;
	TimeCooling = TIME_COOLING;
}

void CCALCULATOR::ChangeCount(OP_COUNT OpCount){
	switch (OpId) {
	case SL3_ID:
		Count = OpCount;
		lpApp->mhWaited = SL32MH(Count);
		break;
	}
	lpApp->DlgInfo.ShowData();
}

void CCALCULATOR::CalcGlob(CALC_TEST_TYPE TestType) {
	ResultEnabled = false;
	ChangeCount(0);
	this->TestType = TestType;
	if (TestType != CALC_TEST_NULL)
		Enabled = true;
	if (Enabled) {
		// Запомнить время начала выполнения операции
		Enabled = time(&lpApp->Total.TimeBeginCalc) != (time_t) -1;
		if (! Enabled) 
			SET_ERROR_MSG("time() = -1");
		else {
			lpApp->DoScriptAltStop();
			for (__int8 i = 0; i < MAX_GPU; i++) {
				lpApp->GPU[i].Temp.Count   = 0;
				lpApp->GPU[i].Temp.Average = 0;
				lpApp->GPU[i].Temp.Max     = 0;
				lpApp->GPU[i].Util.Count   = 0;
				lpApp->GPU[i].Util.Average = 0;
				lpApp->GPU[i].Util.Max     = 0;
				lpApp->GPU[i].Speed        = 0;
#if V0 >= 4
				lpApp->GPU[i].Fan.Count    = 0;
				lpApp->GPU[i].Fan.Average  = 0;
				lpApp->GPU[i].Fan.Max      = 0;
#endif
			}
			Timeout = TestType != CALC_TEST_NULL? CALC_TEST_TIME: CALC_MAX_TIMEOUT;
			TimeFinish = lpApp->Total.TimeBeginCalc + Timeout;
			// Выполнить вычисление операции
			Enabled = CalcOperator();
			if (Enabled) {
				// Получить время завершения выполнения операции
				Enabled = time(&TimeFinish) != (time_t) -1;
				if (! Enabled)
					SET_ERROR_MSG("time() = -1");
				else {
					if (TimeFinish < lpApp->Total.TimeBeginCalc)
						SET_ERROR_MSG("TimeFinish < TimeBegin");
					else {
						// Длительности выполнения в секундах
						time_t Time = TimeFinish - lpApp->Total.TimeBeginCalc;
						// Длительность достаточная для вычисления скорости?
						if (TestType != CALC_TEST_SPEED && Time >= CALC_SPEED_TIME_MIN)
							// Скорость
							lpApp->Speed = (SPEED) (lpApp->mhWaited / Time);
					}
					switch(TestType) {
					case CALC_TEST_RESULT:
						if (! ResultEnabled) {
							Enabled = false;
							SET_ERROR_MSG("Calc Test Error");
						}
						break;
					case CALC_TEST_SPEED:
						if (lpApp->Speed < CALC_MIN_MHS) {
							Enabled = false;
							SET_ERROR_MSG("Speed Test Error");
						}
						break;
					}
				}
				lpApp->Total.mh += lpApp->mhWaited;
			}
			ChangeCount(0);
		}
	}
}

void CCALCULATOR::WriteCons(TCHAR Key) {
	if (lpApp->hConsoleInput == INVALID_HANDLE_VALUE)
		lpApp->InitConsole();
	INPUT_RECORD InputRecord;
	InputRecord.EventType = KEY_EVENT;
	InputRecord.Event.KeyEvent.bKeyDown = TRUE;
	InputRecord.Event.KeyEvent.dwControlKeyState = 0;
	InputRecord.Event.KeyEvent.uChar.UnicodeChar = Key;
	InputRecord.Event.KeyEvent.wRepeatCount = 1;
	InputRecord.Event.KeyEvent.wVirtualKeyCode = 0;
	InputRecord.Event.KeyEvent.wVirtualScanCode = 0;
	if (! WriteConsoleInput(lpApp->hConsoleInput, &InputRecord, 1, &size)) {
		err = GetLastError();
		SET_ERROR_CODE(err);
		if (err == ERROR_INVALID_HANDLE) {
			TerminateProcess(ProcessInformation.hProcess, HASHCATLITE_ERROR_EXHAUSTED);
			WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
			ExitCode = EXIT_CODE_ERROR_CONSOLE;
			lpApp->InitConsole();
		}
	}
}
