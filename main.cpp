#include <iostream>
#include <chrono>
#include <list>
#include <queue>

#include <sys/attr.h>
#include <sys/errno.h>
#include <sys/vnode.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fts.h>
#include<string.h>
#include<errno.h>

//https://stackoverflow.com/questions/12609747/traversing-a-filesystem-with-fts3/22354701
int compare(const FTSENT **one, const FTSENT **two) {
    return (strcmp((*one)->fts_name, (*two)->fts_name));
}

int scanDirs(const char* home) {
    int count = 0;
    char name[2323423];
    char* pName = (char*)&name[0];
    strcpy(pName, home);
    FTS* file_system = fts_open(&pName, FTS_COMFOLLOW | FTS_NOCHDIR | FTS_PHYSICAL | FTS_NOSTAT_TYPE, &compare);
    if (file_system == nullptr) return 0;

    while (fts_read(file_system) != nullptr) {
        FTSENT *child = fts_children(file_system, 0);

        if (errno != 0) {
            perror("fts_children");
            continue;
        }

        count++;
        while (child != nullptr && child->fts_link != nullptr) {
            child = child->fts_link;
            count++;
        }
    }
    fts_close(file_system);
    return count;
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    const char *home = "/Users/jonnyzzz/Work/intellij";

    for (int i = 0; i < 5; i++) {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        int total = scanDirs(home);

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                  << "[ms]" << std::endl;
        std::cout << "Total files: " << total << std::endl;
    }
    return 0;
}
