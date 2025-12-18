#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

int FindAllFilesUnix(int argc, const char* path) {
    struct dirent* item;
    int directoryCounter = 0;
    int fileCounter = 0;
    int total = 0;

    if (argc != 2) {
        printf("Usage: <source> <path>\n");
        return 1;
    }

    DIR* dir = opendir(path);
    if (!dir) {
        perror("Access denied.");
        return 1;
    }

    while ((item = readdir(dir)) != NULL) {
        total++;
        if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)
            continue;

        char fullPath[4096];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, item->d_name);

        struct stat stats;
        if (lstat(fullPath, &stats) == -1) {
            continue;
        }

        if (S_ISDIR(stats.st_mode)) {
            directoryCounter++;
        }
        else if (S_ISREG(stats.st_mode)) {
            fileCounter++;
        }
    }

    closedir(dir);

    printf("%s   <DIR>\n", path);
    printf("Number of directories: %d\n", directoryCounter);
    printf("Number of files: %d\n", fileCounter);
    printf("Total: %d\n", total);

    return 0;
}



int FindBigFiles(const char* path) {
    size_t bigFileCounter = 0;
    DIR* dir = opendir(path);
    if (!dir) return (-1);

    struct dirent* item;
    while ((item = readdir(dir)) != NULL) {
        if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0) continue;
        if (strcmp(path, "/") == 0 &&
            (strcmp(item->d_name, "proc") == 0 ||
                strcmp(item->d_name, "sys") == 0 ||
                strcmp(item->d_name, "dev") == 0 ||
                strcmp(item->d_name, "run") == 0 ||
                strcmp(item->d_name, "tmp") == 0))
            continue;

        char fullPath[4096];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, item->d_name);

        struct stat stats;
        if (lstat(fullPath, &stats) == -1) continue;

        if (S_ISDIR(stats.st_mode)) {
            bigFileCounter += FindBigFiles(fullPath);
        }
        else if (S_ISREG(stats.st_mode)) {
            if (stats.st_size >= (size_t)200 * 1024 * 1024) {
                bigFileCounter++;
                printf("%s\n", fullPath);
            }
        }
    }

    closedir(dir);

    return bigFileCounter;
}

void FindOldestFile(char* path, struct timespec* oldest, char* oldestPath) {
    DIR* dir = opendir(path);
    if (!dir) {
        oldestPath = "Path is NULL.\n";
        return;
    }

    struct dirent* item;
    while ((item = readdir(dir)) != NULL) {
        if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0) continue;
        if (strcmp(path, "/") == 0 &&
            (strcmp(item->d_name, "proc") == 0 ||
                strcmp(item->d_name, "sys") == 0 ||
                strcmp(item->d_name, "dev") == 0 ||
                strcmp(item->d_name, "run") == 0 ||
                strcmp(item->d_name, "tmp") == 0))
            continue;

        char fullPath[4096];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, item->d_name);

        struct stat stats;
        if (lstat(fullPath, &stats) == -1) continue;

        if (S_ISDIR(stats.st_mode)) {
            FindOldestFile(fullPath, oldest, oldestPath);
        }
        else if (S_ISREG(stats.st_mode)) {
            if (stats.st_mtim.tv_sec < oldest->tv_sec) {
                oldest->tv_sec = stats.st_mtim.tv_sec;
                oldest->tv_nsec = stats.st_mtim.tv_nsec;
                snprintf(oldestPath, 4096, "%s", fullPath);
            }
        }
    }

    closedir(dir);
    return;
}

int Zad2(int argc, char* argv[]) {
    // Zad1
    int bigFiles = 0;


    if (argc != 2) {
        printf("Usage: <source> <path>\n");
        return 1;
    }

    //bigFiles = FindBigFiles(argv[1]);

    //printf("Number of big files: %d\n", bigFiles);

    // Zad2
    struct timespec* oldestFileTime = (struct timespec*)malloc(sizeof(struct timespec));
    timespec_get(oldestFileTime, TIME_UTC);
    char buff[100];
    time_t seconds = oldestFileTime->tv_sec;
    strftime(buff, sizeof buff, "%d/%m/%y %T", gmtime(&seconds));
    printf("Current time: %s\n", buff);

    char* oldestPath = (char*)malloc(4096*sizeof(char));

    FindOldestFile(argv[1], oldestFileTime, oldestPath);

    if (oldestFileTime->tv_sec != 0) {
        seconds = oldestFileTime->tv_sec;
        printf("Path of the oldest file: %s\n", oldestPath);
        strftime(buff, sizeof buff, "%d/%m/%y", gmtime(&seconds));
        printf("Creation date: %s\n", buff);
    }
    else {
        printf("No files.\n");
    }

    return 0;
}