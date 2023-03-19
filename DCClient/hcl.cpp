/*
#include <Windows.h>

DWORD xxx()
{
	HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Event = WaitForSingleObject(hStdInput, 1000);
	switch(Event)
	{
	case WAIT_TIMEOUT:
		return 0;
	case WAIT_ABANDONED:
	case WAIT_FAILED:
		return -1;
	}
	INPUT_RECORD InputRecords[100];
	DWORD NumberOfEventsRead;
	BOOL Result = ReadConsoleInputA(hStdInput, InputRecords, _countof(InputRecords), &NumberOfEventsRead);
	if(NumberOfEventsRead == 0)
		return 0;
	for(DWORD i = 0; i < NumberOfEventsRead; i++)
		if(InputRecords[i].EventType == KEY_EVENT && InputRecords[i].Event.KeyEvent.bKeyDown)
			return  InputRecords[i].Event.KeyEvent.uChar.AsciiChar;
	return 0;
}
*/