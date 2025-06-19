// dllmain.cpp : Définit le point d'entrée de l'application DLL.
#include "pch.h"

DWORD WINAPI mainThread(LPVOID lpParam);

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainThread, NULL, NULL, NULL);
    }
    return TRUE;
}

