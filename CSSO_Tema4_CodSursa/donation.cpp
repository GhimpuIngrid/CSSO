#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

int main()
{
    int vectorSize = 10000;
    HANDLE mutex = OpenMutex(NULL, FALSE, "deschidere_fisiere_mapate");
    if (mutex == INVALID_HANDLE_VALUE) {
        printf("Eroare la deschiderea mutexului. \n");
        CloseHandle(mutex);
        return 0;
    }
    else printf("Mutex deschis cu succes in donation!\n");

    WaitForSingleObject(mutex, INFINITE);
    printf("Pot folosi resursele partajate\n");

    HANDLE hShelves = OpenFileMapping(FILE_MAP_READ, FALSE, "MarketShelves");
    if (hShelves == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut deschide fisierul de mapare MarketShelves.\n");
        return 0;
    }

    DWORD* shelves = (DWORD*)MapViewOfFile(hShelves, FILE_MAP_READ, 0, 0, vectorSize * sizeof(DWORD));
    if (shelves == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut aduce fisierul MarketShelves in memoria virtuala.\n");
        return 0;
    }

    HANDLE hValability = OpenFileMapping(FILE_MAP_READ, FALSE, "MarketValability");
    if (hValability == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut deschide fisierul de mapare MarketValability.\n");
        return 0;
    }

    DWORD* valability = (DWORD*)MapViewOfFile(hValability, FILE_MAP_READ, 0, 0, vectorSize * sizeof(DWORD));
    if (valability == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut aduce fisierul MarketValability in memoria virtuala.\n");
        return 0;
    }


    HANDLE hPrices = OpenFileMapping(FILE_MAP_READ, FALSE, "ProductPrices");
    if (hPrices == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut deschide fisierul de mapare ProductPrices.\n");
        return 0;
    }

    DWORD* prices = (DWORD*)MapViewOfFile(hPrices, FILE_MAP_READ, 0, 0, vectorSize * sizeof(DWORD));
    if (prices == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut aduce fisierul ProductPrices in memoria virtuala.\n");
        return 0;
    }

    printf("Am terminat de citit din fisierele mapate si eliberez mutex din donation\n");

    ReleaseMutex(mutex);
    CloseHandle(mutex);

    for (size_t i = 0; i < vectorSize; i++) {
        if (valability[i] == 0xFFFFFFFF) continue;
        else if (valability[i] == 0) {
            HANDLE hDonation = CreateFile("C:\\Facultate\\CSSO\Week4\\Reports\\Summary\\donations.txt", (GENERIC_READ | GENERIC_WRITE), NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hDonation == INVALID_HANDLE_VALUE) {
                cout << "Nu am putut deschide fisierul sold.txt " << GetLastError() << '\n';
                return 0;
            }

            char myBuffer_sold[20000];
            memset(myBuffer_sold, 0, 20000);
            DWORD bytesCititi_sold = 0;
            DWORD bytesWritten_sold = 0;

            if (ReadFile(hDonation, myBuffer_sold, 20000, &bytesCititi_sold, NULL) == FALSE) {
                printf("Eroare la scrierea in fisier. Cod de eroare: %d\n", GetLastError());
                CloseHandle(hDonation);
                return 0;
            }

            int val_actualizata = 0;
            char_to_int(myBuffer_sold, val_actualizata);
            val_actualizata += prices[id_produs];

            string val_actualizata_de_scris;
            val_actualizata_de_scris.clear();

            while (val_actualizata) {
                string auxiliar = to_string(val_actualizata % 10);
                val_actualizata_de_scris.insert(0, auxiliar);

                val_actualizata /= 10;
            }

            SetFilePointer(hDonation, 50, NULL, FILE_BEGIN);


            if (!WriteFile(hDonation, &val_actualizata_de_scris, sizeof(val_actualizata_de_scris), &bytesWritten_sold, NULL)) {
                printf("Eroare la scrierea in fisier. Cod de eroare: %d\n", GetLastError());
                CloseHandle(hDonation);
                return 0;
            }

            CloseHandle(hDonation);
        }
    }
}
