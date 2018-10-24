#pragma once

#include <string>
#include <vector>

struct MapReduce {
    MapReduce(const std::string &file_name, size_t mnum, size_t rnum);

    void run();

private:
    struct Block {
        size_t begin{};
        size_t end{};
    };

    std::vector<Block> split(size_t mnum);

    std::string        m_file_name;
    size_t             m_mnum;
    size_t             m_rnum;
    std::vector<Block> m_blocks;
};