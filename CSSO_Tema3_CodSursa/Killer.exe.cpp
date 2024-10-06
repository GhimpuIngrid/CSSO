#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <tlhelp32.h>
#include <Winnt.h>
#include <string>

int main()
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    int processID;
    int ok = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        printf("CreateToolhelp32Snapshot failed %d \n", GetLastError());
        return 0;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        printf("Process32First failed. err = %d \n", GetLastError());
        CloseHandle(hProcessSnap);
        return 0;
    }
    do
    {
        if (strcmp(pe32.szExeFile, "Filter.exe.exe") == 0) {
            processID = pe32.th32ProcessID;
            ok = 1;
        }

    } while (Process32Next(hProcessSnap, &pe32) && ok == 0);

    if (ok == 0) {
        printf("Nu am gasit procesul Filter.exe");
        CloseHandle(hProcessSnap);
        return 0;
    }

    printf("Am gasit pid ul procesului Filter.exe: %d\n", processID);

    CloseHandle(hProcessSnap);

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);

    if(hProcess == INVALID_HANDLE_VALUE)
    {
        printf("Eroare la deschiderea procesului %d \n", GetLastError());
        return 0;
    }

    if (!TerminateProcess(hProcess, -1)) {
        printf("Procesul nu a putut fi terminat\n");
        CloseHandle(hProcess);
        return 0;
    }
    
    printf("Procesul a fost terminat cu succes!\n");
    CloseHandle(hProcess);

    return 0;
}
