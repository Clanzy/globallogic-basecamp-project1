#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>

bool walk(const std::string &name, std::string &path, DIR *walker)
{
    struct dirent *dir;
    if (path == "/run/user/")
        return false;
    while ((dir = readdir(walker)))
    {

        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        }
        if (strcmp(dir->d_name, name.c_str()) == 0)
        {
            path.append(dir->d_name);
            return true;
        }
        if (dir->d_type == DT_DIR)
        {
            path.append(dir->d_name).append("/");
            DIR *another_walker = opendir(path.c_str());
            if (another_walker)
            {
                bool isSuccessful = walk(name, path, another_walker);
                closedir(another_walker);

                if (isSuccessful)
                {
                    return true;
                }
            }
            path.resize(path.size() - strlen(dir->d_name) - 1);
        }
    }

    return false;
}

void find_path(const std::string &name, std::string &path)
{
    path.append("/");
    DIR *walker = opendir("/");
    walk(name, path, walker);
}

int main(int argc, char *argv[])
{
    std::string path;
    const std::string name = argv[1];
    find_path(name, path);
    std::cout << path << '\n';
    return 0;
}