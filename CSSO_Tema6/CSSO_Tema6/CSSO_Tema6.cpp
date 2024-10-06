#include <iostream>
#include<Windows.h>
#include<TlHelp32.h>
#include <windowsx.h>
#include <string.h>
#include <tchar.h>
#include <conio.h>
#include <aclapi.h>
#include <sddl.h>
#include <stdio.h>
#include<Lmcons.h>
#include <commctrl.h>
#include <wingdi.h>
#include <aclapi.h>
#include <string>
LPSTR info = NULL; LPSTR headerInfo = NULL, imagePath = NULL;
char szFileName[MAX_PATH] = "";
double times[6];
int row_index = -1;
int nop, noi, it_used, it_left;
HBITMAP image;

static TCHAR szWindowClass[] = ("DesktopApp");
static TCHAR szTitle[] = ("APP");
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HANDLE openFile()
{
    HANDLE hFile = CreateFile("C:\\Facultate\\CSSO\\Week6\\info.txt", FILE_APPEND_DATA, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Nu s-a putut crea fisierul info.txt. Cod eroare %d\n", GetLastError());
        exit(1);
    }
    return hFile;
}
HANDLE hFile = openFile();
void writeInfo(LPSTR buffer)
{
    DWORD bytesWritten;
    printf(buffer);
    printf("\n");
    WriteFile(hFile, buffer, strlen(buffer), &bytesWritten, NULL);
}
void GrantReadAccessToCurrentUser(LPSTR filePath) {
    PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;


    if (GetNamedSecurityInfo(filePath, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
        NULL, NULL, NULL, NULL, &pSecurityDescriptor) != ERROR_SUCCESS) {
        MessageBox(NULL, "Eroare la getNamedSecurityInfo", "Eroare", MB_OK | MB_ICONERROR);
        return;
    }

    EXPLICIT_ACCESS explicitAccess;
    ZeroMemory(&explicitAccess, sizeof(EXPLICIT_ACCESS));

    explicitAccess.grfAccessPermissions = GENERIC_READ;
    explicitAccess.grfAccessMode = SET_ACCESS;
    explicitAccess.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    explicitAccess.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
    explicitAccess.Trustee.TrusteeType = TRUSTEE_IS_USER;
    LPTSTR user = NULL; user = (LPTSTR)malloc(100); memset(user, 0, 100);
    strcpy(user, "CURRENT_USER");
    explicitAccess.Trustee.ptstrName = user;

    PACL pAcl = NULL; DWORD cbAcl = 0;
    cbAcl = (cbAcl + (sizeof(DWORD) - 1)) & 0xfffffffc;

    pAcl = (ACL*)LocalAlloc(LPTR, cbAcl);
    if (InitializeAcl(pAcl, cbAcl, ACL_REVISION) != ERROR_SUCCESS) {
        return;
    }
    if (SetEntriesInAcl(1, &explicitAccess, NULL, &pAcl) != ERROR_SUCCESS) {

        MessageBox(NULL, "Eroare la setEntriesInACL", "Eroare", MB_OK | MB_ICONERROR);
        LocalFree(pSecurityDescriptor);
        return;
    }


    if (SetNamedSecurityInfo(filePath, SE_FILE_OBJECT, PROTECTED_DACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
        NULL, NULL, pAcl, NULL) != ERROR_SUCCESS) {
        MessageBox(NULL, "Eroare la setNamedSecurityInfo", "Eroare", MB_OK | MB_ICONERROR);
        LocalFree(pSecurityDescriptor);
        LocalFree(pAcl);
        return;
    }


    LocalFree(pSecurityDescriptor);
    LocalFree(pAcl);
}
LPSTR  printSID(LPCSTR accountName)
{
    PSID sid;
    LPTSTR domainName;
    LPTSTR sidString = NULL;
    DWORD cbSid = 0;
    DWORD cchDomainName = 0;
    SID_NAME_USE peUse;

    LPSTR buffer = (LPSTR)malloc(5000); memset(buffer, 0, 5000);

    LookupAccountName(NULL, accountName, NULL, &cbSid, NULL, &cchDomainName, &peUse);

    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        sid = (PSID)malloc(cbSid);
        domainName = (LPTSTR)malloc(cchDomainName * sizeof(TCHAR));

        if (LookupAccountName(NULL, accountName, sid, &cbSid, domainName, &cchDomainName, &peUse)) {
            if (ConvertSidToStringSid(sid, &sidString)) {
                /*printf("%s SID: %s\n", accountName, sidString);*/
                sprintf(buffer, "%s SID: %s\n", accountName, sidString);
                LocalFree(sidString);
            }
        }
        else {
            printf("Eroare la obtinere SID pentru %s. Eroare %d\n", accountName, GetLastError());
            exit(1);
        }

        free(sid);
    }
    else {
        printf("Eroare la obtinerea dimensiunii SID-ului pentru: %s. Eroare %d\n", accountName, GetLastError());
        exit(1);
    }
    writeInfo(buffer);
    return buffer;
    /*free(buffer);*/
}
LPSTR getLogicalProcessorInfo()
{
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = NULL;
    LPSTR buffer2 = (LPSTR)malloc(5000); memset(buffer2, 0, 5000);
    DWORD bufferSize = 0;
    if (!GetLogicalProcessorInformation(buffer, &bufferSize) && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bufferSize);

        if (GetLogicalProcessorInformation(buffer, &bufferSize)) {
            for (DWORD i = 0; i < bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++) {
                sprintf(buffer2, "ProcessorMask: %d\n", buffer[i].ProcessorMask);
                LPSTR buffer3 = (LPSTR)malloc(500); memset(buffer3, 0, 500);
                sprintf(buffer3, "Relationship: %d\n", buffer[i].Relationship);
                /* printf("ProcessorMask: %d\n", buffer[i].ProcessorMask);*/
                 /*printf("Relationship: %d\n", buffer[i].Relationship);*/
                free(buffer3);
            }
        }
        else {
            printf("Eroare la obținerea informațiilor despre procesor. Cod eroare %d\n", GetLastError());
            exit(1);
        }
        free(buffer);
    }
    else {
        printf("Eroare la determinarea dimensiunii buffer-ului. Cod eroare %d\n", GetLastError());
        exit(1);
    }
    writeInfo(buffer2);
    return buffer2;
    /*free(buffer2);*/
}
LPSTR numaApi()
{
    LPSTR buffer = (LPSTR)malloc(5000); memset(buffer, 0, 5000);

    DWORD_PTR lpProcessAffinityMask, lpSystemAffinityMask;
    GetProcessAffinityMask(GetCurrentProcess(), &lpProcessAffinityMask, &lpSystemAffinityMask);

    DWORD highestNodeNumber = 0;
    GetNumaHighestNodeNumber(&highestNodeNumber);

    int totalNumaNodes = 0;
    for (DWORD node = 0; node <= highestNodeNumber; ++node) {
        DWORD_PTR lpNodeProcessorMask = 0;
        if (GetNumaNodeProcessorMask(node, &lpNodeProcessorMask)) {
            totalNumaNodes++;
        }
    }
    sprintf(buffer, "Masca de afinitate a procesului: 0x%Ix\n", lpProcessAffinityMask);
    LPSTR buffer2 = (LPSTR)malloc(500); memset(buffer2, 0, 500);
    sprintf(buffer2, "Masca de afinitate a sistemului: 0x%Ix\n", lpSystemAffinityMask);
    strcat(buffer, buffer2);
    memset(buffer2, 0, 500);
    sprintf(buffer2, "Numarul total de noduri NUMA: %d\n", totalNumaNodes);
    strcat(buffer, buffer2);
    free(buffer2);
    writeInfo(buffer);
    return buffer;
    /* free(buffer);*/
     /*printf("Masca de afinitate a procesului: 0x%Ix\n", lpProcessAffinityMask);*/
     /*printf("Masca de afinitate a sistemului: 0x%Ix\n", lpSystemAffinityMask);*/
    /* printf("Numarul total de noduri NUMA: %d\n", totalNumaNodes);*/
}
LPSTR cpuSets(int& number_of_proccessors)
{
    DWORD count = 0;
    DWORD* cpuSetIds = (DWORD*)malloc(1000);
    LPSTR buffer = (LPSTR)malloc(5000); memset(buffer, 0, 5000);

    if (GetProcessDefaultCpuSets(GetCurrentProcess(), cpuSetIds, 10000, &count))
    {

        /* printf("Numarul total de element din CpuSetIds: %d\n", count);*/
        sprintf(buffer, "Numarul total de elemente din CpuSetIds: %d\n", count);
        for (DWORD i = 0; i < count; ++i) {
            LPSTR buffer2 = (LPSTR)malloc(1000); memset(buffer, 0, 1000);
            if (i == 0) {
                /* printf("Elementele din CpuSetIds  sunt:\n");*/
                strcpy(buffer2, "Elementele din CpuSetIds:\n");
            }
            /* printf("%d", cpuSetIds[i]);*/
            sprintf(buffer2, "%d\n", cpuSetIds[i]);
            strcat(buffer, buffer2);
            free(buffer2);
        }
        /* printf("\n");*/
        free(cpuSetIds);
    }
    else {
        printf("Eroare la obtinerea informatiilor din CpuSets. Cod eroare %d\n", GetLastError());
        exit(1);
    }
    ULONG returnLength = 0;
    SYSTEM_CPU_SET_INFORMATION* cpuSetInfo = (SYSTEM_CPU_SET_INFORMATION*)malloc(1000);
    if (GetSystemCpuSetInformation(cpuSetInfo, 1000, &returnLength, 0, 0))
    {
        LPSTR buffer2 = (LPSTR)malloc(1000); memset(buffer2, 0, 1000);
        number_of_proccessors = returnLength / sizeof(SYSTEM_CPU_SET_INFORMATION);

        nop = returnLength / sizeof(SYSTEM_CPU_SET_INFORMATION);

        /*std::string numberop = std::to_string(nop);
        LPCSTR lpcstrValue = numberop.c_str();

        MessageBox(NULL, lpcstrValue, _T("Valoarea nop calculata"), MB_OK);*/

        sprintf(buffer2, "Numarul de structuri SYSTEM_CPU_SET_INFORMATION: %lu\n", returnLength / sizeof(SYSTEM_CPU_SET_INFORMATION));
        strcat(buffer, buffer2);
        free(buffer2);
        /*printf("Numarul de structuri SYSTEM_CPU_SET_INFORMATION: %lu\n", returnLength / sizeof(SYSTEM_CPU_SET_INFORMATION));*/

        for (ULONG i = 0; i < returnLength / sizeof(SYSTEM_CPU_SET_INFORMATION); ++i) {

            /* printf("Set %lu: . Id: %lu . Group: %lu.\n", i + 1, cpuSetInfo[i].CpuSet.Id, cpuSetInfo[i].CpuSet.Group);*/
            LPSTR buffer2 = (LPSTR)malloc(1000); memset(buffer2, 0, 1000);
            sprintf(buffer2, "Set %lu: . Id: %lu . Group: %lu.\n", i + 1, cpuSetInfo[i].CpuSet.Id, cpuSetInfo[i].CpuSet.Group);
            strcat(buffer, buffer2);
            free(buffer2);
        }
        free(cpuSetInfo);
    }
    else {
        printf("Eroare la obtinerea informatiilor din SystemCpuSetInformation. Cod eroare %d\n", GetLastError());
        exit(1);

    }
    /* printf("%s",buffer);*/
    writeInfo(buffer);
    return buffer;
    /* free(buffer);*/
}
void printPCInfo(int& number_of_proccessors)
{
    LPSTR result = NULL; result = (LPSTR)malloc(5000); memset(result, 0, 5000);
    info = (LPSTR)malloc(10000); memset(info, 0, 10000);
    LPSTR currentUser = (LPSTR)malloc(UNLEN + 1);
    DWORD currentUserSize = UNLEN + 1;
    if (GetUserName(currentUser, &currentUserSize))
    {
        /* printf("Numele utilizatorului curent este: %s .\n", currentUser);*/
        result = printSID(currentUser);
    }
    else {
        printf("Eroare la obtinerea numelui utilizatorului curent. Cod eroare %d \n.", GetLastError());
    }
    strcpy(info, result); memset(result, 0, 5000);
    free(currentUser);
    result = printSID("Everyone");
    strcat(info, result); memset(result, 0, 5000);
    result = printSID("Administrator");
    strcat(info, result); memset(result, 0, 5000);
    result = getLogicalProcessorInfo();
    strcat(info, result); memset(result, 0, 5000);
    result = numaApi();
    strcat(info, result); memset(result, 0, 5000);
    result = cpuSets(number_of_proccessors);
    strcat(info, result); memset(result, 0, 5000);
}

