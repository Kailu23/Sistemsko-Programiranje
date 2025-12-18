#ifndef INC

#define INC 1

#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

#endif // !INC

#pragma comment(lib, "User32.lib")

void DisplayErrorBox(LPTSTR lpszFunction) {
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and clean up

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

int FindOldestFile(TCHAR* path, LPFILETIME currentOldest) {
    size_t length_of_arg;
    WIN32_FIND_DATA ffd;
    TCHAR szDir[3 * MAX_PATH];
    TCHAR rDir[3 * MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    FILETIME current;
    LPSYSTEMTIME systemTimeL = (LPSYSTEMTIME)malloc(sizeof(SYSTEMTIME));

    StringCchLength(path, 3 * MAX_PATH, &length_of_arg);

    if (length_of_arg > (3 * MAX_PATH - 3)) {
        _tprintf(TEXT("\nDirectory path is too long.\n"));
        _tprintf(TEXT("%s"), path);

        return (-1);
    }

    //_tprintf(TEXT("\nTarget directory is %s\n\n"), path);

    StringCchCopy(szDir, 3 * MAX_PATH, path);
    StringCchCat(szDir, 3 * MAX_PATH, TEXT("\\*"));

    //_tprintf(TEXT("\Directory is %s\n\n"), szDir);


    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        return dwError;
    }

    do {
        if (!strcmp(ffd.cFileName, ".")) continue;
        if (!strcmp(ffd.cFileName, "..")) continue;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            StringCchCopy(rDir, 3 * MAX_PATH, path);
            StringCchCat(rDir, 3 * MAX_PATH, TEXT("\\"));
            StringCchCat(rDir, 3 * MAX_PATH, ffd.cFileName);
            FindOldestFile(rDir, currentOldest);
        }
        else {
            current.dwHighDateTime = ffd.ftCreationTime.dwHighDateTime;
            current.dwLowDateTime = ffd.ftCreationTime.dwLowDateTime;
            if (current.dwHighDateTime < currentOldest->dwHighDateTime && current.dwLowDateTime < currentOldest->dwLowDateTime) {
                currentOldest->dwHighDateTime = current.dwHighDateTime;
                currentOldest->dwLowDateTime = current.dwLowDateTime;
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);



    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        DisplayErrorBox(TEXT("FindFirstFile"));
    }
    FindClose(hFind);

    return dwError;
}

int FindBiggestFiles(TCHAR* path) {
    size_t length_of_arg;
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER fileSize;
    TCHAR szDir[3 * MAX_PATH];
    TCHAR rDir[3 * MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    size_t bigFileCounter = 0;

    StringCchLength(path, 3 * MAX_PATH, &length_of_arg);

    if (length_of_arg > (3 * MAX_PATH - 3)) {
        _tprintf(TEXT("\nDirectory path is too long.\n"));
        _tprintf(TEXT("%s"), path);

        return 0;
    }

    //_tprintf(TEXT("\nTarget directory is %s\n\n"), path);

    StringCchCopy(szDir, 3 * MAX_PATH, path);
    StringCchCat(szDir, 3 * MAX_PATH, TEXT("\\*"));

    //_tprintf(TEXT("\Directory is %s\n\n"), szDir);


    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        return dwError;
    }

    do {
        if (!strcmp(ffd.cFileName, ".")) continue;
        if (!strcmp(ffd.cFileName, "..")) continue;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            StringCchCopy(rDir, 3 * MAX_PATH, path);
            StringCchCat(rDir, 3 * MAX_PATH, TEXT("\\"));
            StringCchCat(rDir, 3 * MAX_PATH, ffd.cFileName);
            bigFileCounter += FindBiggestFiles(rDir);
        }
        else {
            fileSize.LowPart = ffd.nFileSizeLow;
            fileSize.HighPart = ffd.nFileSizeHigh;
            if (fileSize.QuadPart > (size_t)2e8) {
                bigFileCounter++;
                _tprintf(TEXT("\n  %s   %zu bytes\n"), ffd.cFileName, fileSize.QuadPart);
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);



    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        DisplayErrorBox(TEXT("FindFirstFile"));
    }
    FindClose(hFind);
    return bigFileCounter;
}



int FindAllFilesInDirectory(TCHAR* argv[]) {
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER fileSize;
    TCHAR szDir[MAX_PATH];
    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    int directoryCounter = 0;
    int fileCounter = 0;
    int total = 0;


    StringCchLength(argv[1], MAX_PATH, &length_of_arg);

    if (length_of_arg > (MAX_PATH - 3)) {
        _tprintf(TEXT("\nDirectory path is too long.\n"));
        return (-1);
    }

    _tprintf(TEXT("\nTarget directory is %s\n\n"), argv[1]);

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.

    StringCchCopy(szDir, MAX_PATH, argv[1]);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        DisplayErrorBox(TEXT("FindFirstFile"));
        return dwError;
    }

    // List all the files in the directory with some info about them.

    do {
        total++;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!strcmp(ffd.cFileName, ".")) continue;
            if (!strcmp(ffd.cFileName, "..")) continue;
            directoryCounter++;
            _tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
        }
        else {
            fileCounter++;
            fileSize.LowPart = ffd.nFileSizeLow;
            fileSize.HighPart = ffd.nFileSizeHigh;
            _tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, fileSize.QuadPart);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        DisplayErrorBox(TEXT("FindFirstFile"));
    }
    _tprintf(TEXT("Number of directories: %d\n"), directoryCounter);
    _tprintf(TEXT("Number of files: %d\n"), fileCounter);
    _tprintf(TEXT("Total: %d\n"), total);

    FindClose(hFind);
    return dwError;
}
