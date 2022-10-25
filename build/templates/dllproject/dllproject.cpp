
#include "stdafx.h"


BOOL WINAPI DllMain(_In_ HINSTANCE hDLL, _In_ DWORD dwReason, _In_ LPVOID lpvReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    }

    return TRUE;
}

int WINAPI NoNameFunction()
{
    return 0;
}

int WINAPI NamedFunction()
{
    return 0;
}
