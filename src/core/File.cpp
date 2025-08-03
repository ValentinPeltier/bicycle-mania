#include "File.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

File::File(const Path &path)
    : path(path) {
}

bool File::exists() const {
    return std::filesystem::exists(this->path.getAbsolute());
}

std::string File::read() const {
    if (!this->exists()) {
        throw std::runtime_error("File " + this->path.getAbsolute() + " does not exist.");
    }

    // From https://stackoverflow.com/a/68031810
    std::ifstream file(this->path.getAbsolute());
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}