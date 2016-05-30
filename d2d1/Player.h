#pragma once

void XTrace0(LPCTSTR lpszText)
{
	::OutputDebugString(lpszText);
}

void XTrace(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	TCHAR szBuffer[512]; // get rid of this hard-coded buffer
	nBuf = _vsnprintf_s(szBuffer, 511, lpszFormat, args);
	::OutputDebugString(szBuffer);
	va_end(args);
}

struct MyPlayer_t
{
	DWORD_PTR SomethingToPlayer1;
	DWORD_PTR SomethingToPlayer2;
	DWORD_PTR Player;
	Vector3 Position;
	int Health;
	float ViewAngle;

	void ReadInformation()
	{
		//DWORD_PTR MainPtr = client->GetImage();
		//DWORD_PTR MainPtrSize = client->GetSize();
		//SomethingToPlayer1 = process->Read<DWORD>(MainPtr + 0x0132064C);
		//SomethingToPlayer2 = process->Read<DWORD>(SomethingToPlayer1 + 0x2b0);
		//Position = process->Read<Vector3>(SomethingToPlayer2 + 0xb8);
		Player = process->Read<DWORD>(0x50F4F4);
		Position = process->Read<Vector3>(Player + 0x34);
		Health = process->Read<int>(Player + 0xF8);
	}
}MyPlayer;

//Enemy struct
struct PlayerList_t
{
	
	Vector3 HeadPosition, FootPosition;
	float viewMatrix[16];
	int Health = 0;
	wchar_t Name[64];
	DWORD_PTR LoopDistance = 0x04;

	void ReadInformation(int Player)
	{
		DWORD EntityLoop = process->Read<DWORD>(0x50F4F4 + 0x4);
		DWORD PlayerBase = process->Read<DWORD>(EntityLoop + (Player * LoopDistance));
		
		HeadPosition = process->Read<Vector3>(PlayerBase + 0x4);
		FootPosition = process->Read<Vector3>(PlayerBase + 0x34);
		Health = process->Read<int>(PlayerBase + 0xF8);

		ReadProcessMemory(process->m_hProcess, (LPCVOID)(PlayerBase + 0x225), &Name, sizeof(Name), NULL);
		ReadProcessMemory(process->m_hProcess, (LPCVOID)0x501AE8, &viewMatrix, sizeof(viewMatrix), NULL);
	}

	bool isAlive() {
		if (Health > 0) {
			return true;
		}
		else {
			return false;
		}
	}
}PlayerList[16];
