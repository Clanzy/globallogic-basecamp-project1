#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <atomic>

bool walk(const std::string &name, std::string &path, DIR *walker)
{
    struct dirent *dir;
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
                if(hasFound){
                    return true;
                }
            }
            path.resize(path.size() - strlen(dir->d_name) - 1);
        }
    }
    return false;
}

void find_path(const std::string &name)
{
    std::string path = "/";
    DIR *walker = opendir(path.c_str());
    walk(name, path, walker);
}

int main(int argc, char *argv[])
{
    const std::string name = argv[1];
    find_path(name);
    return 0;
}