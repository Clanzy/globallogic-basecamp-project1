#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <array>
#include <thread>
#include <atomic>
#include <chrono>
#include <utility>

#define NUMBER_OF_THREADS 7

std::atomic_bool hasFound(false);
bool walk(const std::string &name, std::string &path, DIR *walker)
{
    struct dirent *dir;
    while ((dir = readdir(walker)))
    {
        if (hasFound.load() == true)
        {
            return true;
        }
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        }
        if (strcmp(dir->d_name, name.c_str()) == 0)
        {
            path.append(dir->d_name);
            if (dir->d_type == DT_DIR)
            {
                path.append("/");
            }
            std::cout << path << '\n';
            hasFound.store(true);
            return true;
        }
        if (dir->d_type == DT_DIR)
        {
            path.append(dir->d_name).append("/");
            DIR *another_walker = opendir(path.c_str());
            if (another_walker)
            {
                bool hasFound = walk(name, path, another_walker);
                closedir(another_walker);
                if (hasFound)
                {
                    return true;
                }
            }
            path.resize(path.size() - strlen(dir->d_name) - 1);
        }
    }
    return false;
}
/**
 * We store all root folders as pair of DIR * and path to this folder in queue root_directories.
 * Each thread checks if there any non-taken root folders. If such exists, it takes it. Otherwise, thread ends.
 */
void main_thread(std::vector<std::pair<DIR *, std::string>> &root_directories, const std::string &name)
{
    while (!root_directories.empty() && !hasFound)
    {
        DIR *dir = root_directories.back().first;
        std::string local_string = root_directories.back().second;
        root_directories.pop_back();
        walk(name, std::ref(local_string), dir);
        closedir(dir);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void find_path(const std::string &name)
{
    std::string path = "/";
    DIR *walker = opendir(path.c_str());

    struct dirent *dir;
    std::vector<std::pair<DIR *, std::string>> root_directories;
    while ((dir = readdir(walker)))
    {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        }
        if (strcmp(dir->d_name, name.c_str()) == 0)
        {
            path.append(dir->d_name);
            if (dir->d_type == DT_DIR)
            {
                path.append("/");
            }
            std::cout << path << '\n';
            return;
        }
        if (dir->d_type == DT_DIR)
        {
            path.append(dir->d_name).append("/");
            DIR *another_walker = opendir(path.c_str());
            if (another_walker)
            {
                root_directories.push_back(std::pair<DIR *, std::string>(another_walker, path));
            }
            path.resize(path.size() - strlen(dir->d_name) - 1);
        }
    }
    closedir(walker);
    std::array<std::thread, NUMBER_OF_THREADS> threads;
    for (auto &i : threads)
    {
        i = std::thread(main_thread, std::ref(root_directories), name);
    }
    for (auto &i : threads)
    {
        i.join();
    }
}