#pragma once
#include "SERVER.h"
#include "..\CalcService\SERVICE_CALC_CLASS.h"

SERVERS::SERVERS(void)
{
	Reg.Ver   = 0;
	Cache.Ver = 0;
	for(unsigned __int8 i = 0; i < MAX_SERVER25; i++) {
		Reg.Items[i].Type            = SERVER_TYPE_NULL;
		Reg.Items[i].Name[0]         = _T('\0');
		Reg.Items[i].Url[0]          = _T('\0');
		Reg.Items[i].IDData.tID[0]   = _T('\0');
		Reg.Items[i].IDData.tPass[0] = _T('\0');
		Cache.Items[i].Type          = SERVER_TYPE_COUNT;
		Cache.Items[i].Name[0]       = _T('\0');
		Cache.Items[i].Url[0]        = _T('\0');
	}
	DWORD dw = 0;
	bool NoReg = myRegGetValue(_T("Servers25"), REG_BINARY, &Reg, sizeof(Reg), _T(CRYPT_SERVERS), &dw) && dw == 0;
	if (NoReg || Reg.Ver <= 0) {
		_tcscpy_s(Reg.Items[0].Name, _T("Server ¹ 2"));
		_tcscpy_s(Reg.Items[0].Url,  _T("http://calc.comli.com/"));
		Reg.Items[0].Type = SERVER_TYPE_RESERV;

		_tcscpy_s(Reg.Items[1].Name, _T("Server ¹ 3"));
		_tcscpy_s(Reg.Items[1].Url,  _T("http://dc.host56.com/"));
		Reg.Items[1].Type = SERVER_TYPE_RESERV;

		_tcscpy_s(Reg.Items[2].Name, _T("Server ¹ 4 (TEST)"));
		_tcscpy_s(Reg.Items[2].Url,  _T("http://dc.netne.net/"));
		Reg.Items[2].Type = SERVER_TYPE_TEST;

		_tcscpy_s(Reg.Items[3].Name, _T("Server ¹ 5 (TEST)"));
		_tcscpy_s(Reg.Items[3].Url,  _T("http://dc.webatu.com/"));
		Reg.Items[3].Type = SERVER_TYPE_TEST;

		_tcscpy_s(Reg.Items[4].Name, _T("Server ¹ 6"));
		_tcscpy_s(Reg.Items[4].Url,  _T("http://5.77.41.189/"));
		Reg.Items[4].Type = SERVER_TYPE_MAIN;

		_tcscpy_s(Reg.Items[5].Name, _T("Server ¹ 7"));
		_tcscpy_s(Reg.Items[5].Url,  _T("http://calc.hostoi.com/"));
		Reg.Items[5].Type = SERVER_TYPE_RESERV;

		_tcscpy_s(Reg.Items[6].Name, _T("Server ¹ 8"));
		_tcscpy_s(Reg.Items[6].Url,  _T("http://dc.netau.net/"));
		Reg.Items[6].Type = SERVER_TYPE_RESERV;

		if (NoReg) {
			SERVERS_DYNAMIC24 ServersDynamic24;
			ZeroMemory(&ServersDynamic24, sizeof(ServersDynamic24));
			if (myRegGetValue(_T("Servers"), REG_BINARY, &ServersDynamic24, sizeof(ServersDynamic24), _T(CRYPT_SERVERS), &dw) && dw) {
				for (unsigned __int8 i = 0; i < _countof(ServersDynamic24); i++)
					if (ServersDynamic24[i].Url[0]) {
						_tcscpy_s(Reg.Items[i].IDData.tID,   ServersDynamic24[i].IDData.tID);
						_tcscpy_s(Reg.Items[i].IDData.tPass, ServersDynamic24[i].IDData.tPass);
					}
			}
		}
	}
	for(unsigned __int8 i = 0; i < MAX_SERVER25; i++)
		Reg.Items[i].ChangeUser = true;
}

void SERVERS::Init() {
	Cache.Ver = 0;
	for(unsigned __int8 i = 0; i < _countof(Cache.Items); i++) {
		Cache.Items[i].Type    = SERVER_TYPE_COUNT;
		Cache.Items[i].Name[0] = _T('\0');
		Cache.Items[i].Url[0]  = _T('\0');
	}
	for (Current = 0; Current < _countof(Reg.Items) && Reg.Items[Current].Type != (lpApp->PublicData.Test? SERVER_TYPE_TEST: SERVER_TYPE_MAIN); Current++);
	if (Current >= _countof(Reg.Items))
		SET_ERROR_MSG("Error of Servers Data");
}

void SERVERS::CacheToReg() {
	if (Cache.Ver >= Reg.Ver) {
		for (unsigned __int8 i = 0; i < _countof(Reg.Items); i++) {
			if (Cache.Items[i].Type < SERVER_TYPE_COUNT)
				Reg.Items[i].Type = Cache.Items[i].Type;
			if (Cache.Items[i].Name[0])
				_tcscpy_s(Reg.Items[i].Name, Cache.Items[i].Name);
			if (Cache.Items[i].Url[0]) {
				_tcscpy_s(Reg.Items[i].Url, Cache.Items[i].Url);
				Reg.Items[i].ChangeUser = true;
			}
		}
		Reg.Ver = Cache.Ver;
		Init();
		myRegSetValue(_T("Servers25"), REG_BINARY, &Reg, sizeof(Reg), _T(CRYPT_SERVERS));
	}
}

bool SERVERS::Next() {
	bool NextWithoutPause = lpApp->PublicData.ChangeServers || (Reg.Items[Current].Type == SERVER_TYPE_TEST) != lpApp->PublicData.Test;
	if (NextWithoutPause) {
		do {
			Current++;
			if(Current >= _countof(Reg.Items))
				Current = 0;
			if(Reg.Items[Current].Type == SERVER_TYPE_MAIN) {
				if (! NextWithoutPause) {
					SET_ERROR_MSG("Error Of Servers Data");
					break;
				} else
					NextWithoutPause = false;
			}
		}
		while(lpApp->PublicData.Test? Reg.Items[Current].Type != SERVER_TYPE_TEST: Reg.Items[Current].Type != SERVER_TYPE_MAIN && Reg.Items[Current].Type != SERVER_TYPE_RESERV);
	}
	return NextWithoutPause;
}


