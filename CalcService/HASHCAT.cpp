#pragma once
#include <time.h>
#include <Shlwapi.h>
#include "HASHCAT.h"
#include "SERVICE_CALC_CLASS.h"

void HCL::Init() {
	wcscpy_s(FileOutput, ARRAY_LEN(FileOutput), lpApp->TempPath);
	if (! PathAppend(FileOutput, _T(HASHCATLITE_FILE_OUT)))
		SET_ERROR_MSG("PathAppend Error");
	else {
		_tcscpy_s(SessionFile, lpApp->TempPath);
		if (! PathAppend(SessionFile, _T(HASHCATLITE_FILE_SESSION)))
			SET_ERROR_MSG("PathAppend Error");
		else {
			_tcscpy_s(StatParsPrgPath, _T("\""));
#if V0 >= 4
			_tcscat_s(StatParsPrgPath, lpApp->CommonAppDataFolder);
#else
			_tcscat_s(StatParsPrgPath, lpApp->PrgPath);
#endif
			_tcscat_s(StatParsPrgPath, _T("\\"));
			_tcscat_s(StatPars1, _T("\" -m 1900 -o \""));
			_tcscat_s(StatPars1, FileOutput);
			_tcscat_s(StatPars1, _T("\" --hex-salt --hex-charset --pw-min=") _TCH(SL3_PW_LEN) _T(" --pw-max=") _TCH(SL3_PW_LEN) _T(" --outfile-format=1 ")
#if V0 >= 4
				_T("--markov-disable --markov-hcstat=\""));
			_tcscat_s(StatPars1, lpApp->PrgPath);
			_tcscat_s(StatPars1, _T("\\") _T(HASHCAT_FILE_MARKOV_HCSTAT) _T("\" ")
#endif
				_T("--restore-timer=0 --session=\""));
			_tcscat_s(StatPars1, SessionFile);
			_tcscat_s(StatPars1, _T("\" -1 00010203040506070809 ")
#if V0 >= 4
				_T("--gpu-temp-abort=")
#else
				_T("--gpu-watchdog=")
#endif
				);
			_tcscat_s(SessionFile, _T(HASHCATLITE_EXT_SESSION));
		}
	}
};

