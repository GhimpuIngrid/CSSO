#include <windows.h>

#include <stdio.h>

#include <string>

#include <ctime>



void createDirectoriesAndFiles() {

    // Creare directoare

    if (!CreateDirectory(L"C:\\Facultate\\CSSO\\Laboratoare\\Week2\\InstalledSoftware", NULL) &&

        GetLastError() != ERROR_ALREADY_EXISTS) {

        fprintf(stderr, "Error creating directory 1\n");

        return;

    }



    if (!CreateDirectory(L"C:\\Facultate\\CSSO\\Laboratoare\\Week2\\Rezultate", NULL) &&

        GetLastError() != ERROR_ALREADY_EXISTS) {

        fprintf(stderr, "Error creating directory 2\n");

        return;

    }



    // Creare fișiere HKLM.txt, HKCC.txt, HKCU.txt

    HKEY hKeys[] = { HKEY_LOCAL_MACHINE, HKEY_CLASSES_ROOT, HKEY_CURRENT_USER };

    const wchar_t* hKeyNames[] = { L"HKLM.txt", L"HKCC.txt", L"HKCU.txt" };



    for (int i = 0; i < 3; i++) {

        HKEY hive;

        if (RegOpenKeyExW(hKeys[i], L"Software", 0, KEY_READ, &hive) == ERROR_SUCCESS) {

            DWORD subKeys, maxSubKeyLen;

            FILETIME lastWriteTime;

            RegQueryInfoKey(hive, NULL, NULL, NULL, &subKeys, &maxSubKeyLen, NULL, NULL, NULL, NULL, NULL, &lastWriteTime);



            FILE* file = _wfopen(L"C:\\Facultate\\CSSO\\Laboratoare\\Week2\\Rezultate\\" + hKeyNames[i], L"w");

            if (file) {

                fprintf(file, "lpcSubKeys: %d\n", subKeys);

                fprintf(file, "lpcMaxSubKeyLen: %d\n", maxSubKeyLen);



                SYSTEMTIME st;

                FileTimeToSystemTime(&lastWriteTime, &st);

                fprintf(file, "lpftLastWriteTime: %04d/%02d/%02d %02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);



                fclose(file);

            }

            RegCloseKey(hive);

        }

    }



    // Creare fișier sumar.txt

    FILE* summary = _wfopen(L"C:\\Facultate\\CSSO\\Laboratoare\\Week2\\Rezultate\\sumar.txt", L"w");

    if (summary) {

        for (int i = 0; i < 3; i++) {

            WIN32_FILE_ATTRIBUTE_DATA fileData;

            if (GetFileAttributesExW((L"C:\\Facultate\\CSSO\\Laboratoare\\Week2\\Rezultate\\" + hKeyNames[i]).c_str(), GetFileExInfoStandard, &fileData)) {

                fprintf(summary, "%ls %d\n", (L"C:\\Facultate\\CSSO\\Laboratoare\\Week2\\Rezultate\\" + hKeyNames[i]).c_str(), fileData.nFileSizeLow);

            }

        }

        fclose(summary);

    }



    // Creare fișiere în directorul Week3\InstalledSoftware

    if (!CreateDirectory(L"C:\\Facultate\\CSSO\\Laboratoare\\Week3\\InstalledSoftware", NULL) &&

        GetLastError() != ERROR_ALREADY_EXISTS) {

        fprintf(stderr, "Error creating directory 3\n");

        return;

    }



    // Creați fișiere corespunzătoare cheilor găsite în HKCU\Software

    HKEY hkcuSoftware;

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software", 0, KEY_READ, &hkcuSoftware) == ERROR_SUCCESS) {

        wchar_t subKeyName[MAX_PATH];

        DWORD subKeyIndex = 0;

        while (RegEnumKeyExW(hkcuSoftware, subKeyIndex, subKeyName, &MAX_PATH, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {

            FILE* file = _wfopen(L"C:\\Facultate\\CSSO\\Laboratoare\\Week3\\InstalledSoftware\\" + std::wstring(subKeyName) + L".txt", L"w");

            if (file) {

                fclose(file);

            }

            subKeyIndex++;

        }

        RegCloseKey(hkcuSoftware);

    }



    // Creați cheia de registru HKCU\Software\CSSO\Week2

    HKEY cssoWeek2;

    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\CSSO\\Week2", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &cssoWeek2, NULL) == ERROR_SUCCESS) {

        std::wstring value1 = L"C:\\Facultate\\CSSO\\Laboratoare\\Week3\\InstalledSoftware";

        RegSetValueExW(cssoWeek2, L"Path", 0, REG_SZ, (BYTE*)value1.c_str(), (value1.size() + 1) * sizeof(wchar_t));



        DWORD value2 = 3;

        RegSetValueExW(cssoWeek2, L"FileCount", 0, REG_DWORD, (BYTE*)&value2, sizeof(DWORD));



        RegCloseKey(cssoWeek2);

    }

    else {

        fprintf(stderr, "Error creating registry key\n");

    }

}



int main() {

    createDirectoriesAndFiles();

    return 0;

}