void grayScale(HBITMAP hBitmap, int start, int finish, int ok) {

    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);


    BYTE* pPixels = (BYTE*)malloc(bm.bmWidthBytes * bm.bmHeight);
    if (!pPixels) {
        MessageBox(NULL, "Eroare la alocarea memoriei", "Eroare", MB_OK | MB_ICONERROR);
        return;
    }

    GetBitmapBits(hBitmap, bm.bmWidthBytes * bm.bmHeight, pPixels);

    if (ok == 0)
        finish = bm.bmWidthBytes * bm.bmHeight;

    for (int i = start; i < finish; i += 4) {

        BYTE gray = (BYTE)(0.299 * pPixels[i + 2] + 0.587 * pPixels[i + 1] + 0.114 * pPixels[i]);
        pPixels[i] = gray;
        pPixels[i + 1] = gray;
        pPixels[i + 2] = gray;
    }
    SetBitmapBits(hBitmap, bm.bmWidthBytes * bm.bmHeight, pPixels);
    free(pPixels);
}
void inversare(HBITMAP hBitmap, int start, int finish, int ok) {

    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);


    BYTE* pPixels = (BYTE*)malloc(bm.bmWidthBytes * bm.bmHeight);
    if (!pPixels) {
        MessageBox(NULL, "Eroare la alocarea memoriei", "Eroare", MB_OK | MB_ICONERROR);
        return;
    }

    GetBitmapBits(hBitmap, bm.bmWidthBytes * bm.bmHeight, pPixels);

    if (ok == 0)
        finish = bm.bmWidthBytes * bm.bmHeight;

    for (int i = start; i < finish; i += 4) {

        pPixels[i] = 0xFF - pPixels[i];
        pPixels[i + 1] = 0xFF - pPixels[i + 1];
        pPixels[i + 2] = 0xFF - pPixels[i + 2];
    }
    SetBitmapBits(hBitmap, bm.bmWidthBytes * bm.bmHeight, pPixels);
    free(pPixels);
}

