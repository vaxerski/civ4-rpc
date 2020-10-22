
#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <d3dx9.h>
#include <d3d9.h>
#include "detours.h"
#include <system_error>
#include "CvData.h"
#include <signal.h>
#include "structs.h"
#include <string>
#include "sigscan.h"
#include "CvString.h"
#include <filesystem>
#include <fstream>
#include "Discord.h"

#pragma comment(lib, "d3dx9.lib")

DWORD CvGameCoreDLL;
static bool bOpen;

#define jmp(frm, to) (int)(((int)to - (int)frm) - 5); 

bool BUGmod = true;

cSigscan* sigscan;

const char* windowName = "Civ IV: Beyond The Sword";

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

WNDPROC oWndProc;

//-----FUNCS------//
typedef int(WINAPI* basicIntVoid)();
basicIntVoid ogetGameTurn;
basicIntVoid ogetCalendar;
basicIntVoid ogetGameSpeed;
basicIntVoid ocountCivPlayersAlive;
basicIntVoid ocountCivPlayersEverAlive;

typedef int(WINAPI* setYearStr)(CvWString&, int, char, int, int, int);
setYearStr oYearStr;

int turn;
int calendar;
int gamespeed;
int alive;
int everalive;
std::string yearS;

bool DrawMessage(LPD3DXFONT font, unsigned int x, unsigned int y, int alpha, unsigned char r, unsigned char g, unsigned char b, LPCSTR Message)
{	// Create a colour for the text
	D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, r, g, b);
	RECT rct; //Font
	rct.left = x;
	rct.right = 1680;
	rct.top = y;
	rct.bottom = rct.top + 200;
	font->DrawTextA(NULL, Message, -1, &rct, 0, fontColor);
	return true;
}

int WINAPI Hooked_getGameTurn() {
	int temp = ogetGameTurn();
	if (temp == turn) return turn;
	turn = temp;

	CIV->getRpc()->turn = turn;
	CIV->updateRPC();
	return turn;
}

int WINAPI Hooked_getCalendar() {
	calendar = ogetCalendar();
	return calendar;
}

int WINAPI Hooked_getGameSpeed() {
	gamespeed = ogetGameSpeed();
	return gamespeed;
}

int WINAPI Hooked_countCivPlayersAlive() {

	int temp = ocountCivPlayersAlive();
	if (temp == alive) return temp;
	alive = temp;

	CIV->getRpc()->playersAlive = alive;
	CIV->updateRPC();
	return alive;
}

int WINAPI Hooked_countCivPlayersEverAlive() {
	int temp = ocountCivPlayersEverAlive();
	if (temp == everalive) return everalive;
	everalive = temp;

	CIV->getRpc()->playersTotal = everalive;
	CIV->updateRPC();
	return everalive;
}

int WINAPI Hooked_setYearStr(CvWString& wstr, int a2, char a3, int a4, int a5, int a6) {
	int result = oYearStr(wstr, a2, a3, a4, a5, a6);

	char yearC[7];
	for (int i = 0; i < 7; ++i) {
		yearC[i] = (char)wstr[i + 2];
		if ((char)wstr[i + 2] == (char)"D" || (char)wstr[i + 2] == (char)"C") break;
	}

	if (yearS == yearC) return result;

	yearS = yearC;

	CIV->getRpc()->igDate = yearS;
	CIV->updateRPC();

	return result;
}

Discord DISCORD;

DWORD WINAPI mainTh(LPVOID param) // Our main thread
{
	DISCORD.Initialise("518379213645021185");

	sigscan = new cSigscan;

	HWND  window = FindWindowA(NULL, windowName);

	CvGameCoreDLL = (UINT_PTR)GetModuleHandle("CvGameCoreDLL.dll");

	DWORD getgamet = sigscan->FindPattern((char*)"CvGameCoreDLL.dll", (char*)"\xb9\xff\xff\xff\xff\xe8\xff\xff\xff\xff\x8b\x80\xbc\x00\x00\x00", (char*)"x????x????xxxxxx");
	//DWORD getcalendar = sigscan->FindPattern((char*)"CvGameCoreDLL.dll", (char*)"\xb9\xff\xff\xff\xff\xe8\xff\xff\xff\xff\x8b\x80\x94\x00\x00\x00", (char*)"x????x????xxxxxx");
	//DWORD getgamespeed = sigscan->FindPattern((char*)"CvGameCoreDLL.dll", (char*)"\xb9\xff\xff\xff\xff\xe8\xff\xff\xff\xff\x8b\x80\x8c\x00\x00\x00", (char*)"x????x????xxxxxx");
	DWORD aliveee = sigscan->FindPattern((char*)"CvGameCoreDLL.dll", (char*)"\x56\x57\x33\xFF\x33\xF6\xEB\x08\x8d", (char*)"xxxxxxxxx");
	DWORD everaliveee = sigscan->FindPattern((char*)"CvGameCoreDLL.dll", (char*)"\x53\x56\x57\x33\xDB\x33\xFF\xEB\x07", (char*)"xxxxxxxxx");

	//\x64\xA1\x00\x00\x00\x00\x8B\x4C\x24\x10\x8B xx????xxxxx
	//DWORD setyear = sigscan->FindPattern((char*)"CvGameCoreDLL.dll", (char*)"\x64\xA1\x00\x00\x00\x00\x8B\x4C\x24\x10\x8B", (char*)"xx????xxxxx");
	// THIS IS BUGGED!!!!

	ogetGameTurn = (basicIntVoid)DetourFunction((PBYTE)getgamet, (PBYTE)Hooked_getGameTurn);
	//oYearStr = (setYearStr)DetourFunction((PBYTE)setyear, (PBYTE)Hooked_setYearStr);
	ocountCivPlayersAlive = (basicIntVoid)DetourFunction((PBYTE)aliveee, (PBYTE)Hooked_countCivPlayersAlive);
	ocountCivPlayersEverAlive = (basicIntVoid)DetourFunction((PBYTE)everaliveee, (PBYTE)Hooked_countCivPlayersEverAlive);

	

	CIV->updateTime();

	while (420 != 69) {
		CIV->updateRPC();
		Sleep(1000);
	}

	return false;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  dwReason,
	LPVOID lpReserved
)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, mainTh, nullptr, 0, nullptr);
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

