#pragma once
#include <tchar.h>
#include "CALCULATOR.h"
#include "sha1.h"

#ifdef _WIN64
#define APPNAME_HASHCATLITE_CUDA "chl64.exe"
#define APPNAME_HASHCATLITE_OCL  "ohl64.exe"
#else
#define APPNAME_HASHCATLITE_CUDA "chl.exe"
#define APPNAME_HASHCATLITE_OCL  "ohl.exe"
#endif


#define HASHCATLITE_KEY_STATUS 's' // строка передаваемая на консоль вычислителю для получения от него состояния
#define HASHCATLITE_KEY_ABORT  'q' // строка передаваемая на консоль вычислителю для прекращения работы
#define HASHCATLITE_CUDA_ID 0      // Идентификатор вычислителя CUDA
#define HASHCATLITE_OCL_ID  1      // Идентификатор вычислителя OCL

#define HASHCATLITE_FILE_OUT "o.tmp" // имя выходного файла
#define HASHCATLITE_FILE_SESSION "s" // имя файла для хранения сессии расчета
#define HASHCATLITE_EXT_SESSION ".restore"
#define HASHCATLITE_RESTORE_TIMER 5      // период (сек.) записи информации для последующего продолжения расчета в случае аварийного завершения
#define HASHCAT_MASK_CHAR_LEN 2
#define HASHCAT_HEX_CHAR_LEN  2


#define HASHCATLITE_ERROR_TEMPERATURE   ((DWORD) -2) // ERROR: Temperature limit on GPU 1 reached, aborting...
#define HASHCATLITE_ERROR_NOT_SUPPORTED ((DWORD) -1) // ERROR: restore file 's01.restore': No such file or directory // ERROR: No AMD compatible platform found

#if V0 >= 5
/*
oclHashcat-lite v0.13 by atom starting...

Password lengths: 15 - 15
Watchdog: Temperature abort trigger set to 90c
Watchdog: Temperature retain trigger set to 80c
Device #1: Turks, 2048MB, 725Mhz, 6MCU

[s]tatus [p]ause [r]esume [q]uit => 
Session.Name...: C:\Users\werty\AppData\Local\Temp\bv7\DCClient5\s
Status.........: Cracked
Hash.Target....: df34b47992aedd16c8d69584b6ec47589ad1ce6f:009999999999999900
Hash.Type......: SL3
Time.Started...: 0 secs
Time.Estimated.: 0 secs
Plain.Mask.....: ?1?1?1?1?1?1?1?1?1?1?1?1?1?1?1
Plain.Text.....: ********0704000200000000000000
Plain.Length...: 15
Progress.......: 10232048/10250000 (99.82%)
Speed.GPU.#1...:   402.2k/s
HWMon.GPU.#1...:  0% Util, 56c Temp, 30% Fan

Started: Mon Jan 28 17:18:52 2013
Stopped: Mon Jan 28 17:18:53 2013
*/
#define HASHCATLITE_ERROR_CRACKED   0 // Status.......: Cracked
#define HASHCATLITE_ERROR_EXHAUSTED 1 // Status.......: Exhausted (nod find, terminate process)
#define HASHCATLITE_ERROR_ABORTED   2 // Status.......: Aborted (timeout, manual press "q")
#define HASHCATLITE_STR_PLTEXT      "Plain.Text.....: "
#define HASHCATLITE_STR_HWMON       "HWMon.GPU.#1...:"
#define HASHCATLITE_IDX_HWMON       11
#define HASHCATLITE_STR_SPEED       "Speed.GPU.#*...:"
#define HASHCATLITE_IDX_SPEED       11
#define HASHCATLITE_LEN_SPEED       8
#define HASHCATLITE_STR_EXHAUSTED   "Status.........: Exhausted"
#else
#define HASHCATLITE_ERROR_EXHAUSTED 0 // Status.......: Aborted (timeout, manual press "q") // Status.......: Exhausted (nod find)
#define HASHCATLITE_ERROR_CRACKED   1 // Status.......: Cracked
#define HASHCATLITE_STR_PLTEXT      "Plain.Text...: "
#define HASHCATLITE_STR_EXHAUSTED "Status.......: Exhausted"
#if V0 == 2
#define HASHCATLITE_IDX_HWMON     12
#define HASHCATLITE_STR_SPEED     "Speed........:"
#define HASHCATLITE_LEN_SPEED      7
#define HASHCATLITE_STR_ABORTED   "Status.......: Aborted"
#else
#define HASHCATLITE_STR_HWMON     "HWMon.GPU.#1.:"
#define HASHCATLITE_IDX_HWMON     11
#define HASHCATLITE_STR_SPEED     "Speed.GPU.#*.:"
#define HASHCATLITE_IDX_SPEED     11
#define HASHCATLITE_LEN_SPEED      8
#endif
#endif

