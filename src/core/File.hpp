#pragma once

#include "core/Path.hpp"
#include <string>
#include <vector>

class File {
    public:
        File(const Path &path);
        bool exists() const;
        std::string readText() const;
        std::vector<char> readBinary() const;

    private:
        const Path &path;
};