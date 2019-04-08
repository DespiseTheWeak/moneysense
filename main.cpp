#include <stdio.h>
#include <string>
#include <iostream>
#include <ctime>

#include <Windows.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <TlHelp32.h>
#include <WinInet.h>
#include "DLLsize.h"

#include "Hack/includes.hpp"
#include <wchar.h>
#include <thread>
#include <webapplication.h>
#include <fstream>
#include "menu/cmenu.hpp"
#include "utils/csgo.hpp"
#include "hack/AutisticImageL0ader.h"
C_CSGO * $;
#define OWO 1337

using namespace std;
bool pCodenz;
//DON'T change things here!

float licenseyear = 2018;
float licensemonth = 5;
float licenseday = 9; //8 oder 9
const int MAX_VALID_YR = 9999;
const int MIN_VALID_YR = 1800;
//DON'T change things here!
// Returns true if
// given year is valid.
bool isLeap(int year)
{
	// Return true if year
	// is a multiple pf 4 and
	// not multiple of 100.
	// OR year is multiple of 400.
	return (((year % 4 == 0) &&
		(year % 100 != 0)) ||
		(year % 400 == 0));
}
UCHAR szFileSys[255], szVolNameBuff[255];
DWORD dwMFL, dwSysFlags;
DWORD dwSerial;
LPCTSTR szHD = "C:\\";

bool unload = false;
HINSTANCE HThisModule;
bool DoUnload;

PVOID AntiReversing(HMODULE dwModule)
{
	PVOID pEntry = NULL;
	PIMAGE_DOS_HEADER pId = (PIMAGE_DOS_HEADER)dwModule;
	PIMAGE_NT_HEADERS pInt = (PIMAGE_NT_HEADERS)(dwModule + pId->e_lfanew);
	pEntry = dwModule + pInt->OptionalHeader.BaseOfCode;
	return pEntry;
}

void fontscheck()
{ 

}

void AntiHeaders(HINSTANCE hModule)
{
	DWORD dwPEB_LDR_DATA = 0;
	_asm
	{
		pushad;
		pushfd;
		mov eax, fs:[30h]
			mov eax, [eax + 0Ch]
			mov dwPEB_LDR_DATA, eax

			InLoadOrderModuleList :
		mov esi, [eax + 0Ch]
			mov edx, [eax + 10h]

			LoopInLoadOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 18h]
			cmp ecx, hModule
			jne SkipA
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp InMemoryOrderModuleList
			SkipA :
		cmp edx, esi
			jne LoopInLoadOrderModuleList

			InMemoryOrderModuleList :
		mov eax, dwPEB_LDR_DATA
			mov esi, [eax + 14h]
			mov edx, [eax + 18h]

			LoopInMemoryOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 10h]
			cmp ecx, hModule
			jne SkipB
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp InInitializationOrderModuleList
			SkipB :
		cmp edx, esi
			jne LoopInMemoryOrderModuleList

			InInitializationOrderModuleList :
		mov eax, dwPEB_LDR_DATA
			mov esi, [eax + 1Ch]
			mov edx, [eax + 20h]

			LoopInInitializationOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 08h]
			cmp ecx, hModule
			jne SkipC
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp Finished
			SkipC :
		cmp edx, esi
			jne LoopInInitializationOrderModuleList

			Finished :
		popfd;
		popad;
	}
}


void HideModule(HINSTANCE hModule)
{
	DWORD dwPEB_LDR_DATA = 0;
	_asm
	{
		pushad;
		pushfd;
		mov eax, fs:[30h]
			mov eax, [eax + 0Ch]
			mov dwPEB_LDR_DATA, eax
			InLoadOrderModuleList :
		mov esi, [eax + 0Ch]
			mov edx, [eax + 10h]
			LoopInLoadOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 18h]
			cmp ecx, hModule
			jne SkipA
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp InMemoryOrderModuleList
			SkipA :
		cmp edx, esi
			jne LoopInLoadOrderModuleList
			InMemoryOrderModuleList :
		mov eax, dwPEB_LDR_DATA
			mov esi, [eax + 14h]
			mov edx, [eax + 18h]
			LoopInMemoryOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 10h]
			cmp ecx, hModule
			jne SkipB
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp InInitializationOrderModuleList
			SkipB :
		cmp edx, esi
			jne LoopInMemoryOrderModuleList
			InInitializationOrderModuleList :
		mov eax, dwPEB_LDR_DATA
			mov esi, [eax + 1Ch]
			mov edx, [eax + 20h]
			LoopInInitializationOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 08h]
			cmp ecx, hModule
			jne SkipC
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp Finished
			SkipC :
		cmp edx, esi
			jne LoopInInitializationOrderModuleList
			Finished :
		popfd;
		popad;
	}
}

bool IsProcessRun(const char * const processName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe);

	while (1) {
		if (strcmp(pe.szExeFile, processName) == 0) return true;
		if (!Process32Next(hSnapshot, &pe)) return false;
	}
}

void ErasePE()
{
	//removed
}

