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

int readOneDir(const std::string &dirName, std::queue<std::string> &newDirs) {
    int count = 0;

    int dirfd = open(dirName.c_str(), O_RDONLY, 0);
    if (dirfd < 0) return 0;

    char attrBuf[1024*1024];

    attrlist attrList{};
    attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
    attrList.commonattr  = ATTR_CMN_RETURNED_ATTRS |
                           ATTR_CMN_NAME |
                           ATTR_CMN_ERROR |
                           ATTR_CMN_OBJTYPE;

    for (;;) {
        int retCount = getattrlistbulk(dirfd, &attrList, &attrBuf[0], sizeof(attrBuf), 0);
        if (retCount <= 0) break;

        char* entry_start = &attrBuf[0];
        for (int index = 0; index < retCount; index++) {
            count++;

            char* field = entry_start;
            uint32_t length = *(uint32_t *)field;
            field += sizeof(uint32_t);
            entry_start += length;

            attribute_set_t returned = *(attribute_set_t *)field;
            field += sizeof(attribute_set_t);

            if (returned.commonattr & ATTR_CMN_ERROR) continue;

            std::string name;
            if (returned.commonattr & ATTR_CMN_NAME) {
                attrreference_t name_info = *(attrreference_t *)field;
                name = (field + name_info.attr_dataoffset);
                field += sizeof(attrreference_t);
            }

            if (returned.commonattr & ATTR_CMN_OBJTYPE) {
                fsobj_type_t obj_type = *(fsobj_type_t *)field;

                if (obj_type == VDIR) {
                    newDirs.emplace(dirName + '/' + name);
                }
            }
        }
    }
    close(dirfd);

    return count;
}

int readRecursive(const std::string &dir) {
    int count = 1;
    std::queue<std::string> dirQueue;
    dirQueue.emplace(dir);

    while (!dirQueue.empty()) {
        std::string home = dirQueue.front();
        dirQueue.pop();
        count += readOneDir(home, dirQueue);
    }

    return count;
}


int main() {
    std::cout << "Hello, World!" << std::endl;

    const char *home = "/Users/jonnyzzz/Work/intellij";

    for(int i = 0; i < 5; i++) {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        int total = readRecursive(home);

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                  << "[ms]" << std::endl;
        std::cout << "Total files: " << total << std::endl;
    }
    return 0;
}
