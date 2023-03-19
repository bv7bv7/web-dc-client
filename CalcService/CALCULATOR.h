#pragma once
#include "..\global\mystd.h"

#define MAX_CALCULATORS 2
#define CALC_TIME_STATE       5000 // Период опроса состояния калькулятора, мсек.
#define CALC_MAX_BUFF_OUT     4096 // Максимальный размер буфера консоли вычислителя
#define CALC_PRICE_DIM        1000 // Количество микроопераций, за которые указывается цена
#define CALC_SPEED_TIME_MIN (CALC_TEST_TIME / 2) // Минимальное время выполнения операции для регистрации скорости выполнения операции
#define CALC_MAX_TIMEOUT         (10*60*3/2) // Максимальное время вычисления порции, полученной от сервера
#define CALC_TEST_TIME        10 // Время тестирования в секундах
#define CALC_MIN_MHS           1 // Минимально допустимая скорость вычислителя (MH/s)
#define SL3_PSW_LEN           15
#define SL3_HASH_LEN          59
#if V2 >= 29
#define SL3_HASH1_LEN         40
#define SL3_SALT_LEN          18
#endif
#define SL3_RESULT_STR_MAX    (SL3_PSW_LEN + 1)
#define SL3_PER_MH            5270000
#define SL3_TEST_RES_HASH     "DF34B47992AEDD16C8D69584B6EC47589AD1CE6F:009999999999999900"
#define SL3_TEST_RES_PW_FIRST 10239990
#define SL3_TEST_RES_PW_LAST  10240010
#define SL3_TEST_RES_RES      10240000
#define SL3_TEST_SPD_HASH     "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:999999999999999999"
#define SL3_TEST_SPD_PW_FIRST 0
#define SL3_TEST_SPD_PW_LAST  999999999999999
#define SL3_SEP_PAR           "-"
#define SL3_PRICE_DIM         1000000000000000
#define SL32MH(op)            ((op) / SL3_PER_MH)
#define SL32GH(op)            ((op) / (SL3_PER_MH * 1000))
#define MH2$(mh,priceGHSDay)  ((double) (mh)*(priceGHSDay)/1000/24/60/60)
// v------------------ 15 Byte -----------------------v
// '0".substr(chunk_split($_POST['res'],1,'0'),0,-1)."' AS `msc`
//            v------------------- 31 Byte -------------------v v--- 20 Byte --v
// UNHEX(SHA1(x'".$sj['msc']."00".substr($sj['pi'],0,14)."00'))=x'".$sj['ph']."'

#define MAX_OPERATOR_PAR   128

#define OP_SL3_ID 0

enum OP_ID {
	SL3_ID = OP_SL3_ID
};

struct OP_STR_PAR {
	OP_ID Id;
	TCHAR Par[MAX_OPERATOR_PAR];
};

class SERVICE_CALC_CLASS;

typedef unsigned __int64 SL3_PW;
typedef struct SL3_PWS {
	SL3_PW First;
	SL3_PW Last;
} * LPSL3_PWS;
typedef TCHAR SL3_HASH[SL3_HASH_LEN + 1];
typedef SL3_HASH * LPSL3_HASH;
typedef TCHAR  SL3_RESULT_STR[SL3_RESULT_STR_MAX];
typedef struct {
	SL3_PWS  Pws;
	SL3_HASH Hash;
} SL3_PARAMS;

typedef struct {
	SL3_PW         Result;
	SL3_RESULT_STR ResultStr;      // Строка с результаттом
} SL3_DATA;



typedef unsigned __int64 OP_COUNT;
typedef struct OP_PARAMS {
	union {
		SL3_PARAMS sl3;
	};
} * LPPARAMS;

typedef struct {
	union {
		SL3_DATA sl3;
	};
} OP_DATA;

typedef unsigned __int32 SPEED; // MH/s

enum CALC_TEST_TYPE {
	CALC_TEST_NULL,
	CALC_TEST_RESULT,
	CALC_TEST_SPEED
};

typedef __int32 CALC_TIMEOUT;

typedef enum {
	EXIT_CODE_EMPTY,
	EXIT_CODE_WATCHDOG,
	EXIT_CODE_CRACKED,
	EXIT_CODE_EXHAUSTED,
	EXIT_CODE_CRITICAL,
	EXIT_CODE_ABORT,
	EXIT_CODE_TIMEOUT,
	EXIT_CODE_KRANCH784
	, EXIT_CODE_RECONFIG
	, EXIT_CODE_ERROR_CONSOLE
} CALC_EXIT_CODES;

class CCALCULATOR
{
public:
	bool Enabled;
	bool IsCalc; // Калькулятор считал
	TCHAR tApplicationName[MAX_PATH];
	SERVICE_CALC_CLASS * lpApp;
	OP_ID                OpId;
	OP_PARAMS            OpParams;
	OP_DATA              OpData;
	OP_COUNT             Count; // Изменять только ч-з ChangeCount();
	CALC_TIMEOUT         Timeout;
	CALC_EXIT_CODES      ExitCode;
	time_t               TimeFinish;
	bool                 ResultEnabled;
	TCHAR                CommandPars[MAX_PATH * 5];
	char                 CommandOut[CALC_MAX_BUFF_OUT];
	DWORD                CommandExitCode;
	CALC_TEST_TYPE       TestType;
	CCALCULATOR(TCHAR cKeyStatus, TCHAR cKeyAbort);
	virtual void ChangeCommandPars(/*bool Restore = false*/){}; // Вызывается в прцессе вычисления, если была изменены параметры, для изменения параметра командной строки CalcData.CommandPars
	virtual void ScanStatus(bool ProcessClosed){}; // Поиск в буффере информации о сотоянии процесса вычислителя (буффер - результат стандартного вывода вычислителя)
	virtual void ConvertExitCode(){}; // Пребразование кода возврата вычислителя в CALC_EXIT_CODE
	virtual void Test() abstract;
	virtual bool CalcOperator() {return false;}; // Должен выполнить вычисление, и вернуть Result {Enabled, Count, Str}
	void ChangeCount(OP_COUNT OpCount);
	void Process();
	void CalcGlob(CALC_TEST_TYPE Test = CALC_TEST_NULL);
	TCHAR KeyStatus;
	TCHAR KeyAbort;
	OP_COUNT PrevWatchdogCount;
	DWORD TimeCooling;
	bool IsExhausted;
	DWORD               err;
	DWORD               size;
	PROCESS_INFORMATION ProcessInformation;
	void WriteCons(TCHAR Key);
};