////////////////////////////////////////////////////////////////////////

void Calculate_Number_of_iterations(HBITMAP img) {
    BITMAP bm;
    GetObject(img, sizeof(BITMAP), &bm);


    BYTE* pPixels = (BYTE*)malloc(bm.bmWidthBytes * bm.bmHeight);
    if (!pPixels) {
        MessageBox(NULL, "Eroare la alocarea memoriei", "Eroare", MB_OK | MB_ICONERROR);
        return;
    }

    GetBitmapBits(img, bm.bmWidthBytes * bm.bmHeight, pPixels);

    noi = bm.bmWidthBytes * bm.bmHeight;

}


struct ThreadParameters {
    int threadNumber;
    int NumberOfCalc;
    int NumberOfWorkers;
    HBITMAP img;
    int op;
    int number_of_iter;
};

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    ThreadParameters* params = static_cast<ThreadParameters*>(lpParam);
    int threadNumber = params->threadNumber;
    int NumberOfCalc = params->NumberOfCalc;
    int NumberOfWorkers = params->NumberOfWorkers;
    int op = params->op;

    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, "MyMutex");
    WaitForSingleObject(hMutex, INFINITE);

    HBITMAP image = params->img;

    if (threadNumber == NumberOfWorkers - 1) {
        if (op == 0) grayScale(image, threadNumber * NumberOfCalc, 0, 0);
        else inversare(image, threadNumber * NumberOfCalc, 0, 0);
    }

    else {
        if (op == 0) grayScale(image, threadNumber * NumberOfCalc, (threadNumber + 1) * NumberOfCalc - 1, 1);
        else inversare(image, threadNumber * NumberOfCalc, (threadNumber + 1) * NumberOfCalc - 1, 1);
    }

    //Sleep(100000);

    delete params; // Clean up the allocated structure.

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    return 0;
}

