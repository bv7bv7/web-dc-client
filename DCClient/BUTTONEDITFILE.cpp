// x.0.25
#include "..\global\mystd.h"
#include "BUTTONEDITFILE.h"

BUTTONEDITFILE::BUTTONEDITFILE(int IdcEdt) {
	this->IdcEdt = IdcEdt;
}


void BUTTONEDITFILE::Init(LPFILE_NAME lpFileName){
	this->lpFileName = lpFileName;
}

bool BUTTONEDITFILE::Command(UINT Id) {
	bool Ok;
	if (Id == IdcEdt) {
		NotepadEdit(* lpFileName);
		Ok = true;
	} else
		Ok = false;
	return Ok;
}