#define SL3_PW_LEN   15 // Длина пароля
#define SL3_RT_LEN    2 // Длина предельного времени
#define SL3_HASH_LEN 59 // Длина ХЕШ

#if V0 >= 4
#define HASHCAT_MARKOV_HCSTAT_PAR_NAME "hashcat.hcstat"
#define HASHCAT_FILE_MARKOV_HCSTAT     "h.hs"
#define HASHCAT_STR_UTIL               "Util,"
#define HASHCAT_STR_TEMP               "Temp,"
#else
#define HASHCAT_STR_UTIL               "GPU,"
//#define HASHCAT_WATCHDOG_PAR_NAME _T("--gpu-watchdog=")
#endif
#if V0 == 2
#define HASHCATLITE_STR_HWMON "HW.Monitor.#1:"
typedef TCHAR SL3_MASK[SL3_PW_LEN * HASHCAT_MASK_CHAR_LEN + 1];
#endif


const struct {
	char * Old;
	char * New;
} HCLReplStr[] = {
	{"oclHashcat-lite", APPNAME_HASHCATLITE_OCL}
	, {"cudaHashcat-lite", APPNAME_HASHCATLITE_CUDA}
#if V0 >= 4
	, {HASHCAT_MARKOV_HCSTAT_PAR_NAME, HASHCAT_FILE_MARKOV_HCSTAT}
	, {"hccap", "apcap"}
#endif
	, {"hashcat", "app-ion"}
	, {"atom", "DCC"}
	, {
#if V0 >= 5
		"0.13"
#elif V0 >= 4
		"0.10"
#elif V0 >= 3
		"0.09"
#elif V0 >= 2
		"0.06"
#endif
			, CH(V0) CH(V1) CH(V2)
	}
};


// Вычислитель HASHCAT-LITE
class HCL :public CCALCULATOR
{
public:
	FILE_NAME FileOutput;
	FILE_NAME SessionFile;
	SL3_PW    SubPwFirst;
	FILE_NAME StatParsPrgPath;
	TCHAR     StatPars1[MAX_PATH * 4];
	SHA1 sha1;
	HCL():CCALCULATOR(_T(HASHCATLITE_KEY_STATUS), _T(HASHCATLITE_KEY_ABORT)){};
	virtual void ChangeCommandPars(/*bool Restore = false*/); // Вызывается в прцессе вычисления, если была изменены параметры, для изменения параметра командной строки CalcData.CommandPars
	virtual void Test();
	virtual bool CalcOperator();
	bool CalcLS7();
	virtual void ScanStatus(bool ProcessClosed);             // Поиск в буффере информации о сотоянии процесса вычислителя (буффер - результат стандартного вывода вычислителя)
	virtual void ConvertExitCode();        // Пребразование кода возврата вычислителя в CALC_EXIT_CODE
	void CalcPar(OP_STR_PAR &Op);
	void Init();
};

typedef  HCL * LPHASHCATLITE;
