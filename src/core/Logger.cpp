#include "Logger.hpp"
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <map>
#include <string>

void Logger::setFormat(const std::string &format) noexcept {
    Logger::format = format;
}

void Logger::debug(const std::string &message, const std::initializer_list<std::string> &values) noexcept {
#ifdef DEBUG
    auto builtMessage = Logger::buildMessage(message, values);
    auto parameters = Logger::buildParameters("DEBUG", builtMessage);
    std::cout << Logger::replace(Logger::format, parameters) << std::endl;
#endif
}

void Logger::info(const std::string &message, const std::initializer_list<std::string> &values) noexcept {
    auto builtMessage = Logger::buildMessage(message, values);
    auto parameters = Logger::buildParameters("INFO", builtMessage);
    std::cout << Logger::replace(Logger::format, parameters) << std::endl;
}

void Logger::warning(const std::string &message, const std::initializer_list<std::string> &values) noexcept {
    auto builtMessage = Logger::buildMessage(message, values);
    auto parameters = Logger::buildParameters("WARNING", builtMessage);
    std::cerr << Logger::replace(Logger::format, parameters) << std::endl;
}

void Logger::error(const std::string &message, const std::initializer_list<std::string> &values) noexcept {
    auto builtMessage = Logger::buildMessage(message, values);
    auto parameters = Logger::buildParameters("ERROR", builtMessage);
    std::cerr << Logger::replace(Logger::format, parameters) << std::endl;
}

std::map<std::string, std::string> Logger::buildParameters(
    const std::string &level, const std::string &message) noexcept {
    std::time_t timestamp = std::time(nullptr);
    std::tm *datetime = std::localtime(&timestamp);

    return {
        {"l", level},
        {"t", message},
        // Date
        {"d", Logger::formatDatePart(datetime->tm_mday, 2)},
        {"m", Logger::formatDatePart(datetime->tm_mon + 1, 2)},
        {"y", Logger::formatDatePart((int)((datetime->tm_year + 1900) / 100), 2)},
        {"Y", Logger::formatDatePart(datetime->tm_year + 1900, 4)},
        // Time
        {"H", Logger::formatDatePart(datetime->tm_hour, 2)},
        {"I", Logger::formatDatePart(datetime->tm_hour % 12, 2)},
        {"p", datetime->tm_hour < 12 ? "AM" : "PM"},
        {"M", Logger::formatDatePart(datetime->tm_min, 2)},
        {"S", Logger::formatDatePart(datetime->tm_sec, 2)},
    };
}

std::string Logger::formatDatePart(const int &value, const unsigned int &length) noexcept {
    auto text = std::to_string(value);
    if (text.length() < length) {
        text.insert(0, length - text.length(), '0');
    }
    return text;
}

std::string Logger::replace(std::string text, const std::map<std::string, std::string> &parameters) noexcept {
    char parameterStartCharacter = '%';

    for (unsigned int i = 0; i < text.length(); ++i) {
        // If the character is not the start character, then go to the next one
        if (text.at(i) != parameterStartCharacter) {
            continue;
        }

        // If the start character is doubled, then it must be escaped
        if (text.at(i + 1) == parameterStartCharacter) {
            text = text.replace(i, 2, {parameterStartCharacter});
            continue;
        }

        // Look for any parameter
        for (auto &parameter : parameters) {
            if (parameter.first == text.substr(i + 1, parameter.first.length())) {
                text = text.replace(i, 1 + parameter.first.length(), parameter.second);
                i += parameter.second.length() - 1;
                break;
            }
        }
    }

    return text;
}

std::string Logger::buildMessage(std::string message, const std::initializer_list<std::string> &values) noexcept {
    for (auto &value : values) {
        auto position = message.find("{}");

        // If there is more values than `{}` in the message, just stop here
        if (position == std::string::npos) {
            break;
        }

        message.replace(position, 2, value);
    }

    return message;
}