bool HCL::CalcLS7(){
	HANDLE hFile;
	SubPwFirst = OpParams.sl3.Pws.First;
	bool Ok = true;
	while(SubPwFirst <= OpParams.sl3.Pws.Last){
		if (! DeleteFile(FileOutput) && GetLastError() != ERROR_FILE_NOT_FOUND) {
			SET_ERROR;
			lpApp->SetState(STATE_ERROR_TMP, FALSE);
			return false;
		}
		Process();
		bool Break = false;
		SL3_PW PrevSubPwFirst = SubPwFirst;
		switch(ExitCode) {
		case EXIT_CODE_EXHAUSTED:
			if (! IsExhausted) {
				SET_ERROR_MSG("ERROR: Unexpected Shutdown Process.");
				SubPwFirst = OpParams.sl3.Pws.First;
				if (Count)
					SubPwFirst += Count -1;
				continue;
			}
			SubPwFirst = OpParams.sl3.Pws.Last + 1;
			ChangeCount(SubPwFirst - OpParams.sl3.Pws.First);
			break;
		case EXIT_CODE_KRANCH784:
		case EXIT_CODE_RECONFIG:
		case EXIT_CODE_WATCHDOG:
		case EXIT_CODE_ERROR_CONSOLE:
			SubPwFirst = OpParams.sl3.Pws.First;
			if (Count)
				SubPwFirst += Count;
			break;
		case EXIT_CODE_CRACKED:
			char StrRes[SL3_PSW_LEN * HASHCAT_HEX_CHAR_LEN +1];
			hFile = CreateFile(FileOutput, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			Ok = false;
			if (hFile == INVALID_HANDLE_VALUE)
				SET_ERROR;
			else {
				DWORD dw;
				char tmp[SL3_HASH_LEN + 1];
				char tmp2[SL3_HASH_LEN + 1];
				if(ASTR_S(tmp2, _countof(tmp2), OpParams.sl3.Hash) != SL3_HASH_LEN + 1)
					SET_ERROR_MSG("ERROR: Wrong format of result");
				else if(! ReadFile(hFile, tmp, SL3_HASH_LEN, &dw, NULL))
					SET_ERROR;
				else {
					tmp[SL3_HASH_LEN] = '\0';
					if(_stricmp(tmp, tmp2) != 0)
						SET_ERROR_MSG("ERROR: Wrong format of result");
					else if (SetFilePointer(hFile, SL3_HASH_LEN + 1, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER || ! ReadFile(hFile, StrRes, sizeof(StrRes), &dw, NULL))
						SET_ERROR;
					else {
						char * pStrRes = StrRes + 1;
						char re;
						SL3_PW res = 0;
						SL3_PW i10 = 1;
						sha1.Reset();
						for(unsigned __int8 i = 0; i < SL3_PSW_LEN; i++, i10 *= 10, pStrRes +=2)
						{
							re = * pStrRes - '0';
							res += re * i10;
							OpData.sl3.ResultStr[i] = _T('0') + re;
							sha1.Input(re);
						}
						for(unsigned __int8 i = SL3_HASH1_LEN + 1; i < SL3_HASH_LEN; i +=2)
							sha1.Input((char) (((tmp[i] - '0') << 4) + tmp[i + 1] - '0'));
						if(!sha1.Result())
							SET_ERROR_MSG("ERROR: Check result failed");
						else {
							Ok = true;
							unsigned h = 0;
							for(unsigned __int8 i = 0, j = 0; Ok && i < 5; i++){
								for(unsigned __int8 k = 0; k < 8; j++, k++){
									h <<= 4;
									h |= AHEX(tmp[j]);
								}
								Ok = sha1.H[i] == h;
							}
							if(!Ok)
								SET_ERROR_MSG("ERROR: Wrong result");
							else{
								OpParams.sl3.Pws.Last = res;
								Break = TRUE;
								OpData.sl3.ResultStr[SL3_PSW_LEN] = _T('\0');
								ResultEnabled = true;
							}
						}
					}
				}
				if (! CloseHandle(hFile))
					SET_ERROR;
				if (DeleteFile(FileOutput) == 0) {
					SET_ERROR;
				}
			}
			if (! Ok) {
				SET_ERROR_MSG("ERROR: Unexpected Shutdown Process.");
				SubPwFirst = OpParams.sl3.Pws.First;
				if (Count)
					SubPwFirst += Count -1;
				continue;
			}
			break;
		default:
			Break = TRUE;
			break;
		}
		if (Break || (PrevSubPwFirst == SubPwFirst && (TestType == CALC_TEST_NULL || ExitCode != EXIT_CODE_ERROR_CONSOLE)))
			break;
	}
	switch(ExitCode) {
	case EXIT_CODE_CRACKED:
	case EXIT_CODE_EXHAUSTED:
		break;
	case EXIT_CODE_ABORT:
	case EXIT_CODE_TIMEOUT:
	case EXIT_CODE_KRANCH784:
		OpParams.sl3.Pws.Last = OpParams.sl3.Pws.First + Count - 1;
		break;
	case EXIT_CODE_CRITICAL:
		if (strstr(CommandOut, HASHCATLITE_FILE_SESSION HASHCATLITE_EXT_SESSION ": Permission denied"))
			lpApp->SetState(STATE_ERROR_TMP, FALSE);
		else
		{
			lpApp->SetState(STATE_ERROR_CALC | STATE_PAUSED, FALSE);
			SET_ERROR_MSG("Critical Error of GPU");
		}
		Ok = false;
		break;
	default:
		Ok = false;
	}
	if (! DeleteFile(SessionFile) && GetLastError() != ERROR_FILE_NOT_FOUND) {
		SET_ERROR;
		lpApp->SetState(STATE_ERROR_TMP, FALSE);
		Ok = false;
	}
	return Ok;
}

bool HCL::CalcOperator(){
	switch(OpId){
	case SL3_ID:
		return CalcLS7();
	// Заполнить для новых операторов вычисляемых HASHCAT-LITE
	//case ..._ID:
	//	break;
	//...
	default:
		return CCALCULATOR::CalcOperator();
	}
}

// Поиск в буффере информации о сотоянии процесса вычислителя (буффер - результат стандартного вывода вычислителя)
void HCL::ScanStatus(bool ProcessClosed) {
	union {
		DWORD t;
		unsigned __int64 tl;
	};
	char * pCh;
	char Tmp[] = HASHCATLITE_STR_HWMON;
	for (__int8 i = 0; i < MAX_GPU; i++) {
		Tmp[HASHCATLITE_IDX_HWMON] = '1' + i;
		pCh = StrStrR(CommandOut, Tmp);
		if (pCh) {
			lpApp->GPU[i].Util.Current = atoi(pCh + ARRAY_LEN(HASHCATLITE_STR_HWMON) - 1);
			lpApp->GPU[i].Util.Average = (((unsigned __int32) lpApp->GPU[i].Util.Average) * lpApp->GPU[i].Util.Count + lpApp->GPU[i].Util.Current) / (lpApp->GPU[i].Util.Count + 1);
			if (lpApp->GPU[i].Util.Current > lpApp->GPU[i].Util.Max)
				lpApp->GPU[i].Util.Max = lpApp->GPU[i].Util.Current;
			lpApp->GPU[i].Temp.Count++;
			pCh = strstr(pCh, HASHCAT_STR_UTIL);
			if (pCh) {
				t = atoi(pCh + ARRAY_LEN(HASHCAT_STR_UTIL) - 1);
				lpApp->GPU[i].Temp.Current = t;
				lpApp->GPU[i].Temp.Average = (lpApp->GPU[i].Temp.Average * lpApp->GPU[i].Temp.Count + t) / (lpApp->GPU[i].Temp.Count + 1);
				if (t > lpApp->GPU[i].Temp.Max)
					lpApp->GPU[i].Temp.Max = t;
				lpApp->GPU[i].Temp.Count++;
#if V0 >= 4
				pCh = strstr(pCh, HASHCAT_STR_TEMP);
				if (pCh) {
					lpApp->GPU[i].Fan.Current = atoi(pCh + ARRAY_LEN(HASHCAT_STR_TEMP) - 1);
					lpApp->GPU[i].Fan.Average = (((unsigned __int32) lpApp->GPU[i].Fan.Average) * lpApp->GPU[i].Fan.Count + lpApp->GPU[i].Fan.Current) / (lpApp->GPU[i].Fan.Count + 1);
					if (lpApp->GPU[i].Fan.Current > lpApp->GPU[i].Fan.Max)
						lpApp->GPU[i].Fan.Max = lpApp->GPU[i].Fan.Current;
					lpApp->GPU[i].Fan.Count++;
				}
#endif
			}
		}
	}
	pCh = StrStrR(CommandOut, HASHCATLITE_STR_PLTEXT);
#if V0 >= 3
	SPEED SumSpd = 0;
	strcpy_s(Tmp, HASHCATLITE_STR_SPEED);
	for (__int8 i = 0; i < MAX_GPU; i++) {
		Tmp[HASHCATLITE_IDX_SPEED] = '1' + i;
		pCh = StrStrR(CommandOut, Tmp);
		if (pCh) {
			tl = atoi(pCh + _countof(HASHCATLITE_STR_SPEED) - 1);
			switch(pCh[_countof(HASHCATLITE_STR_SPEED) - 1 + 8]) {
			case 'M':
				tl *= 1000000;
				break;
			case 'k':
				tl *= 1000;
				break;
			}
			lpApp->GPU[i].Speed = (DWORD) SL32MH(tl);
		}
		SumSpd += lpApp->GPU[i].Speed;
	}
#endif
	pCh = StrStrR(CommandOut, HASHCATLITE_STR_SPEED);
	if (pCh) {
		tl = atoi(pCh + _countof(HASHCATLITE_STR_SPEED) - 1);
		switch(pCh[_countof(HASHCATLITE_STR_SPEED) - 1 + HASHCATLITE_LEN_SPEED]) {
		case 'M':
			tl *= 1000000;
			break;
		case 'k':
			tl *= 1000;
			break;
		}
		lpApp->Speed = (DWORD) SL32MH(tl);
	}
#if V0 >= 3
	else
		lpApp->Speed = SumSpd;
#endif
	pCh = StrStrR(CommandOut, HASHCATLITE_STR_PLTEXT);
	if (pCh) {
		pCh = pCh + _countof(HASHCATLITE_STR_PLTEXT) - 1;
		if (OpId == SL3_ID){
			pCh += HASHCAT_HEX_CHAR_LEN - 1;
			unsigned __int64 Pw = 0;
			char Ch;
			for(unsigned __int64 i = 1; ; i *=10, pCh += HASHCAT_HEX_CHAR_LEN) {
				Ch = *pCh;
				if (Ch != '*') {
					if (Ch >= '0' && Ch <= '9') Pw += (Ch - '0') * i;
					else break;
				}
			}
			if (Pw >= OpParams.sl3.Pws.First)
				ChangeCount(Pw - OpParams.sl3.Pws.First + 1);
			IsCalc = true;
		}
	}
	if (! IsExhausted) {
		if (strstr(CommandOut, HASHCATLITE_STR_EXHAUSTED))
			IsExhausted = true;
#if V0 == 2
		else if(strstr(CommandOut, HASHCATLITE_STR_ABORTED))
			IsExhausted = true;
#endif
	}
	pCh = strstr(CommandOut, "ERROR:");
	if (!pCh)
		pCh = strstr(CommandOut, "WARN:");
	if (pCh) {
		char * pStrErrEnd = strchr(pCh, '\n');
		if (pStrErrEnd)
			* pStrErrEnd = '\0';
	} else
		if (ProcessClosed) {
			switch(CommandExitCode) {
			case HASHCATLITE_ERROR_TEMPERATURE:
			case HASHCATLITE_ERROR_EXHAUSTED:
			case HASHCATLITE_ERROR_CRACKED:
#if V0 >= 5
			case HASHCATLITE_ERROR_ABORTED:
#endif
				break;
			default:
				pCh = strrchr(CommandOut, '\n');
				if (pCh) {
					* pCh = '\0';
					pCh = strrchr(CommandOut, '\n');
				}
			}
		}
	if (pCh) {
		char * pOld;
		size_t lo, ln;
		for (unsigned __int8 i = 0; i < ARRAY_LEN(HCLReplStr); i++) {
			while (pOld = strstr(pCh, HCLReplStr[i].Old)) {
				lo = strlen(HCLReplStr[i].Old);
				ln = strlen(HCLReplStr[i].New);
				strncpy_s(pOld, lo + 1, HCLReplStr[i].New, min(lo, ln));
				if (ln < lo)
					_strnset_s(pOld + ln, lo - ln + 1, ' ', lo - ln);
			}

		}
		SET_ERROR_MSG(pCh);
	}
}

/*
v.0-4
-2:
	ERROR: Temperature limit on GPU 1 reached, aborting...
-1:
	ERROR: restore file 's01.restore': No such file or directory
	ERROR: No AMD compatible platform found
0:
	Status.......: Aborted (timeout, manual press "q")
	Status.......: Exhausted (nod find)
	The EULA has changed, need to restart oclHashcat-lite
1:
	Status.......: Cracked
v.5
status codes on exit:
=====================

-2 = gpu-watchdog alarm
-1 = error
 0 = cracked
 1 = exhausted
 2 = aborted
*/
// Пребразование кода возврата вычислителя в CALC_EXIT_CODE
void HCL::ConvertExitCode(){
	switch(CommandExitCode){
	case HASHCATLITE_ERROR_TEMPERATURE:
		ExitCode = EXIT_CODE_WATCHDOG;
		break;
	case HASHCATLITE_ERROR_CRACKED:
		ExitCode = TestType != CALC_TEST_SPEED? EXIT_CODE_CRACKED: EXIT_CODE_CRITICAL;
		break;
	case HASHCATLITE_ERROR_EXHAUSTED:
#if V0 >= 5
	case HASHCATLITE_ERROR_ABORTED:
#endif
		ExitCode = (TestType != CALC_TEST_RESULT && IsCalc? EXIT_CODE_EXHAUSTED: EXIT_CODE_CRITICAL);
		break;
	default:
		ExitCode = EXIT_CODE_CRITICAL;
		break;
	}
}

void HCL::ChangeCommandPars() {
	time_t Time;
	TCHAR Tmp[SL3_PW_LEN + 1];
	ResetEvent(lpApp->EventReconfig);
	_tcscpy_s(CommandPars, StatParsPrgPath);
	_tcscat_s(CommandPars, lpApp->RegData25.GPU.Architecture == GPU_CUDA? _T(APPNAME_HASHCATLITE_CUDA): _T(APPNAME_HASHCATLITE_OCL));
	_tcscat_s(CommandPars, StatPars1);
	if (lpApp->PublicData.WatchdogUse) {
		_itot_s((int) lpApp->PublicData.Watchdog, Tmp, 10);
		_tcscat_s(CommandPars, Tmp);
	} else
		_tcscat_s(CommandPars, _T("0"));
#if V0 >= 4
	_tcscat_s(CommandPars, _T(" --gpu-temp-retain="));
	if (lpApp->RegData25.GPU.Temp.Retain.Use) {
		_itot_s((int) lpApp->RegData25.GPU.Temp.Retain.Value, Tmp, 10);
		_tcscat_s(CommandPars, Tmp);
	} else
		_tcscat_s(CommandPars, _T("0"));
	if (!lpApp->RegData25.GPU.Temp.Use)
		_tcscat_s(CommandPars, _T(" --gpu-temp-disable"));
#endif
	_tcscat_s(CommandPars, _T(" --runtime="));
	if (time(&Time) > 0 && TimeFinish > Time) {
		_itot_s((int) (TimeFinish - Time), Tmp, 10);
		_tcscat_s(CommandPars, Tmp);
	} else
		_tcscat_s(CommandPars, _T("1"));
	if (SubPwFirst > 
#if V0 >= 3
		0
#else
		9999
#endif
		) {
		_tcscat_s(CommandPars, _T(" --pw-skip="));
		_ui64tot_s(SubPwFirst
#if V0 < 3
			/ 10000
#endif
			, Tmp, ARRAY_LEN(Tmp), 10);
		_tcscat_s(CommandPars, Tmp);
	}
	if (OpParams.sl3.Pws.Last < 999999999999999) {
		_tcscat_s(CommandPars, _T(" --pw-limit="));
		_ui64tot_s(OpParams.sl3.Pws.Last + 1, Tmp, ARRAY_LEN(Tmp), 10);
		_tcscat_s(CommandPars, Tmp);
	}
	BOOL UseAll = TRUE;
	BOOL UseAny = FALSE;
	unsigned __int8 i;
	bool b = false;
	for (i = 0; i < MAX_GPU; i++) {
		UseAll = UseAll && lpApp->PublicData.GPU[i].Use;
		UseAny = UseAny || lpApp->PublicData.GPU[i].Use;
	}
	if(! UseAll && UseAny) {
		Tmp[1] = _T('\0');
		_tcscat_s(CommandPars, _T(" -d "));
		for (i = 0, b = false; i < MAX_GPU; i++) 
			if (lpApp->PublicData.GPU[i].Use) {
				if (b)
					_tcscat_s(CommandPars, _T(","));
				Tmp[0] = _T('1') + i;
				_tcscat_s(CommandPars, Tmp);
				b = true;
			}
	}
	if (lpApp->PublicData.Workload.Use)
	{
		_tcscat_s(CommandPars, _T(" -n "));
		_itot_s((int) lpApp->PublicData.Workload.Value, Tmp, 10);
		_tcscat_s(CommandPars, Tmp);
	}
#if V0 >= 3
	if (lpApp->RegData25.CPUAffinityMask && lpApp->RegData25.CPUAffinityMask != lpApp->SystemAffinityMask) {
		DWORD_PTR dw = 1;
		_tcscat_s(CommandPars, _T(" --cpu-affinity="));
		for (i = 0, b = false; i < MAX_CPU; i++, dw <<= 1) {
			if (lpApp->RegData25.CPUAffinityMask & dw) {
				if (b)
					_tcscat_s(CommandPars, _T(","));
				_itot_s((int) i, Tmp, 10);
				_tcscat_s(CommandPars, Tmp);
				b = true;
			}
		}
	}
#endif
	_tcscat_s(CommandPars, _T(" "));
	_tcscat_s(CommandPars, OpParams.sl3.Hash);
	_tcscat_s(CommandPars, _T(" ?1?1?1?1?1?1?1?1?1?1?1?1?1?1?1"));
}

// Тестирование вычислителя и операции
void HCL::Test() {
	OpId = SL3_ID;
	_tcscpy_s(OpParams.sl3.Hash, _T(SL3_TEST_RES_HASH));
	OpParams.sl3.Pws.First = SL3_TEST_RES_PW_FIRST;
	OpParams.sl3.Pws.Last  = SL3_TEST_RES_PW_LAST;
	CalcGlob(CALC_TEST_RESULT);
	if (Enabled) {
		if (OpParams.sl3.Pws.Last == SL3_TEST_RES_RES) {
			_tcscpy_s(OpParams.sl3.Hash, _T(SL3_TEST_SPD_HASH));
			OpParams.sl3.Pws.First = SL3_TEST_SPD_PW_FIRST;
			OpParams.sl3.Pws.Last  = SL3_TEST_SPD_PW_LAST;
			CalcGlob(CALC_TEST_SPEED);
		} else Enabled = false;
	}
};

void HCL::CalcPar(OP_STR_PAR &Op) {
	OpId = Op.Id;
	switch (OpId) {
	case SL3_ID:
		Enabled = swscanf_s(Op.Par, _T("%[^") _T(SL3_SEP_PAR) _T("]") _T(SL3_SEP_PAR) _T("%I64u") _T(SL3_SEP_PAR) _T("%I64u"), OpParams.sl3.Hash, _countof(OpParams.sl3.Hash), &OpParams.sl3.Pws.First, &OpParams.sl3.Pws.Last) == 3;
		if (! Enabled)
			SET_ERROR_MSG("Invalid Parameters");
		else {
			CalcGlob(CALC_TEST_NULL);
			if (Enabled)
				_stprintf_s(Op.Par,	_T("%s") _T(SL3_SEP_PAR) _T("%0") _T(CH(SL3_PSW_LEN)) _T("I64u") _T(SL3_SEP_PAR) _T("%0") _T(CH(SL3_PSW_LEN)) _T("I64u"), OpParams.sl3.Hash, (unsigned __int64) OpParams.sl3.Pws.First, (unsigned __int64) OpParams.sl3.Pws.Last);
		}
		break;
	}
}