void checkversionstring()
{

	if (lstrcmpA($->m_engine()->GetProductVersionString(), "1.36.8.4") == 0)
	{
		hooks::shutdown();
	}
	else
	{
		MessageBox(0, "oudate u hack", "moneysen$se", 0);
		ExitProcess(0);
		exit(0);
	}
}

DWORD WINAPI CheatEntry(LPVOID lpThreadParameter)
{
	HMODULE hModule = (HMODULE)lpThreadParameter;

	checkversionstring(); // проверка на версию кс саси жопу бля 

	return 0;
}

bool isValidDate(int d, int m, int y)
{
	// If year, month and day
	// are not in given range
	if (y > MAX_VALID_YR ||
		y < MIN_VALID_YR)
		return false;
	if (m < 1 || m > 12)
		return false;
	if (d < 1 || d > 31)
		return false;

	// Handle February month
	// with leap year
	if (m == 2)
	{
		if (isLeap(y))
			return (d <= 29);
		else
			return (d <= 28);
	}

	// Months of April, June,
	// Sept and Nov must have
	// number of days less than
	// or equal to 30.
	if (m == 4 || m == 6 ||
		m == 9 || m == 11)
		return (d <= 30);

	return true;
}

DWORD WINAPI OnDllAttach(PVOID base) {
	materials::get().setup();
#ifdef _DEBUG
	fridge::AttachConsole();
#endif
	fridge::ConsolePrint("Initializing...\n");
	render::get().setup();
	cfg_manager->setup();
	setup_main_menu();
	netvars::get().init();
	hooks::load();

	fridge::ConsolePrint("Finished Initializing!\n");
	while (!unload) {
		using namespace literals::chrono_literals;
		this_thread::sleep_for(1s);
	}
	fridge::ConsolePrint("Uninjecting...!\n");
	this_thread::sleep_for(chrono::milliseconds(100));
	hooks::shutdown();
	this_thread::sleep_for(chrono::milliseconds(100));
	fridge::ConsolePrint("Uninjected!\n");
	this_thread::sleep_for(chrono::milliseconds(100));
	fridge::DetachConsole();
	this_thread::sleep_for(chrono::milliseconds(100));
	fclose((FILE*)stdin);
	fclose((FILE*)stdout);
	FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{

	if (IsProcessRun("idaq64.exe"))
	{
		MessageBox(0, "Мать жива?", "Пентагон", MB_OK);
		exit(1);
	}

	if (IsProcessRun("idaq32.exe"))
	{
		MessageBox(0, "Ау дверь чек там вроде к те пришли", "Пентагон", MB_OK);
		exit(1);
	}

	if (IsProcessRun("ollydbg.exe"))
	{
		MessageBox(0, "ебать щас бы с ольги реверсить", "Пентагон", MB_OK);
		exit(1);
	}

	if (IsProcessRun("ollydbg64.exe"))
	{
		MessageBox(0, "ебать щас бы с ольги реверсить", "Пентагон", MB_OK);
		exit(1);
	}

	if (IsProcessRun("loaddll.exe"))
	{
		MessageBox(0, "у тя матери нет", "Пентагон", MB_OK);
		exit(1);
	}

	if (IsProcessRun("httpdebugger.exe"))
	{
		MessageBox(0, "ебать ты даун длл в байтах саси", "Пентагон", MB_OK);
		exit(1);
	}


	if (IsProcessRun("windowrenamer.exe"))
	{
		MessageBox(0, "щас бы быть в 2к08", "Пентагон", MB_OK);
		exit(1);
	}


	if (IsProcessRun("processhacker.exe"))
	{
		MessageBox(0, "Эйй! Салам алейкум", "Ержан", MB_OK);
		exit(1);
	}

	std::time_t result = std::time(nullptr);
	if (dwReason == DLL_PROCESS_ATTACH) {
		GetVolumeInformation(szHD, (LPTSTR)szVolNameBuff, 255, &dwSerial, &dwMFL, &dwSysFlags, (LPTSTR)szFileSys, 255);

		static int MaxYear = 0; // 1016;
		static int MaxMonth = 0; // 5;
		static float MaxDay = 0; // 15.5f;
		static int nineteenhundred = 0; // 950;

		//change values HERE!!!!!!
		MaxYear = 1019;
		MaxMonth = 3;
		MaxDay = 15.5f;
		nineteenhundred = 950;

		licensemonth = 10;
		licenseyear = MaxYear;
		licenseyear += 1000;
		licenseyear += 2;
		licensemonth--;
		licenseday = 31;
		//removed protection so you pasters dont scream EEEEEEEEEEEEEEEEEEEE ITS NOT WORKING FAKING BAD RELEASED -REP
		//and yes it was a date protection (if the dll is older than 15 days you coudnt inject the dll bc bad date)
		g_ctx.m_globals.lpReserved = hModule;
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, NULL, OnDllAttach, hModule, NULL, nullptr);
		//CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(menu::render), nullptr, 0, nullptr);
	}
	return TRUE;
}