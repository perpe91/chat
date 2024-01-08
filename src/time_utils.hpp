#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

inline std::chrono::system_clock::time_point time_from_string(const std::string& es) {
    long ep = std::stol(es);
    return std::chrono::system_clock::time_point{std::chrono::seconds{ep}};
}

inline std::string time_to_string(const std::chrono::system_clock::time_point& tp) {
    long ep = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
    return std::to_string(ep);
}

inline std::string time_to_human_readable_string(const std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream os;
    os << std::put_time(&tm, "%c");
    return os.str();
}