void Calc_static(int nop, int noi, HBITMAP hBitmap, int op) {

    /*std::string numberop = std::to_string(noi);
    LPCSTR lpcstrValue = numberop.c_str();

    MessageBox(NULL, lpcstrValue, _T("Valoarea noi primita in Calc_static"), MB_OK);*/

    const int nWorkers = nop * 2;

    HANDLE hThreads[1000];
    DWORD dwThreadIds[1000];

    HANDLE hMutex = CreateMutex(NULL, FALSE, "MyMutex");

    for (int i = 0; i < nWorkers; i++) {
        ThreadParameters* params = new ThreadParameters;
        params->threadNumber = i;
        params->NumberOfCalc = noi / nWorkers;
        params->NumberOfWorkers = nWorkers;
        params->img = hBitmap;
        params->op = op;


        hThreads[i] = CreateThread(NULL, 0, ThreadFunction, params, 0, &dwThreadIds[i]);

        if (hThreads[i] == NULL) {
            printf("Eroare la crearea thred ului %d\n", GetLastError());
        }
    }

    WaitForMultipleObjects(nWorkers, hThreads, TRUE, INFINITE);

    for (int i = 0; i < nWorkers; ++i) {
        CloseHandle(hThreads[i]);
    }

    CloseHandle(hMutex);
}


int Calculate_Chunks(int nWorkers) {

    // MessageBox(NULL, _T("Sunt in Calculate_Chunks"), _T("Valoarea noi primita in Calc_static"), MB_OK);

    if (it_left == 0) {

        //MessageBox(NULL, _T("Nu mai am iteratii"), _T("Valoarea noi primita in Calc_static"), MB_OK);
        return -1;

    }

    else {

        //MessageBox(NULL, _T("Mai am iteratii"), _T("Valoarea noi primita in Calc_static"), MB_OK);

        int chunk = it_left / (2 * nWorkers);
        if (chunk == 0) {
            it_left = 0;
            it_used += it_left;
            return it_left;
        }
        else if (chunk % 4 == 0)
        {
            //std::string numberop = std::to_string(chunk);
            //LPCSTR lpcstrValue = numberop.c_str();

           // MessageBox(NULL, lpcstrValue, _T("Valoarea chunku-lui"), MB_OK);
            it_left -= chunk;
            it_used += chunk;
            return chunk;
        }

        else {
            while (chunk % 4 != 0)
                chunk++;

            //std::string numberop = std::to_string(chunk);
            //LPCSTR lpcstrValue = numberop.c_str();

            //MessageBox(NULL, lpcstrValue, _T("Valoarea chunku-lui"), MB_OK);
            it_left -= chunk;
            it_used += chunk;
            return chunk;
        }
    }
}


DWORD WINAPI ThreadFunction2(LPVOID lpParam) {
    ThreadParameters* params = static_cast<ThreadParameters*>(lpParam);
    int threadNumber = params->threadNumber;
    int NumberOfWorkers = params->NumberOfWorkers;
    int op = params->op;
    int NumberOfCalc = 0;

    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, "MyMutex");
    HANDLE getChunk = OpenMutex(SYNCHRONIZE, FALSE, "getChunk");

    WaitForSingleObject(hMutex, INFINITE);

    HBITMAP image = params->img;

    WaitForSingleObject(getChunk, INFINITE);
    NumberOfCalc = Calculate_Chunks(NumberOfWorkers);
    ReleaseMutex(getChunk);

    do {


        if (op == 0)
            grayScale(image, it_used - NumberOfCalc, it_used - 1, 1);
        else
            inversare(image, it_used - NumberOfCalc, it_used - 1, 1);

        ReleaseMutex(hMutex);

        WaitForSingleObject(getChunk, INFINITE);
        NumberOfCalc = Calculate_Chunks(NumberOfWorkers);
        ReleaseMutex(getChunk);

    } while (NumberOfCalc != -1);

    delete params;
    CloseHandle(getChunk);
    CloseHandle(hMutex);
    return 0;
}

void Calc_dinamic(int nop, int noi, HBITMAP hBitmap, int op) {

    int nWorkers = nop * 2;
    //int chunksize = noi / (2 * nWorkers);
    it_left = noi;

    HANDLE hThreads[1000];
    DWORD dwThreadIds[1000];

    HANDLE hMutex = CreateMutex(NULL, FALSE, "MyMutex");
    HANDLE getChunk = CreateMutex(NULL, FALSE, "getChunk");

    MessageBox(NULL, _T("Sunt in Calc_dinamic si am creat mutex"), _T("Valoarea noi primita in Calc_static"), MB_OK);

    for (int i = 0; i < nWorkers; i++) {
        ThreadParameters* params = new ThreadParameters;
        params->threadNumber = i;
        params->NumberOfWorkers = nWorkers;
        params->img = hBitmap;
        params->op = op;


        hThreads[i] = CreateThread(NULL, 0, ThreadFunction2, params, 0, &dwThreadIds[i]);

        if (hThreads[i] == NULL) {
            printf("Eroare la crearea thred ului %d\n", GetLastError());
        }
    }

    WaitForMultipleObjects(nWorkers, hThreads, TRUE, INFINITE);

    MessageBox(NULL, _T("Am terminat thread urile"), _T("Valoarea noi primita in Calc_static"), MB_OK);

    for (int i = 0; i < nWorkers; ++i) {
        CloseHandle(hThreads[i]);
    }

    CloseHandle(getChunk);
    CloseHandle(hMutex);

}
//////////////////////////////////////////////////////////////

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
        MessageBox(NULL, "GetObject", "Eroare", MB_OK | MB_ICONERROR);
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    if (cClrBits < 24)
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
            sizeof(BITMAPINFOHEADER) +
            sizeof(RGBQUAD) * (1 << cClrBits));

    else
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
            sizeof(BITMAPINFOHEADER));

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1 << cClrBits);


    pbmi->bmiHeader.biCompression = BI_RGB;


    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
        * pbmi->bmiHeader.biHeight;
    pbmi->bmiHeader.biClrImportant = 0;
    return pbmi;
}

void CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi,
    HBITMAP hBMP, HDC hDC)
{
    HANDLE hf;
    BITMAPFILEHEADER hdr;
    PBITMAPINFOHEADER pbih;
    LPBYTE lpBits;
    DWORD dwTotal;
    DWORD cb;
    BYTE* hp;
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER)pbi;
    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
        MessageBox(NULL, "GloballAlloc", "Eroare", MB_OK | MB_ICONERROR);
    if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
        DIB_RGB_COLORS))
    {
        MessageBox(NULL, "GetDIBits", "Eroare", MB_OK | MB_ICONERROR);
    }
    hf = CreateFile(pszFile, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
    if (hf == INVALID_HANDLE_VALUE)
        MessageBox(NULL, "CreateFile", "Eroare", MB_OK | MB_ICONERROR);
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  

    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD);

    if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
        (LPDWORD)&dwTmp, NULL))
    {
        MessageBox(NULL, "writeFile", "Eroare", MB_OK | MB_ICONERROR);
    }


    if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
        + pbih->biClrUsed * sizeof(RGBQUAD),
        (LPDWORD)&dwTmp, (NULL)))
        MessageBox(NULL, "writeFile", "Eroare", MB_OK | MB_ICONERROR);


    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL))
        MessageBox(NULL, "writeFile", "Eroare", MB_OK | MB_ICONERROR);

    if (!CloseHandle(hf))
        MessageBox(NULL, "closeHandle", "Eroare", MB_OK | MB_ICONERROR);
    GrantReadAccessToCurrentUser(pszFile);
    GlobalFree((HGLOBAL)lpBits);
}
void resetTimes() {
    for (int i = 0; i < 6; i++)
        times[i] = 0;
}
HBITMAP LoadBMP(LPCSTR path) {

    /*  MessageBox(NULL, path, _T("Path-ul primit ca parametru"), MB_OK);*/


    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if (hBitmap == NULL) {
        DWORD errorCode = GetLastError();

        // Conversie a codului de eroare într-un șir de caractere
        LPTSTR errorMsg;
        if (FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errorMsg,
            0,
            NULL) == 0) {
            // Dacă FormatMessage eșuează, atunci construiește un mesaj simplu
            errorMsg = _tcsdup(_T("Cod de eroare necunoscut."));
        }

        // Instrucțiuni de depanare
        _tprintf(_T("Cod eroare: %lu\n"), errorCode);
        _tprintf(_T("Mesaj eroare: %s\n"), errorMsg);

        // Construirea mesajului cu codul de eroare
        TCHAR message[1000];
        _sntprintf_s(message, _countof(message), _T("Nu s-a putut încărca imaginea. %s\nPath: %s"), errorMsg, path);

        // Afișare MessageBox cu mesajul de eroare
        /*MessageBox(NULL, message, _T("Eroare"), MB_OK | MB_ICONERROR);*/

        // Eliberare a memoriei alocate pentru mesajul de eroare
        LocalFree(errorMsg);
    }
    resetTimes();
    ++row_index;
    return hBitmap;
}


