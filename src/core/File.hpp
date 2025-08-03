#pragma once

#include "core/Path.hpp"
#include <string>

class File {
    public:
        File(const Path &path);
        bool exists() const;
        std::string read() const;

    private:
        const Path &path;
};