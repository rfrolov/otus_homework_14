#include "MapReduce.h"
#include <fstream>

#include <iostream>


MapReduce::MapReduce(const std::string &file_name, size_t mnum, size_t rnum) : m_file_name{file_name}, m_mnum{mnum}, m_rnum{rnum}, m_blocks{} {
    m_blocks.resize(m_mnum);
}

void MapReduce::run() {
    try {
        m_blocks = split(m_mnum);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

std::vector<MapReduce::Block> MapReduce::split(size_t mnum) {
    std::vector<MapReduce::Block> blocks;
    blocks.resize(mnum);

    std::ifstream fs(m_file_name);
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
