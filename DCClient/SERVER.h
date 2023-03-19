#pragma once
#include <Windows.h>
#include "..\global\global.h"

class SERVICE_CALC_CLASS;

struct SERVER_DYNAMIC24
{
	bool  Active;
	TCHAR Url[MAX_PATH];
	ID_DATA IDData;
	NOTUSE_WORK NOTUSE_WorkAccounted, NOTUSE_WorkWaited, NOTUSE_WorkPrev; // Для совместимости с предыдущими версиями ветви реестра "Servers"
	bool ChangeUser;
};

#define MAX_SERVER_NAME 64
#define MAX_SERVER25    16
#define MAX_SERVER24     7

enum SERVER_TYPE{
	SERVER_TYPE_NULL,
	SERVER_TYPE_MAIN,
	SERVER_TYPE_RESERV,
	SERVER_TYPE_TEST,
	SERVER_TYPE_COUNT
};

typedef TCHAR SERVER_NAME[MAX_SERVER_NAME];

struct SERVER_ITEM {
	SERVER_TYPE Type;
	SERVER_NAME Name;
	FILE_NAME   Url;
	ID_DATA     IDData;
	bool        ChangeUser;
};

struct SERVER_ITEM_CACHE {
	SERVER_TYPE Type;
	SERVER_NAME Name;
	FILE_NAME   Url;
};

typedef SERVER_ITEM       SERVER_ITEMS[MAX_SERVER25];
typedef SERVER_ITEM_CACHE SERVER_ITEMS_CACHE[MAX_SERVER25];
typedef SERVER_DYNAMIC24  SERVERS_DYNAMIC24[MAX_SERVER24];

struct SERVER_REG {
	DWORD Ver;
	SERVER_ITEMS Items;
};

struct SERVER_CACHE {
	DWORD              Ver;
	SERVER_ITEMS_CACHE Items;
};

class SERVERS
{
public:
	SERVERS(void);
	__int8 Current;
	SERVICE_CALC_CLASS * lpApp;
	SERVER_REG           Reg;
	SERVER_CACHE         Cache;
	bool Next();
	void Init();
	void CacheToReg();
};