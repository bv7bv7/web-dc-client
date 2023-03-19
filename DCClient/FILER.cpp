// 10021
#pragma once
#include <tchar.h>
#include "..\global\mystd.h"
#include <Shlwapi.h>
#include "FILER.h"

FILER::FILER(int IdcCheck, int IdcEdit, int IdcButton, int IdcEdt)
{
	this->IdcCheck  = IdcCheck;
	this->IdcEdit   = IdcEdit;
	this->IdcButton = IdcButton;
	this->IdcEdt    = IdcEdt;
}


void FILER::Init(HWND hWnd, LPFILER_DATA Data)
{
	this->hWnd = hWnd;
	this->lpData = Data;
	SetDlgItemText(hWnd, IdcEdit, Data->Name);
	CheckDlgButton(hWnd, IdcCheck, (UINT) Data->Enable);
	Check();
}

bool FILER::Command(UINT Id)
{
	bool Ok = true;
	if (Id == IdcCheck)
		Check();
	else if (Id == IdcButton)
		Button();
	else if (Id == IdcEdt)
		Edt();
	else
		Ok = false;
	return Ok;
}

void FILER::Check()
{
	BOOL Checked = IsDlgButtonChecked(hWnd, IdcCheck) == BST_CHECKED;
	EnableWindow(GetDlgItem(hWnd, IdcEdit), Checked);
	EnableWindow(GetDlgItem(hWnd, IdcButton), Checked);
	EditChange();
}

void FILER::Button()
{
	FILE_NAME Name;
	OPENFILENAME ofn;
	GetDlgItemText(hWnd, IdcEdit, Name, _countof(Name));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize    = sizeof ofn;
	ofn.hwndOwner      = hWnd;
	ofn.lpstrFile      = Name;
	ofn.nMaxFile       = _countof(Name);
	ofn.lpstrFilter    = _T("*.exe (Executable / Исполняемые)\0*.exe\0*.cmd (Command / Командные)\0*.cmd\0*.bat (Batch / Пакетные)\0*.bat\0*.* (All / Все)\0*.*\0\0");
	LPTSTR lpExt = PathFindExtension(Name);
	if (*lpExt != NULL)
		lpExt++;
	if(_tcsicmp(lpExt, _T("exe")) == 0 || Name[0] == _T('\0'))
		ofn.nFilterIndex = 1;
	else if(_tcsicmp(lpExt, _T("cmd")) == 0)
		ofn.nFilterIndex = 2;
	else if(_tcsicmp(lpExt, _T("bat")) == 0)
		ofn.nFilterIndex = 3;
	else
		ofn.nFilterIndex = 4;
	ofn.Flags          = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR ;
	if (GetOpenFileName(&ofn)) {
		SetDlgItemText(hWnd, IdcEdit, Name);
		EditChange();
	}
}

void FILER::Edt()
{
	FILE_NAME FileName;
	GetDlgItemText(hWnd, IdcEdit, FileName, _countof(FileName));
	NotepadEdit(FileName);
}

void FILER::Ok() {
	GetDlgItemText(hWnd, IdcEdit, lpData->Name, ARRAY_LEN(lpData->Name));
	lpData->Enable = IsDlgButtonChecked(hWnd, IdcCheck) == BST_CHECKED;
}

void FILER::EditChange()
{
	FILE_NAME Name;
	GetDlgItemText(hWnd, IdcEdit, Name, ARRAY_LEN(Name));
	LPTSTR s = _tcsrchr(Name, _T('.'));
	bool p = s != 0;
	s++;
	EnableWindow(GetDlgItem(hWnd, IdcEdt), IsDlgButtonChecked(hWnd, IdcCheck) == BST_CHECKED && p && (_tcsicmp(s, _T("cmd")) == 0 || _tcsicmp(s, _T("bat")) == 0));
}

