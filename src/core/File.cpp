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

std::string File::readText() const {
    if (!this->exists()) {
        throw std::runtime_error("File " + this->path.getAbsolute() + " does not exist.");
    }

    // From https://stackoverflow.com/a/68031810
    std::ifstream file(this->path.getAbsolute());
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

std::vector<char> File::readBinary() const {
    std::ifstream file(this->path.getAbsolute(), std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file.");
    }

    // Create the buffer
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // Read file data into buffer
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}