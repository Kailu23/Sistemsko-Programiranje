#ifdef __linux__ 

#include "UnixLV2.h"


int main(int argc, char* argv[]) {
    int error;
    error = FindAllFilesUnix(argc, argv[1]);

    error = Zad2(argc, argv);

    if (error != 0)
        printf("%d\n", error);

    return 0;
}


#elif _WIN32
#include "WindowsLV2.h"
int _tmain(int argc, TCHAR* argv[]) {

    if (argc != 2) {
        _tprintf(TEXT("\nUsage: %s <directory name>\n"), argv[0]);
        return (-1);
    }
    size_t bigFiles = 0;
    return FindAllFilesInDirectory(argv);
    bigFiles = FindBiggestFiles(argv[1]);
    printf("Number of big files is %zu\n", bigFiles);
    return bigFiles;
    return 0;

    LPFILETIME oldest = (FILETIME*)malloc(sizeof(FILETIME));
    LPSYSTEMTIME systemTime = (LPSYSTEMTIME)malloc(sizeof(SYSTEMTIME));
    
    GetSystemTimeAsFileTime(oldest);
    FileTimeToSystemTime(oldest, systemTime);
    printf("%d.%d.%d\n", systemTime->wDay, systemTime->wMonth, systemTime->wYear);
    
    int err = FindOldestFile(argv[1], oldest);
    
    FileTimeToSystemTime(oldest, systemTime);
    printf("%d.%d.%d\n", systemTime->wDay, systemTime->wMonth, systemTime->wYear);
    
    free(oldest);
    free(systemTime);
    return err;

    return 0;

}
#else

#endif


