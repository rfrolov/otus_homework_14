#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <algorithm>

struct ParseCmdLine {
    using port_t = uint16_t;

    explicit ParseCmdLine(int argc, char *argv[]) {
        long long port{0};

        if (argc != 4) {
            m_error = "Неверное число аргументов!\nВызов должен иметь следующий формат: yamr <file_name> <mnum> <rnum>";
            return;
        }

        m_file_name = argv[1];

        if (!str_to_num(argv[2], m_mnum)) {
            m_error    = "Неверный параметр: mnum";
            m_is_valid = false;
            return;
        }
        if (!str_to_num(argv[3], m_rnum)) {
            m_error    = "Неверный параметр: rnum";
            m_is_valid = false;
            return;
        }

        m_is_valid = true;
    }

    auto is_valid() const { return m_is_valid; }
    const auto &error() const { return m_error; }
    const auto &file_name() const { return m_file_name; }
    auto mnum() const { return m_mnum; }
    auto rnum()const { return m_rnum; }

    ParseCmdLine() = delete;

private:
    bool str_to_num(const std::string &str, size_t &num) {
        if (str.size() > std::numeric_limits<size_t>::digits10 + 1 || !std::all_of(str.cbegin(), str.cend(), ::isdigit)) {
            return false;
        }
        num = (size_t) strtoll(str.c_str(), nullptr, 0);
        return true;
    }

    bool        m_is_valid{};
    std::string m_error{};
    std::string m_file_name{};
    size_t      m_mnum{};
    size_t      m_rnum{};
};