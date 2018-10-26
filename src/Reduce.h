#pragma once

#include <string>

struct Reduce {
    auto operator()(const std::string &line) {
        if (m_last_prefix == line) {
            m_is_same = true;
        } else {
            auto is_contain = line.compare(0, m_last_prefix.length(), m_last_prefix) == 0;
            if (line.length() > m_length && (!(m_is_same || is_contain) || m_is_same)) {
                m_length = line.length();
            }

            m_is_same     = false;
            m_last_prefix = line;
        }

        return m_length;
    }

private:
    std::string m_last_prefix{};
    bool        m_is_same{false};
    size_t      m_length{};
};


