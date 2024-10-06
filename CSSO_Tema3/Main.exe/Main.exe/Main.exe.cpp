#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <tlhelp32.h>
#include <Winnt.h>
#include <string>

using namespace std;

int main(int argc, TCHAR *argv[])
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

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    ZeroMemory(&processInfo, sizeof(processInfo));
    

    if (!CreateProcess(NULL, argv[1], NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
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

    CloseHandle(mFile);

 

    STARTUPINFO sInfo;
    PROCESS_INFORMATION pInfo;

    ZeroMemory(&sInfo, sizeof(sInfo));
    sInfo.cb = sizeof(sInfo);

    ZeroMemory(&pInfo, sizeof(pInfo));


    if (!CreateProcess(NULL, argv[2], NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo)) {
        printf("Eroare la crearea procesului 2 %d.\n", GetLastError());
        return 0;
    }

    STARTUPINFO stInfo;
    PROCESS_INFORMATION prInfo;

    ZeroMemory(&stInfo, sizeof(stInfo));
    stInfo.cb = sizeof(stInfo);

    ZeroMemory(&prInfo, sizeof(prInfo));


    if (!CreateProcess(NULL, argv[3], NULL, NULL, FALSE, 0, NULL, NULL, &stInfo, &prInfo)) {
        printf("Eroare la crearea procesului 3 %d.\n", GetLastError());
        return 0;
    }

    WaitForSingleObject(prInfo.hProcess, INFINITE);
    printf("Proces terminat cu succes!\n");
    CloseHandle(prInfo.hProcess);
    CloseHandle(prInfo.hThread);

    CloseHandle(pInfo.hProcess);
    CloseHandle(pInfo.hThread);

    return 0;
}