#include <iostream>
#include <dirent.h>
#include <chrono>
#include <list>
#include <queue>

int readOneDir(const std::string &dirName, std::queue<std::string> &newDirs) {
    DIR *pDir = opendir(dirName.c_str());
    if (pDir == nullptr) return 1;

    int count = 0;
    while (true) {
        auto e = readdir(pDir);
        if (e == nullptr) break;

        if (e->d_name[0] == '.') {
            if (e->d_name[1] == '\0') continue;
            if (e->d_name[1] == '.' && e->d_name[2] == '\0') continue;
        }

//        if (strcmp(e->d_name, "out") == 0) continue;

        if (e->d_type == DT_UNKNOWN || e->d_type == DT_DIR) {
            newDirs.emplace(dirName + '/' + std::string(e->d_name));
        }
        count++;
    }
    closedir(pDir);
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
