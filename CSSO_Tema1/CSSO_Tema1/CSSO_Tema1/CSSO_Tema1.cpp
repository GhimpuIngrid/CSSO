#include <windows.h>
#include <stdio.h>
#include<iostream>
#include<cstring>

using namespace std;

//cuvantul citit de la tastatura
char word[100];

//directorul citit de la tastatura
char directory_name[1000];

//functie de creare a directoarelor
void Create_A_Directory(const char path[]) {

    if (!CreateDirectory(path, NULL)){
        
        printf("Failed to create directory: %s. Cod eroare %d.\n", path, GetLastError());
    }

    else printf("%s directory was successfully created. \n", path);

}

int main() {

    Create_A_Directory("C:\\Facultate");
    Create_A_Directory("C:\\Facultate\\CSSO");
    Create_A_Directory("C:\\Facultate\\CSSO\\Laboratoare");
    Create_A_Directory("C:\\Facultate\\CSSO\\Laboratoare\\Week1");
    Create_A_Directory("C:\\Facultate\\CSSO\\Laboratoare\\Week1\\Rezultate");

    cin >> word;
    cin >> directory_name;

    //adaugam la path-ul primit de la tastatura "\\*.txt" pentru a putea itera prin toate fisierele text din acest director
    strcat_s(directory_name, "\\*.txt");

    //variabila in care retinem datele fisierului gasit prin FindFirstFile
    WIN32_FIND_DATA FileData;

    //cautam primul fisier din directorul citit
    HANDLE search = FindFirstFile(directory_name, &FileData);

    if (search == INVALID_HANDLE_VALUE){
        
        printf("No text files found.\n");
        
        return 0;
    }

    //exista fisiere
    else {
        
        HANDLE fisier_scriere = CreateFile("C:\\Facultate\\CSSO\\Laboratoare\\Week1\\Rezultate", GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fisier_scriere == INVALID_HANDLE_VALUE){

            printf("Am o eroare la deschiderea fisierului. Cod 	eroare: %d\n", GetLastError());

            return 0;
        }
        
        do {

            char myBuffer[20000];
            memset(myBuffer, 0, FileData.nFileSizeLow);
            DWORD bytesCititi;

            HANDLE fisier_citire = CreateFile(FileData.cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
           
            if (fisier_citire == INVALID_HANDLE_VALUE){
                
                printf("Am o eroare la deschiderea fisierului. Cod 	eroare: %d\n", GetLastError());

                return 0;
            }

            else if (ReadFile(fisier_citire, myBuffer, FileData.nFileSizeLow, &bytesCititi, NULL) == 0){
                    
                printf("Am o eroare la ReadFile!! Cod eroare %d\n", GetLastError());
                CloseHandle(fisier_citire);
                return 0;

            }

            printf("Am reusit sa citesc din fisier %d caractere.\n", bytesCititi);

            char aux[100];
            int length, poz = 0;

            //iteram prin myBuffer
            for (int i = 0; i != FileEndOfFileInfo || i > 16384; i++){

                aux[0] = '\0';
                length = 0;
                //punem in aux fiecare cuvant
                while (myBuffer[i] != ' ') {
                    aux[length] = myBuffer[i];
                    length++;
                    i++;
                }

                //word = cuvantul citit de la tastatura
                if (strcmp(aux, word) == 0)
                {
                    int length_p = strlen(FileData.cFileName);
                    char poz_c[5];
                    
                    //transformam pozitia din int in char pentru a o putea scrie in fisier
                    _itoa_s(poz, poz_c, 10);
                    strcat_s(poz_c, "\n");
                    

                    if (WriteFile(fisier_scriere, FileData.cFileName, length_p, NULL, NULL) == 0){

                        printf("Eroare la scriere in fisier. Cod eroare: %d\n", GetLastError());
                        CloseHandle(fisier_scriere);
                        return 0;
                    }
                    

                    else if (WriteFile(fisier_scriere, poz_c, length_p, NULL, NULL) == 0) {
                       
                        printf("Eroare la scriere in fisier. Cod eroare: %d\n", GetLastError());
                        CloseHandle(fisier_scriere);
                        return 0;
                    }
                }

                poz++;
            }

            CloseHandle(fisier_citire);
            
            if (FindNextFile(directory_name, &FileData) == 0)
            {
                printf("Am terminat de iterat prin toate fisierele\n");
                break;
            }

        } while (1);

        CloseHandle(fisier_scriere);
        CloseHandle(search);

    }

	system("pause");
	return 0;
}