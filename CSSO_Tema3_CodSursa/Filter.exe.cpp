#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <tlhelp32.h>
#include <Winnt.h>
#include <string>

using namespace std;

void  find_in_file(LPCSTR file_name, string text);

struct ThreadParameters {
    int threadNumber;
};

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    ThreadParameters* params = static_cast<ThreadParameters*>(lpParam);
    int threadNumber = params->threadNumber;

    if (threadNumber == 0)
        find_in_file("C:\\Facultate\\CSSO\\Laboratoare\\Week3\\ProcessInfo\\procese.txt", "svchost.exe");

    else if(threadNumber == 1)
        find_in_file("C:\\Facultate\\CSSO\\Laboratoare\\Week3\\ProcessInfo\\fire.txt", "228");

    else if (threadNumber == 2)
        find_in_file("C:\\Facultate\\CSSO\\Laboratoare\\Week3\\ProcessInfo\\module_process.txt", "KERNEL32.DLL");

    delete params;

    return 0;
}



int main()
{
    HANDLE hToken;
    HANDLE hProcess = GetCurrentProcess();
    if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        printf("Eroare la obtinerea tokenului : %d\n", GetLastError());
        return 0;
    }

    LPCWSTR privilegeName = L"SeSystemProfilePrivilege";
    LUID privilegeLuid;

    if (!LookupPrivilegeValueW(NULL, privilegeName, &privilegeLuid)) {
        printf("Eroare la obtinerea id ului privilegiului: %d\n", GetLastError());
        return 0;
    }

    TOKEN_PRIVILEGES tokenPrivileges;
    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Luid = privilegeLuid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        printf("Eroare la setarea privilegiului: %d\n", GetLastError());
        return 0;
    }


    HANDLE hThreads[3];
    DWORD dwThreadIds[3];

    for (int i = 0; i < 3; i++) {

        ThreadParameters* params = new ThreadParameters;
        params->threadNumber = i;

        hThreads[i] = CreateThread(NULL, 0, ThreadFunction, params, 0, &dwThreadIds[i]);

        if (hThreads[i] == NULL) {
            printf("Eroare la crearea thred ului %d\n", GetLastError());
            return 0;
        }
    }

    WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);

    for (int i = 0; i < 3; i++) {
        CloseHandle(hThreads[i]);
    }

    printf("Am terminat executioa thred urilor si intru in sleep pentru 30 de min.\n");

    DWORD millisecondsToSuspend = 30 * 60 * 1000;
    Sleep(millisecondsToSuspend);
    
    return 0;
}

void find_in_file(LPCSTR file_name, string text)
{
    HANDLE file_read = CreateFile(file_name, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_read == INVALID_HANDLE_VALUE) {
        printf("Eroare la deschidera fisierului %d\n", GetLastError());
        CloseHandle(file_read);
    }
    else {
        char myBuffer[2000];
        memset(myBuffer, 0, 2000);
        DWORD bytesRead;

        if (!ReadFile(file_read, myBuffer, 2000, &bytesRead, NULL)) {
            printf("Eroare la citirea fisierului %d\n", GetLastError());
            CloseHandle(file_read);
        }
        else {
            const char* start = myBuffer;
            const char* end = myBuffer;
            int lineNumber = 1;
            int count = 0;

            while (*end != '\0') {
                if (*end == '\n' || *end == '\0') {
                    
                    size_t lineLength = end - start;

                    string line(start, lineLength);

                    if (line.find(text) != std::string::npos) {
                        count++;
                    }

                    start = end + 1;
                    lineNumber++;
                }
                end++;
            }

            printf("In fisierul %s am gasit sirul de caractere specificat pe %d linii.\n", file_name, count);
            CloseHandle(file_read);
        }
    }
}
