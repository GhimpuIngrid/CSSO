#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <tlhelp32.h>
#include <Winnt.h>
#include <string>

using namespace std;

int main()
{
    const DWORD fileSize = 4096;
    LPCSTR file_name = "cssoh3basicsync";
    
    HANDLE mFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, fileSize, file_name);
    if (mFile == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut crea fisierul de mapare\n");
        return 0;
    }

    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
    //startupInfo.cb = sizeof(STARTUPINFO);

    ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
    

    if (!CreateProcess("C:\\CSSO_Tema3\\Collect", NULL, NULL, NULL, FALSE, NULL, 0, 0, &startupInfo, &processInfo)) {
        printf("Eroare la crearea procesului 1 %d.\n", GetLastError());
        return 0;
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    printf("Proces creat cu succes!\n");
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    LPVOID fileView = MapViewOfFile(mFile, FILE_MAP_READ, 0, 0, 0);
    if (fileView == INVALID_HANDLE_VALUE) {
        printf("Eroare la aducerea in memorie a fisierului %d\n", GetLastError());
        CloseHandle(mFile);
        return 0;
    }

    cout << static_cast<const char*>(fileView) << '\n';

    STARTUPINFO sInfo;
    PROCESS_INFORMATION pInfo;

    ZeroMemory(&sInfo, sizeof(STARTUPINFO));
    //startupInfo.cb = sizeof(STARTUPINFO);

    ZeroMemory(&pInfo, sizeof(PROCESS_INFORMATION));


    if (!CreateProcess("C:\\CSSO_Tema3\\Filter.exe", NULL, NULL, NULL, FALSE, NULL, 0, 0, &sInfo, &pInfo)) {
        printf("Eroare la crearea procesului 2 %d.\n", GetLastError());
        return 0;
    }

    STARTUPINFO stInfo;
    PROCESS_INFORMATION prInfo;

    ZeroMemory(&stInfo, sizeof(STARTUPINFO));
    //startupInfo.cb = sizeof(STARTUPINFO);

    ZeroMemory(&prInfo, sizeof(PROCESS_INFORMATION));


    if (!CreateProcess("C:\\CSSO_Tema3\\Killer.exe", NULL, NULL, NULL, FALSE, NULL, 0, 0, &stInfo, &prInfo)) {
        printf("Eroare la crearea procesului 3 %d.\n", GetLastError());
        return 0;
    }

    printf("Proces terminat cu succes!\n");
    CloseHandle(prInfo.hProcess);
    CloseHandle(prInfo.hThread);

    CloseHandle(pInfo.hProcess);
    CloseHandle(pInfo.hThread);

    return 0;
}