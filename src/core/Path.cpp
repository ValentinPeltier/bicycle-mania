#include "Path.hpp"

#if OS_WINDOWS
    #include <Windows.h>
    #include <regex>
#elif OS_LINUX
    #include <unistd.h>
#endif

Path::Path(const std::string &path) {
    if (Path::isAbsolute(path)) {
        this->path = path;
    } else {
        this->path = Path::getProjectRoot() + '/' + path;
    }
}

std::string Path::getAbsolute() const {
    return this->path;
}

std::string Path::getName() const {
    return this->path.substr(this->path.rfind('/'));
}

std::string Path::getProjectRoot() {
#if OS_WINDOWS
    WCHAR execPath[1024];
    GetModuleFileNameW(NULL, execPath, 1024);
    return execPath.substr(0, execPath.rfind('/'));
#elif OS_LINUX
    char buffer[1024];
    readlink("/proc/self/exe", buffer, 1024);
    std::string execPath = std::string(buffer);
    return execPath.substr(0, execPath.rfind('/'));
#endif
}

bool Path::isAbsolute(const std::string &path) {
#if OS_WINDOWS
    const std::regex r{R"/(^[A-Z]:/)/"};
    return std::regex_match(path, r);
#elif OS_LINUX
    return path.starts_with('/');
#endif
}
