#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <tlhelp32.h>


void Create_A_Directory(const char path[]) {

    if (!CreateDirectory(path, NULL)) {

        printf("Failed to create directory: %s. Cod eroare %d.\n", path, GetLastError());
    }

    else printf("%s directory was successfully created. \n", path);

}

void int_to_char(char chr[], int n) {
    chr[0] = '\0';
    char itoc[20];
    int len = 0;
    int len_chr = 0;
    int i, j;

    while (n > 0) {
        itoc[len] = n % 10 + '0';
        n = n / 10;
        len++;
    }
    itoc[len] = '\0';

    for (int i = len - 1; i >= 0; i--) {
        chr[len_chr] = itoc[i];
        len_chr++;
    }
        
    chr[len_chr] = ' ';
    len_chr++;
    chr[len_chr] = '\0';
}

void create_aux(int a, int b, char aux[]) {
    char chr[20];
    aux[0] = '\0';
    int_to_char(chr, a);
    strcpy(aux, chr);
    int_to_char(chr, b);
    strcat(aux, chr);

}

int process_enum();
int thread_enum();
int module_enum();


int main()
{
    
    
    Create_A_Directory("C:\\Facultate");
    Create_A_Directory("C:\\Facultate\\CSSO");
    Create_A_Directory("C:\\Facultate\\CSSO\\Laboratoare");
    Create_A_Directory("C:\\Facultate\\CSSO\\Laboratoare\\Week3");
    Create_A_Directory("C:\\Facultate\\CSSO\\Laboratoare\\Week3\\ProcessInfo");

    char toWrite_Aux[1000];
    strcpy(toWrite_Aux, "Procese: ");

    int nr;
    char aux[1000];

    nr = process_enum();
    if (nr == -1) {
        printf("Eroare la enumerarea proceselor.\n");
        return 0;
    }

    int_to_char(aux, nr);
    strcat(toWrite_Aux, aux);
    strcat(toWrite_Aux, "\nFire: ");

    nr = thread_enum();
    if (nr == -1) {
        printf("Eroare la enumerarea thread-urilor.\n");
        return 0;
    }

    int_to_char(aux, nr);
    strcat(toWrite_Aux, aux);
    strcat(toWrite_Aux, "\nModule: ");

    nr = module_enum();

    if (nr == -1) {
        printf("Eroare la enumerarea modulelor.\n");
        return 0;
    }

    int_to_char(aux, nr);
    strcat(toWrite_Aux, aux);
    strcat(toWrite_Aux, "\0");

    HANDLE mFile = OpenFileMapping(FILE_MAP_WRITE, FALSE, "cssoh3basicsync");
    if (mFile == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut deschide fisierul de mapare.\n");
        return 0;
    }
    
    LPVOID fileView = MapViewOfFile(mFile, FILE_MAP_WRITE, 0, 0, 0);
    if (!fileView) {
        printf("Nu s-a putut aduce fisierul in memoria virtuala.\n");
        return 0;
    }

    const char* toWrite = toWrite_Aux;
    memcpy(fileView, toWrite, strlen(toWrite));

    printf("Am scris cu succes in fisierul mapat.\n");

    UnmapViewOfFile(fileView);
    CloseHandle(mFile);
}

int process_enum(){
    char myBuffer[200];
    memset(myBuffer, 0, 200);
    int l;
    int count = 0;

    HANDLE proces_file = CreateFile("C:\\Facultate\\CSSO\\Laboratoare\\Week3\\ProcessInfo\\procese.txt", (GENERIC_READ | GENERIC_WRITE), NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (proces_file == INVALID_HANDLE_VALUE) {
        printf("Am o eroare la crearea fisierului. Cod 	eroare: %d\n", GetLastError());

        return -1;
    }

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        printf("CreateToolhelp32Snapshot failed %d \n", GetLastError());
        return -1;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        printf("Process32First failed. err = %d \n", GetLastError());
        CloseHandle(hProcessSnap);
        return -1;
    }
    do
    {
        char aux[100];
        create_aux(pe32.th32ParentProcessID, pe32.th32ProcessID, aux);
        strcat(aux, pe32.szExeFile);
        strcat(aux, "\n\0");
        strcpy_s(myBuffer, aux);
        l = strlen(myBuffer);

        if (WriteFile(proces_file, myBuffer, l, NULL, NULL) == 0) {
            printf("Eroare la scriere in fisier. Cod eroare: %d\n", GetLastError());
            CloseHandle(proces_file);
            return -1;
        }

        count++;

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    CloseHandle(proces_file);

    return count;
}

int thread_enum() {
    char myBuffer[200];
    memset(myBuffer, 0, 200);
    int l;
    int count = 0;

    HANDLE thread_file = CreateFile("C:\\Facultate\\CSSO\\Laboratoare\\Week3\\ProcessInfo\\fire.txt", (GENERIC_READ | GENERIC_WRITE), NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (thread_file == INVALID_HANDLE_VALUE) {
        printf("Am o eroare la crearea fisierului. Cod 	eroare: %d\n", GetLastError());
        return -1;
    }

    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
    {
        printf("CreateToolhelp32Snapshot failed %d \n", GetLastError());
        return -1;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hThreadSnap, &te32))
    {
        printf("Thread32First failed. err = %d \n", GetLastError());
        CloseHandle(hThreadSnap);
        return -1;
    }
    do
    {
        char aux[100];
        create_aux(te32.th32ThreadID, te32.th32OwnerProcessID, aux);
        strcat(aux, "\n\0");
        strcpy_s(myBuffer, aux);
        l = strlen(myBuffer);

        if (WriteFile(thread_file, myBuffer, l, NULL, NULL) == 0) {
            printf("Eroare la scriere in fisier. Cod eroare: %d\n", GetLastError());
            CloseHandle(thread_file);
            return -1;
        }

        count++;

    } while (Thread32Next(hThreadSnap, &te32));

    CloseHandle(hThreadSnap);
    CloseHandle(thread_file);

    return count;
}

int module_enum() {
    char myBuffer[200];
    memset(myBuffer, 0, 200);
    int l;
    int count = 0;

    HANDLE module_file = CreateFile("C:\\Facultate\\CSSO\\Laboratoare\\Week3\\ProcessInfo\\module_process.txt", (GENERIC_READ | GENERIC_WRITE), NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (module_file == INVALID_HANDLE_VALUE) {
        printf("Am o eroare la crearea fisierului. Cod 	eroare: %d\n", GetLastError());
        return -1;
    }

    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        printf("CreateToolhelp32Snapshot failed %d \n", GetLastError());
        return -1;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hModuleSnap, &me32))
    {
        printf("Module32First failed. err = %d \n", GetLastError());
        CloseHandle(hModuleSnap);
        return -1;
    }

    do
    {
        char aux[100];
        create_aux(me32.th32ModuleID, me32.th32ProcessID, aux);
        strcat(aux, me32.szModule);
        strcat(aux, " ");
        strcat(aux, me32.szExePath);
        strcat(aux, "\n\0");
        strcpy_s(myBuffer, aux);
        l = strlen(myBuffer);

        if (WriteFile(module_file, myBuffer, l, NULL, NULL) == 0) {
            printf("Eroare la scriere in fisier. Cod eroare: %d\n", GetLastError());
            CloseHandle(module_file);
            return -1;
        }

        count++;

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
    CloseHandle(module_file);

    return count;
}