void LoadImage(const char* imagePath, HWND hImage) {
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    /* SendMessage(hImage, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);*/
}
void OpenF(HWND hImage) {
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Imagini (*.bmp;*.jpg;*.png)\0*.bmp;*.jpg;*.png\0Toate fisierele (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = sizeof(szFileName);
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "bmp";

    if (GetOpenFileName(&ofn)) {
        LoadImage(szFileName, hImage);
    }
}
void DisplayInfoHeaders(LPSTR filePath) {
    HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBoxA(NULL, "Eroare la deschiderea fisierului BMP.", "Eroare", MB_OK | MB_ICONERROR);
        return;
    }
    BITMAPFILEHEADER bmpFileHeader;
    DWORD bytesRead;
    ReadFile(hFile, &bmpFileHeader, sizeof(BITMAPFILEHEADER), &bytesRead, NULL);
    if (bytesRead != sizeof(BITMAPFILEHEADER)) {
        MessageBoxA(NULL, "Eroare la citirea Bitmap File Header.", "Eroare", MB_OK | MB_ICONERROR);
        CloseHandle(hFile);
        return;
    }
    BITMAPINFOHEADER bmpInfoHeader;
    ReadFile(hFile, &bmpInfoHeader, sizeof(BITMAPINFOHEADER), &bytesRead, NULL);
    if (bytesRead != sizeof(BITMAPINFOHEADER)) {
        MessageBoxA(NULL, "Eroare la citirea DIB Header.", "Eroare", MB_OK | MB_ICONERROR);
        CloseHandle(hFile);
        return;
    }
    CloseHandle(hFile);


    headerInfo = (LPSTR)malloc(5000); memset(headerInfo, 0, 5000);
    // Construiește șirul cu informații
    sprintf(headerInfo, "Bitmap File Header:\n"
        "Type: %c%c\n"
        "Size: %d bytes\n"
        "Reserved: %d\n"
        "Reserved: %d\n"
        "Offset to pixel array: %d bytes\n\n"
        "DIB Header:\n"
        "Size: %d bytes\n"
        "Width: %d pixels\n"
        "Height: %d pixels\n"
        "Planes: %d\n"
        "Bits per pixel: %d\n"
        "Compression: %d\n"
        "Image size: %d bytes\n"
        "X pixels per meter: %d\n"
        "Y pixels per meter: %d\n"
        "Colors used: %d\n"
        "Important colors: %d\n",
        bmpFileHeader.bfType & 0xFF, (bmpFileHeader.bfType >> 8) & 0xFF,
        bmpFileHeader.bfSize,
        bmpFileHeader.bfReserved1,
        bmpFileHeader.bfReserved2,
        bmpFileHeader.bfOffBits,
        bmpInfoHeader.biSize,
        bmpInfoHeader.biWidth,
        bmpInfoHeader.biHeight,
        bmpInfoHeader.biPlanes,
        bmpInfoHeader.biBitCount,
        bmpInfoHeader.biCompression,
        bmpInfoHeader.biSizeImage,
        bmpInfoHeader.biXPelsPerMeter,
        bmpInfoHeader.biYPelsPerMeter,
        bmpInfoHeader.biClrUsed,
        bmpInfoHeader.biClrImportant);
    strcat(headerInfo, "\nimagine: ");
    strcat(headerInfo, filePath);
}
void AddLine(LPSTR image, int row, int column, HWND hTable, HWND hWnd) {
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = row;
    lvItem.iSubItem = column;

    char buffer[50];
    if (column != 0)
    {
        sprintf(buffer, "%.2f", times[column - 1]);
        lvItem.pszText = buffer;
        ListView_InsertItem(hTable, &lvItem);
        ListView_SetItemText(hTable, row, column, buffer);
    }
    else
    {
        lvItem.pszText = image;
        ListView_InsertItem(hTable, &lvItem);
        ListView_SetItemText(hTable, row, column, image);
    }

}
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    int number_of_proccessors;
    printPCInfo(number_of_proccessors);
    CloseHandle(hFile);
    //loadImages();
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            ("Call to RegisterClassEx failed!"),
            ("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }


    hInst = hInstance;
    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1300, 500,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            ("Call to CreateWindow failed!"),
            ("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    free(info);
    return (int)msg.wParam;
}
HWND hButton1, hButton2, hButton3, hButton4, hButton5, hButton6, hButton7, hButton8, hButton9, hEdit1, hEdit2, hButton10;
HWND hImage, hTable;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    LPSTR text = NULL; text = (LPSTR)malloc(50); memset(text, 0, 50);
    switch (message)
    {
    case WM_CREATE:
        hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            300, 70, 400, 35, hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);
        SendMessage(hEdit1, EM_SETLIMITTEXT, (WPARAM)255, 0);
        SetWindowText(hEdit1, "Introduceti calea catre imagine");

        hButton1 = CreateWindow("BUTTON", "Aplica grayScale secvential", WS_CHILD | WS_VISIBLE,
            300, 130, 190, 25, hWnd, (HMENU)2, GetModuleHandle(NULL), NULL);
        hButton2 = CreateWindow("BUTTON", "Aplica Inversare secvential", WS_CHILD | WS_VISIBLE,
            510, 130, 190, 25, hWnd, (HMENU)4, GetModuleHandle(NULL), NULL);
        hButton3 = CreateWindow("BUTTON", "Aplica grayScale static", WS_CHILD | WS_VISIBLE,
            300, 170, 190, 25, hWnd, (HMENU)6, GetModuleHandle(NULL), NULL);
        hButton4 = CreateWindow("BUTTON", "Aplica Inversare static", WS_CHILD | WS_VISIBLE,
            510, 170, 190, 25, hWnd, (HMENU)8, GetModuleHandle(NULL), NULL);

        hButton5 = CreateWindow("BUTTON", "Aplica grayScale dinamic", WS_CHILD | WS_VISIBLE,
            300, 210, 190, 25, hWnd, (HMENU)10, GetModuleHandle(NULL), NULL);

        hButton6 = CreateWindow("BUTTON", "Aplica Inversare dinamic", WS_CHILD | WS_VISIBLE,
            510, 210, 190, 25, hWnd, (HMENU)12, GetModuleHandle(NULL), NULL);
        hButton7 = CreateWindow("BUTTON", "Informatii PC", WS_CHILD | WS_VISIBLE,
            170, 10, 190, 25, hWnd, (HMENU)13, GetModuleHandle(NULL), NULL);

        hButton8 = CreateWindow("BUTTON", "Incarca imagine", WS_CHILD | WS_VISIBLE,
            380, 10, 190, 25, hWnd, (HMENU)14, GetModuleHandle(NULL), NULL);

        hButton9 = CreateWindow("BUTTON", "Informatii headere", WS_CHILD | WS_VISIBLE,
            580, 10, 190, 25, hWnd, (HMENU)15, GetModuleHandle(NULL), NULL);

        hButton10 = CreateWindow("BUTTON", "Incarca imaginea", WS_CHILD | WS_VISIBLE,
            710, 70, 190, 35, hWnd, (HMENU)17, GetModuleHandle(NULL), NULL);

        hTable = CreateWindowEx(0, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT, 10, 250, 940, 300, hWnd, (HMENU)16, NULL, NULL);

        LVCOLUMN lvColumn;
        memset(&lvColumn, 0, sizeof(lvColumn));
        lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        strcpy(text, "Imagine");
        lvColumn.pszText = text;
        lvColumn.cx = 100;
        lvColumn.iSubItem = 0;
        ListView_InsertColumn(hTable, 0, &lvColumn);
        strcpy(text, "T.grayScale secvential");
        lvColumn.pszText = text;
        lvColumn.cx = 140;
        lvColumn.iSubItem = 1;
        ListView_InsertColumn(hTable, 1, &lvColumn);
        strcpy(text, "T.inversare secvential");
        lvColumn.pszText = text;
        lvColumn.cx = 140;
        lvColumn.iSubItem = 2;
        ListView_InsertColumn(hTable, 2, &lvColumn);
        strcpy(text, "T.grayScale static");
        lvColumn.pszText = text;
        lvColumn.cx = 140;
        lvColumn.iSubItem = 3;
        ListView_InsertColumn(hTable, 3, &lvColumn);
        strcpy(text, "T.inversare static");
        lvColumn.pszText = text;
        lvColumn.cx = 140;
        lvColumn.iSubItem = 4;
        ListView_InsertColumn(hTable, 4, &lvColumn);
        strcpy(text, "T.grayScale dinamic");
        lvColumn.pszText = text;
        lvColumn.cx = 140;
        lvColumn.iSubItem = 5;
        ListView_InsertColumn(hTable, 5, &lvColumn);
        strcpy(text, "T.inversare dinamic");
        lvColumn.pszText = text;
        lvColumn.cx = 140;
        lvColumn.iSubItem = 6;
        ListView_InsertColumn(hTable, 6, &lvColumn);

        free(text);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 2:
        {

            if (image) {
                LARGE_INTEGER start_time, end_time, frequency;
                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start_time);
                grayScale(image, 0, 0, 0);
                QueryPerformanceCounter(&end_time);
                double total_time = ((double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart) * 1e6;
                times[0] = total_time;
                AddLine(imagePath, row_index, 1, hTable, hWnd);
                HDC hdc = GetDC(hWnd);
                PBITMAPINFO pbmi = CreateBitmapInfoStruct(image);
                LPSTR path = NULL; path = (LPSTR)malloc(500); memset(path, 0, 500);
                strcpy(path, "C:\\Facultate\\CSSO\\Week6\\rezultate\\secvential\\");
                strcat(path, strrchr(imagePath, '\\') + 1);
                path[strlen(path) - 4] = '\0';
                strcat(path, "_grayScale_");
                LPSTR time = NULL; time = (LPSTR)malloc(50); memset(time, 0, 50);
                sprintf(time, "%.2f", total_time);
                strcat(path, time);
                strcat(path, ".bmp");
                CreateBMPFile(path, pbmi, image, hdc);
                ReleaseDC(hWnd, hdc);
                free(time);
                free(path);
            }


            break; }
        case 4:
        {
            if (image) {
                LARGE_INTEGER start_time, end_time, frequency;
                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start_time);
                inversare(image, 0, 0, 0);
                QueryPerformanceCounter(&end_time);
                double total_time = ((double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart) * 1e6;
                times[1] = total_time;
                AddLine(imagePath, row_index, 2, hTable, hWnd);
                HDC hdc = GetDC(hWnd);
                PBITMAPINFO pbmi = CreateBitmapInfoStruct(image);
                LPSTR path = NULL; path = (LPSTR)malloc(500); memset(path, 0, 500);
                strcpy(path, "C:\\Facultate\\CSSO\\Week6\\rezultate\\secvential\\");
                strcat(path, strrchr(imagePath, '\\') + 1);
                path[strlen(path) - 4] = '\0';
                strcat(path, "_inversare_");
                LPSTR time = NULL; time = (LPSTR)malloc(50); memset(time, 0, 50);
                sprintf(time, "%.2f", total_time);
                strcat(path, time);
                strcat(path, ".bmp");
                CreateBMPFile(path, pbmi, image, hdc);
                ReleaseDC(hWnd, hdc);
                free(time);
                free(path);
            }

            break;
        }

        case 6:
        {
            if (image) {
                Calculate_Number_of_iterations(image);
                LARGE_INTEGER start_time, end_time, frequency;
                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start_time);
                Calc_static(nop, noi, image, 0);
                QueryPerformanceCounter(&end_time);
                double total_time = ((double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart) * 1e6;
                times[2] = total_time;
                AddLine(imagePath, row_index, 3, hTable, hWnd);
                HDC hdc = GetDC(hWnd);
                PBITMAPINFO pbmi = CreateBitmapInfoStruct(image);
                LPSTR path = NULL; path = (LPSTR)malloc(500); memset(path, 0, 500);
                strcpy(path, "C:\\Facultate\\CSSO\\Week6\\rezultate\\static\\");
                strcat(path, strrchr(imagePath, '\\') + 1);
                path[strlen(path) - 4] = '\0';
                strcat(path, "_grayScale_");
                LPSTR time = NULL; time = (LPSTR)malloc(50); memset(time, 0, 50);
                sprintf(time, "%.2f", total_time);
                strcat(path, time);
                strcat(path, ".bmp");
                CreateBMPFile(path, pbmi, image, hdc);
                ReleaseDC(hWnd, hdc);
                free(time);
                free(path);
            }

            break; }

        case 8:
        {
            if (image) {
                Calculate_Number_of_iterations(image);
                LARGE_INTEGER start_time, end_time, frequency;
                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start_time);
                Calc_static(nop, noi, image, 1);
                QueryPerformanceCounter(&end_time);
                double total_time = ((double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart) * 1e6;
                times[3] = total_time;
                AddLine(imagePath, row_index, 4, hTable, hWnd);
                HDC hdc = GetDC(hWnd);
                PBITMAPINFO pbmi = CreateBitmapInfoStruct(image);
                LPSTR path = NULL; path = (LPSTR)malloc(500); memset(path, 0, 500);
                strcpy(path, "C:\\Facultate\\CSSO\\Week6\\rezultate\\static\\");
                strcat(path, strrchr(imagePath, '\\') + 1);
                path[strlen(path) - 4] = '\0';
                strcat(path, "_inversare_");
                LPSTR time = NULL; time = (LPSTR)malloc(50); memset(time, 0, 50);
                sprintf(time, "%.2f", total_time);
                strcat(path, time);
                strcat(path, ".bmp");
                CreateBMPFile(path, pbmi, image, hdc);
                ReleaseDC(hWnd, hdc);
                free(time);
                free(path);
            }

            break;
        }

        case 10:
        {
            if (image) {
                Calculate_Number_of_iterations(image);
                LARGE_INTEGER start_time, end_time, frequency;
                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start_time);
                Calc_dinamic(nop, noi, image, 0);
                QueryPerformanceCounter(&end_time);
                double total_time = ((double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart) * 1e6;
                times[4] = total_time;
                AddLine(imagePath, row_index, 5, hTable, hWnd);
                HDC hdc = GetDC(hWnd);
                PBITMAPINFO pbmi = CreateBitmapInfoStruct(image);
                LPSTR path = NULL; path = (LPSTR)malloc(500); memset(path, 0, 500);
                strcpy(path, "C:\\Facultate\\CSSO\\Week6\\rezultate\\dinamic\\");
                strcat(path, strrchr(imagePath, '\\') + 1);
                path[strlen(path) - 4] = '\0';
                strcat(path, "_grayScale_");
                LPSTR time = NULL; time = (LPSTR)malloc(50); memset(time, 0, 50);
                sprintf(time, "%.2f", total_time);
                strcat(path, time);
                strcat(path, ".bmp");
                CreateBMPFile(path, pbmi, image, hdc);
                ReleaseDC(hWnd, hdc);
                free(time);
                free(path);
            }

            break; }

        case 12:
        {
            if (image) {
                Calculate_Number_of_iterations(image);
                LARGE_INTEGER start_time, end_time, frequency;
                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start_time);
                Calc_dinamic(nop, noi, image, 1);
                QueryPerformanceCounter(&end_time);
                double total_time = ((double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart) * 1e6;
                times[5] = total_time;
                AddLine(imagePath, row_index, 6, hTable, hWnd);
                HDC hdc = GetDC(hWnd);
                PBITMAPINFO pbmi = CreateBitmapInfoStruct(image);
                LPSTR path = NULL; path = (LPSTR)malloc(500); memset(path, 0, 500);
                strcpy(path, "C:\\Facultate\\CSSO\\Week6\\rezultate\\dinamic\\");
                strcat(path, strrchr(imagePath, '\\') + 1);
                path[strlen(path) - 4] = '\0';
                strcat(path, "_inversare_");
                LPSTR time = NULL; time = (LPSTR)malloc(50); memset(time, 0, 50);
                sprintf(time, "%.2f", total_time);
                strcat(path, time);
                strcat(path, ".bmp");
                CreateBMPFile(path, pbmi, image, hdc);
                ReleaseDC(hWnd, hdc);
                free(time);
                free(path);
            }

            break;
        }
        case 13:
        {

            MessageBox(hWnd, info, "Informatii PC", MB_OK);
            break;
        }
        case 14:
        {
            OpenF(hImage);
            DisplayInfoHeaders(szFileName);
            break;
        }
        case 15: {
            MessageBox(hWnd, headerInfo, "Informatii Header", MB_OK);
            break;
        }
        case 17:
        {
            free(imagePath);
            imagePath = (LPSTR)malloc(MAX_PATH); memset(imagePath, 0, MAX_PATH);
            GetWindowText(hEdit1, imagePath, MAX_PATH);

            /*  MessageBox(NULL, imagePath, _T("Path-ul dat ca parametru"), MB_OK);*/

            image = LoadBMP(LPCSTR(imagePath));
            LPSTR imageName = NULL; imageName = (LPSTR)malloc(500); memset(imageName, 0, 500);
            strcpy(imageName, strrchr(imagePath, '\\') + 1);
            AddLine(imageName, row_index, 0, hTable, hWnd);
            if (image == NULL)
            {
                MessageBox(hWnd, "Nu s-a putut încărca imaginea.", "Eroare", MB_OK | MB_ICONERROR);
            }
        }
        break;
        }
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

//C:\\Facultate\\CSSO\\Week6\\date\\city.bmp