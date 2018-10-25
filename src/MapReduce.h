#pragma once

#include <string>
#include <vector>
#include <future>

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
    std::vector<std::string> map_worker(const std::string &file_name, const Block &block);
    void shuffle_worker(std::future<std::vector<std::string>>& map_future, std::vector<std::unique_ptr<std::mutex>> &mutexes,
                        std::vector<std::vector<std::string>> &worker_result);
    void reduce_worker(size_t id, std::vector<std::string>& data);

    std::string m_file_name;
    size_t      m_mnum;
    size_t      m_rnum;

};