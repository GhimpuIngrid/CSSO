#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <tlhelp32.h>
#include <Winnt.h>
#include <string>


using namespace std;

struct ThreadParameters {
    int threadNumber;
};

HANDLE mutex;

int create_Mapped_Files(const DWORD vectorSize, LPCSTR file_name) {
    HANDLE handel_name = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, vectorSize * sizeof(DWORD), file_name);
    if (handel_name == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut crea fisierul de mapare\n");
        return 0;
    }

    DWORD* v = (DWORD*)MapViewOfFile(handel_name, FILE_MAP_WRITE, 0, 0, vectorSize * sizeof(DWORD));
    if (v == NULL) {
        cout << "Eroare la maparea fisierului in memorie: " << GetLastError() << '\n';
        CloseHandle(handel_name);
        return 0;
    }

    for (DWORD i = 0; i < vectorSize; ++i) {
        v[i] = 0xFFFFFFFF;
    }

    UnmapViewOfFile(v);
    CloseHandle(handel_name);
    return 1;
}

void Create_A_Directory(const char path[]) {

    if (!CreateDirectory(path, NULL)) {

        printf("Failed to create directory: %s. Cod eroare %d.\n", path, GetLastError());
    }

    else printf("%s directory was successfully created. \n", path);

}

DWORD WINAPI ThreadFunction(LPVOID lpParam) {

    ThreadParameters* params = static_cast<ThreadParameters*>(lpParam);
    int threadNumber = params->threadNumber;

    if (threadNumber == 0) {
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);

        ZeroMemory(&processInfo, sizeof(processInfo));

        if (!CreateProcess("C:\\Users\\Ingrid\\Desktop\\4 procese\\Deposit\\x64\\Debug\\Deposit.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
            cout << "Eroare la crearea procesului 1 " << GetLastError();
            return 0;
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);

        cout << "Proces 1 creat cu succes!" << '\n';
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        return 0;
    }

    else if (threadNumber == 1) {
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);

        ZeroMemory(&processInfo, sizeof(processInfo));

        if (!CreateProcess("C:\\Users\\Ingrid\\Desktop\\4 procese\\Sold\\x64\\Debug\\Sold.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
            cout << "Eroare la crearea procesului 2 " << GetLastError();
            return 0;
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);

        cout << "Proces 2 creat cu succes!" << '\n';
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        return 0;
    }

    else {
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);

        ZeroMemory(&processInfo, sizeof(processInfo));

        if (!CreateProcess("C:\\Users\\Ingrid\\Desktop\\4 procese\\Donation\\x64\\Debug\\donation.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
            cout << "Eroare la crearea procesului 3 " << GetLastError();
            return 0;
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);

        cout << "Proces 3 creat cu succes!" << '\n';
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        return 0;
    }
}

int main(int argc, TCHAR* argv[])
{

    mutex = CreateMutex(NULL, FALSE, "deschidere_fisiere_mapate");
    Create_A_Directory("C:\\Facultate");
    Create_A_Directory("C:\\Facultate\\CSSO");
    Create_A_Directory("C:\\Facultate\\CSSO\\Week4");
    Create_A_Directory("C:\\Facultate\\CSSO\\Week4\\Reports");
    Create_A_Directory("C:\\Facultate\\CSSO\\Week4\\Reports\\Summary");

    int rez = create_Mapped_Files(10000, "MarketShelves");
    if (rez == 0) return 0;

    rez = create_Mapped_Files(10000, "MarketValability");
    if (rez == 0) return 0;

    rez = create_Mapped_Files(10000, "ProductPrices");
    if (rez == 0) return 0;


    HANDLE f_sold = CreateFile("C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\sold.txt", (GENERIC_READ | GENERIC_WRITE), 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f_sold == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut crea fisierul sold.txt. %d\n", GetLastError());
        return 0;
    }

    DWORD valueToWrite = 0;
    DWORD bytesWritten;

    if (!WriteFile(f_sold, &valueToWrite, sizeof(valueToWrite), &bytesWritten, NULL)) {
        printf("Eroare la scrierea in fisier. Cod de eroare: %d\n", GetLastError());
        CloseHandle(f_sold);
        return 0;
    }

    CloseHandle(f_sold);

    HANDLE f_donations = CreateFile("C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\donations.txt", (GENERIC_READ | GENERIC_WRITE), 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f_donations == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut crea fisierul donations.txt. %d\n", GetLastError());
        return 0;
    }

    if (!WriteFile(f_donations, &valueToWrite, sizeof(valueToWrite), &bytesWritten, NULL)) {
        printf("Eroare la scrierea in fisier. Cod de eroare: %d\n", GetLastError());
        CloseHandle(f_donations);
        return 0;
    }

    CloseHandle(f_donations);

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

    printf("Am terminat executia thred urilor.\n");
}

