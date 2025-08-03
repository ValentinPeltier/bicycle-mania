#pragma once

#include <string>

class Path {
    public:
        Path(const std::string &path);
        std::string getAbsolute() const;
        std::string getName() const;

    private:
        std::string path;

        static std::string getProjectRoot();
        static bool isAbsolute(const std::string &path);
};