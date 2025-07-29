#pragma once

#include <initializer_list>
#include <map>
#include <string>

class Logger {
    private:
        static std::string format;

        Logger();
        static std::map<std::string, std::string> buildParameters(
            const std::string &level, const std::string &message) noexcept;
        static std::string formatDatePart(const int &value, const unsigned int &length) noexcept;
        static std::string replace(std::string text, const std::map<std::string, std::string> &parameters) noexcept;
        static std::string buildMessage(std::string message, const std::initializer_list<std::string> &values) noexcept;

    public:
        static void setFormat(const std::string &format) noexcept;
        static void debug(const std::string &message, const std::initializer_list<std::string> &values = {}) noexcept;
        static void info(const std::string &message, const std::initializer_list<std::string> &values = {}) noexcept;
        static void warning(const std::string &message, const std::initializer_list<std::string> &values = {}) noexcept;
        static void error(const std::string &message, const std::initializer_list<std::string> &values = {}) noexcept;
};

#define LOG_DEBUG(...) ::Logger::debug(__VA_ARGS__)
#define LOG_INFO(...) ::Logger::info(__VA_ARGS__)
#define LOG_WARNING(...) ::Logger::warning(__VA_ARGS__)
#define LOG_ERROR(...) ::Logger::error(__VA_ARGS__)