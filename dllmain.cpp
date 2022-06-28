#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#pragma warning(disable : 4996)
#include <Psapi.h>
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
#ifdef _WIN64
#define PTRMAXVAL ((DWORD64)0x000F000000000000)
#else
#define PTRMAXVAL ((DWORD64)0xFFF00000)
#endif

DWORD64 FindPattern(const char* szModule, const char* szSignature)
{
    MODULEINFO modInfo;
    GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
    DWORD64 startAddress = (DWORD64)GetModuleHandleA(szModule);
    DWORD64 endAddress = (startAddress + (DWORD64)modInfo.SizeOfImage);
    const char* pat = szSignature;
    DWORD64 firstMatch = 0;
    for (DWORD64 pCur = startAddress; pCur < endAddress; pCur++)
    {
        if (!*pat) return firstMatch;
        if (*(PBYTE)pat == ('\?') || *(BYTE*)pCur == getByte(pat))
        {
            if (!firstMatch) firstMatch = pCur;
            if (!pat[2]) return firstMatch;
            if (*(PWORD)pat == ('\?\?') || *(PBYTE)pat != ('\?')) pat += 3;
            else pat += 2;
        }
        else {
            pat = szSignature;
            firstMatch = 0;
        }
    }
    return NULL;
}

float multiplier = 1000.f;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        DWORD64 GworldPattern = FindPattern(nullptr, "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81");
        MessageBox(NULL, L"Super Punch loaded !", L"https://github.com/pedrin0x10/FiveM-Super-Punch", MB_OK);
        while (true) {
            DWORD64 LocalPlayer = *(DWORD64*)(*(DWORD64*)(GworldPattern + *(int*)(GworldPattern + 0x3) + 0x7) + 0x8);
            *(float*)(*(DWORD64*)(LocalPlayer + 0x10C8) + 0x0D1C) = multiplier;

            Sleep(1000);
        }
        return TRUE;
    }
    return TRUE;
}