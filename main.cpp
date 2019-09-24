#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#include "global.h"
#include "UEDump.h"

using namespace std;

DWORD64 GNames_offset;
DWORD64 GObjObjects_offset;
TNameEntryArray* GNames = NULL;
FUObjectArray* GlobalObjects = NULL;

MODULEINFO GetModuleInfo(LPCTSTR lpModuleName)
{
	MODULEINFO miInfos = { NULL };

	HMODULE hmModule = GetModuleHandle(lpModuleName);

	if (hmModule)
	{
		GetModuleInformation(GetCurrentProcess(), hmModule, &miInfos, sizeof(MODULEINFO));
	}

	return miInfos;
}

void onAttach()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	MODULEINFO miGame = GetModuleInfo(NULL);

	printf("Module Address: 0x%llX\n", (DWORD64)miGame.lpBaseOfDll);

	//Gname offset
	GNames_offset = (DWORD64)miGame.lpBaseOfDll + 0x5DF2C88;
	//GObject offset
	GObjObjects_offset = (DWORD64)((DWORD64)miGame.lpBaseOfDll + 0x5F45A90);

	printf("GNames Address: 0x%llX\n", GNames_offset);
	printf("GObjects Address: 0x%llX\n", GObjObjects_offset);

	GNames = (TNameEntryArray*)GNames_offset;
	GlobalObjects = (FUObjectArray*)GObjObjects_offset;

	NameDump();
	ObjectDump();
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)onAttach, NULL, 0, NULL);
		return true;
		break;

	case DLL_PROCESS_DETACH:
		return true;
		break;
	}
}