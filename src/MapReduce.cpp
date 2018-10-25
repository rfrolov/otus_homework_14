#include "MapReduce.h"
#include <fstream>
#include <thread>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <future>


MapReduce::MapReduce(const std::string &file_name, size_t mnum, size_t rnum) : m_file_name{file_name}, m_mnum{mnum}, m_rnum{rnum} {
}

void MapReduce::run() {
    auto blocks = split(m_file_name, m_mnum);

    std::vector<std::string> result{};

//    for(auto i = 0; i < m_mnum; ++i) {
//        std::async(map_worker, std::cref(m_file_name), std::cref(blocks[i]), std::ref(result));
//    }
}

std::vector<MapReduce::Block> MapReduce::split(const std::string &file_name, size_t mnum) {
    std::vector<Block> blocks(mnum);

    std::ifstream fs(file_name);
    if (!fs.is_open()) {
        throw std::invalid_argument("Файл не может быть открыт");
    }

    fs.seekg(0, std::ios::end);
    const auto size = static_cast<const size_t>(fs.tellg());
    fs.seekg(0, std::ios::beg);

    const size_t preliminary_block_size = size / mnum;
    if (preliminary_block_size == 0) {
        throw std::invalid_argument("Пареметр mnum имеет слишком большое значение");
    }

    for (size_t i = 1; i < mnum; ++i) {
        const size_t preliminary_position = preliminary_block_size * i;
        fs.seekg(preliminary_position, std::ios::beg);
        size_t cur_pos = preliminary_position;

        for (char ch = 0; ch != '\n';) {
            if (fs.read(&ch, 1).fail() || fs.eof()) {
                throw std::logic_error("Ошибка чтения файла");
            }
            ++cur_pos;
        }

        if (cur_pos > preliminary_position + preliminary_block_size) {
            throw std::logic_error("Ошибка разбивки файла");
        }
        blocks[i - 1].end = cur_pos;
        blocks[i].begin   = cur_pos + 1;
    }

    blocks[mnum - 1].end = size - 1;
    fs.close();

    return std::move(blocks);
}

std::vector<std::string> MapReduce::get_combinations(const std::string &str) {
    std::vector<std::string> result;

    std::generate_n(std::back_inserter(result), str.size(), [&str, i = size_t{0}]() mutable {
        return str.substr(0, ++i);
    });

    return std::move(result);
}

void MapReduce::map_worker(const std::string &file_name, const Block &block, std::vector<std::string> &result) {
    std::ifstream fs(file_name);
    if (!fs.is_open()) {
        throw std::invalid_argument("Can't open file: " + file_name);
    }

    size_t block_size = (block.end - block.begin) + 1;
    fs.seekg(block.begin, std::ios::beg);
    std::string str{};
    str.resize(block_size);
    fs.read(&str[0], block_size);
    fs.close();

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    std::istringstream ss(str);
    std::string        line;

    while (std::getline(ss, line)) {
        if (!line.empty()) {
            auto combinations = get_combinations(line);
            result.insert(result.cend(), std::make_move_iterator(combinations.begin()), std::make_move_iterator(combinations.end()));
        }
    }

    std::sort(result.begin(), result.end());
}