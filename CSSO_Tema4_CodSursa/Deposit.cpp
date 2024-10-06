#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

bool CompareFileNames(const WIN32_FIND_DATA& a, const WIN32_FIND_DATA& b) {
    std::cout << strlen(a.cFileName) << " " << a.cFileName << '\n';
    std::cout << strlen(b.cFileName) << " " << b.cFileName << '\n';
    std::cout << '\n';
    return strcmp(a.cFileName, b.cFileName) < 0;

}

void string_to_char(vector<char> v, int& a) {
    int aux;
    for (size_t i = 0; i < v.size(); i++) {
        aux = stoi(string(1, v[i]));
        a = a * 10 + aux;
    }
}


int main()
{
    int vectorSize = 10000;
    HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, "deschidere_fisiere_mapate");
    if (mutex == INVALID_HANDLE_VALUE) {
        printf("Eroare la deschiderea mutexului. \n");
        CloseHandle(mutex);
        return 0;
    }
    else printf("Mutex deschis cu succes in Deposit!\n");

    WaitForSingleObject(mutex, INFINITE);
    printf("Pot folosi resursele partajate\n");

    HANDLE hShelves = OpenFileMapping(FILE_MAP_READ, FALSE, "MarketShelves");
    if (hShelves == INVALID_HANDLE_VALUE) {
        printf("Nu s-a putut deschide fisierul de mapare MarketShelves.\n");
        return 0;
    }

    DWORD* shelves = (DWORD*)MapViewOfFile(hShelves, FILE_MAP_READ, 0, 0, vectorSize *sizeof(DWORD));
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

    printf("Am terminat de citit din fisierele mapate si eliberez mutex din Deposit\n");

    ReleaseMutex(mutex);
    CloseHandle(mutex);


    //variabila in care retinem datele fisierului gasit prin FindFirstFile
    WIN32_FIND_DATA FileData;
    const char* folderPath = "C:\\Users\\Ingrid\\Desktop\\deposit\\";
    const char* searchPattern = "*.txt";

    //cautam primul fisier din directorul citit
    HANDLE search = FindFirstFile((std::string(folderPath) + searchPattern).c_str(), &FileData);

    if (search == INVALID_HANDLE_VALUE) {

        printf("No text files found.\n");

        return 0;
    }
    

    std::vector<WIN32_FIND_DATA> files;

    do{
        files.push_back(FileData);

    } while (FindNextFile(search, &FileData) != 0);

    cout << "Am citit toate fisierele si le am pus in vector" << '\n';

    FindClose(search);

    vector<string> ordine_finala;

    for (size_t i = 0; i < files.size(); i++) {
        if (strlen(files[i].cFileName) == 13) {
            string aux;
            for (size_t j = i + 1; j < files.size(); j++) {

                if (strlen(files[i].cFileName) == strlen(files[j].cFileName) && strlen(files[i].cFileName) == 13)
                {
                    if (strcmp(files[i].cFileName, files[j].cFileName) == -1)
                        aux = files[i].cFileName;
                    else if (strcmp(files[i].cFileName, files[j].cFileName) == 1)
                        aux = files[j].cFileName;
                }
            }
            
            ordine_finala.push_back(aux);
        }
    }
    ordine_finala.push_back("2023.11.9.txt");

    for (size_t i = 0; i < files.size(); i++) {
        
        if (strlen(files[i].cFileName) == 14) {
            string aux;
            for (size_t j = i + 1; j < files.size(); j++) {

                if (strlen(files[i].cFileName) == strlen(files[j].cFileName) && strlen(files[i].cFileName) == 14)
                {
                    if (strcmp(files[i].cFileName, files[j].cFileName) == -1)
                        aux = files[i].cFileName;
                    else if (strcmp(files[i].cFileName, files[j].cFileName) == 1)
                        aux = files[j].cFileName;
                }
            }
            
            ordine_finala.push_back(aux);
        }
    }
    ordine_finala.push_back("2023.11.29.txt");

    for (size_t i = 0; i < ordine_finala.size(); i++) {
        while (ordine_finala[i] == "NULL") i++;
        std::string filePath = std::string(folderPath) + ordine_finala[i];
        std::cout << "Processing file: " << filePath << std::endl;

        char myBuffer[20000];
        memset(myBuffer, 0, FileData.nFileSizeLow);
        DWORD bytesCititi = 0;

        HANDLE fisier_citire = CreateFile(("C:\\Users\\Ingrid\\Desktop\\deposit\\" + std::string(FileData.cFileName)).c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fisier_citire == INVALID_HANDLE_VALUE) {

            printf("Am o eroare la deschiderea fisierului. Cod eroare: %d\n", GetLastError());

            return 0;
        }

        else if (ReadFile(fisier_citire, myBuffer, FileData.nFileSizeLow, &bytesCititi, NULL) == FALSE) {

            printf("Am o eroare la ReadFile!! Cod eroare %d\n", GetLastError());
            CloseHandle(fisier_citire);
            return 0;

        }

        printf("Am reusit sa citesc din fisier %d caractere.\n", bytesCititi);
        int ok, id_produs, expires_in, shelve_id, product_price;
        vector<char>aux_data;
        for (int i = 0; i < bytesCititi; i++) {
            ok = 0;
            id_produs = expires_in = shelve_id = product_price = 0;
            while (myBuffer[i] != '\n') {
                aux_data.clear();
                while (myBuffer[i] >= '0' && myBuffer[i] <= '9') {
                    aux_data.push_back(myBuffer[i]);
                    i++;
                }

                if (ok == 0)
                    string_to_char(aux_data, id_produs);
                else if (ok == 1)
                    string_to_char(aux_data, expires_in);
                else if (ok == 2)
                    string_to_char(aux_data, shelve_id);
                else
                    string_to_char(aux_data, product_price);
                ok++;
                i++;
            }
            cout << "Am terminat un rand " << '\n';

            //cout << "AAAAAAAAA" << shelves[shelve_id] << '\n';

            if (shelves[shelve_id] != 0xFFFFFFFF) {

                HANDLE event_errors = OpenEvent(SYNCHRONIZE, FALSE, "event_errors.txt");
                if (event_errors == INVALID_HANDLE_VALUE) {
                    printf("Eroare la deschiderea eventului. \n");
                    CloseHandle(event_errors);
                    return 0;
                }
                else printf("Event deschis cu succes in Deposit!\n");

                WaitForSingleObject(event_errors, INFINITE);
                printf("Pot folosi resursele partajate\n");

                HANDLE hError = CreateFile("C:\Facultate\CSSO\Week4\Reports\Summary\errors.txt", GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hError == INVALID_HANDLE_VALUE) {
                    cout << "Eroare la crearea/deschiderea fisierului errors.txt " << GetLastError() << '\n';
                    return 0;
                }

                string de_scris = "S-a încercat adăugarea produsului ";
                de_scris += id_produs;
                de_scris += "pe raftul ";
                de_scris += shelve_id;
                de_scris += "care este deja ocupat de ";
                de_scris += shelves[shelve_id];
                de_scris += ".\n";
                DWORD bytesWritten;

                if(!WriteFile(hError, &de_scris, sizeof(de_scris), &bytesWritten, NULL)) {
                    printf("Eroare la scrierea in fisier. Cod de eroare: %d\n", GetLastError());
                    CloseHandle(hError);
                    return 0;
                }

                cout << "Am scris cu succes in fisier" << '\n';

                SetEvent(event_errors);
                CloseHandle(event_errors);
                CloseHandle(hError);
            }

            else {
                
                //cout << "Este 0xFFFFFFFF" << '\n';
                HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, "deschidere_fisiere_mapate");
                if (mutex == INVALID_HANDLE_VALUE) {
                    printf("Eroare la deschiderea mutexului. \n");
                    CloseHandle(mutex);
                    return 0;
                }
                else printf("Mutex deschis cu succes in Deposit!\n");

                WaitForSingleObject(mutex, INFINITE);
                printf("Pot folosi resursele partajate\n");

                shelves[shelve_id] = id_produs;
                valability[id_produs] = expires_in;
                prices[id_produs] = product_price;
                
                ReleaseMutex(mutex);
                CloseHandle(mutex);

                CloseHandle(hPrices);
                CloseHandle(hValability);
                CloseHandle(hShelves);
                
                HANDLE event_logs = OpenEvent(SYNCHRONIZE, FALSE, "event_logs.txt");
                if (event_logs == INVALID_HANDLE_VALUE) {
                    printf("Eroare la deschiderea eventului. \n");
                    CloseHandle(event_logs);
                    return 0;
                }
                else printf("Event deschis cu succes in Deposit!\n");

                WaitForSingleObject(event_logs, INFINITE);
                printf("Pot folosi resursele partajate\n");

                HANDLE hLogs = CreateFile("C:\\Facultate\\CSSO\\Week4\Reports\\logs.txt", GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hLogs == INVALID_HANDLE_VALUE) {
                    cout << "Eroare la crearea/deschiderea fisierului errors.txt " << GetLastError() << '\n';
                    return 0;
                }

                string de_scris = "Am adăugat pe raftul ";
                de_scris += shelve_id;
                de_scris += " produsul ";
                de_scris += id_produs;
                de_scris += "ce are o valabilitate de ";
                de_scris += expires_in;
                de_scris += "zile și un preț de ";
                de_scris += product_price;
                de_scris += ".\n";
                DWORD bytesWritten;

                if (!WriteFile(hLogs, &de_scris, sizeof(de_scris), &bytesWritten, NULL)) {
                    printf("Eroare la scrierea in fisier. Cod de eroare: %d\n", GetLastError());
                    CloseHandle(hLogs);
                    return 0;
                }

                cout << "Am scris cu succes in fisier" << '\n';

                SetEvent(event_logs);
                CloseHandle(event_logs);
                CloseHandle(hLogs);
            }
        }
    }

    
}
