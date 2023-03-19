// 10021
#pragma once

struct FILER_DATA {
	bool Enable;
	FILE_NAME Name;
};

typedef FILER_DATA * LPFILER_DATA;

class FILER {
public:
	int          IdcCheck;
	int          IdcEdit;
	int          IdcButton;
	int          IdcEdt;
	HWND         hWnd;
	LPFILER_DATA lpData;
	FILER(int IdcCheck, int IdcEdit, int IdcButton, int IdcEdt);
	void Init(HWND hWnd, LPFILER_DATA Data);
	void Check();
	void Button();
	void Edt();
	void Ok();
	bool Command(UINT Id);
	void EditChange();
};

