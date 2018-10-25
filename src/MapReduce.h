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

    std::vector<Block> split(const std::string &file_name, size_t mnum);
    std::vector<std::string> get_combinations(const std::string &str);
    void map_worker(const std::string &file_name, const Block &block, std::vector<std::string> &result);

    std::string m_file_name;
    size_t      m_mnum;
    size_t      m_rnum;

};