// stdafx.h: ���������� ���� ��� ����������� ��������� ���������� ������
// ��� ���������� ������ ��� ����������� �������, ������� ����� ������������, ��
// �� ����� ����������
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ��������� ����� ������������ ���������� �� ���������� Windows
// ����� ���������� Windows:
#include <windows.h>



// TODO: ���������� ����� ������ �� �������������� ���������, ����������� ��� ���������
#pragma comment( lib, "msi.lib")
#pragma comment( lib, "Shlwapi.lib")

#include   <msi.h>

#include   <msiquery.h>

#include   <stdio.h>