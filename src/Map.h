#pragma once

#include <string>
#include <vector>
#include <algorithm>

struct Map {
    auto operator()(const std::string &line) {
        std::vector<std::string> result;

        std::generate_n(std::back_inserter(result), line.size(), [&line, i = size_t{0}]() mutable {
            return line.substr(0, ++i);
        });

        return std::move(result);
    }
};
