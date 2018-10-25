#include "MapReduce.h"
#include <fstream>
#include <thread>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <future>


MapReduce::MapReduce(const std::string &file_name, size_t mnum, size_t rnum) :
        m_file_name{file_name}
        , m_mnum{mnum}
        , m_rnum{rnum} {
}

void MapReduce::run() {
    // split
    auto blocks = split(m_file_name, m_mnum);


    // map
    std::vector<std::future<std::vector<std::string>>> map_futures(m_mnum);

    for (auto i = 0; i < m_mnum; ++i) {
        map_futures[i] = (std::async(
                [this, &file_name = m_file_name, &block = blocks[i]] { return map_worker(file_name, block); }));
    }
    blocks.clear();


    // Shuffle
    std::vector<std::thread>                 shuffle_threads(m_mnum);
    std::vector<std::unique_ptr<std::mutex>> shuffle_mutexes(m_rnum);
    std::vector<std::vector<std::string>>    shuffle_result(m_rnum);

    for (auto i = 0; i < m_rnum; ++i) {
        shuffle_mutexes[i] = (std::make_unique<std::mutex>());
    }

    for (auto i = 0; i < m_mnum; ++i) {
        shuffle_threads[i] = (std::thread(
                [this, map_future = std::ref(map_futures[i]), mutexes = std::ref(shuffle_mutexes),
                       worker_result = std::ref(shuffle_result)] {
                    return shuffle_worker(map_future, mutexes, worker_result);
                }));
    }
    for (auto i = 0; i < m_mnum; ++i) {
        if (shuffle_threads[i].joinable()) {
            shuffle_threads[i].join();
        }
    }


    // reduce
    std::vector<std::thread> reduce_threads(m_rnum);

    for (size_t i = 0; i < m_rnum; ++i) {
        reduce_threads[i] = (std::thread(
                [this, i, data = std::ref(shuffle_result[i])] {
                    return reduce_worker(i, data);
                }));
    }
}

void MapReduce::reduce_worker(size_t id, std::vector<std::string> &data) {

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

std::vector<std::string> MapReduce::map_worker(const std::string &file_name, const Block &block) {
    std::vector<std::string> result{};
    std::ifstream            fs(file_name);

    if (!fs.is_open()) {
        throw std::invalid_argument("Невозможно открыть: " + file_name);
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
            result.insert(result.cend(), std::make_move_iterator(combinations.begin()),
                          std::make_move_iterator(combinations.end()));
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}

void MapReduce::shuffle_worker(std::future<std::vector<std::string>> &map_future,
                               std::vector<std::unique_ptr<std::mutex>> &mutexes,
                               std::vector<std::vector<std::string>> &worker_result) {
    auto rnum       = mutexes.capacity();
    auto map_result = map_future.get();

    for (const auto &note:map_result) {

        std::hash<std::string>      hashFunction;
        auto                        index = hashFunction(note) % rnum;
        std::lock_guard<std::mutex> lock(*mutexes[index]);
        worker_result[index].emplace_back(note);
    }
}
