// x.0.25
#pragma once
#include "..\global\mystd.h"

class BUTTONEDITFILE {
public:
	int          IdcEdt;
	LPFILE_NAME  lpFileName;
	BUTTONEDITFILE(int IdcEdt);
	void Init(LPFILE_NAME lpFileName);
	bool Command(UINT Id);
};

