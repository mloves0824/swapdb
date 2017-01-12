//
// Created by zts on 17-1-10.
//

#ifndef SSDB_BLOCKING_QUEUE_H
#define SSDB_BLOCKING_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>


class DumpData {
public:
    std::string key;
    std::string data;

    int64_t expire;
    bool replace;

    DumpData(const std::string &key, const std::string &data, int64_t expire, bool replace) : key(key), data(data),
                                                                                              expire(expire),
                                                                                              replace(replace) {}

};

class BTask {
public:
    uint16_t type;

    std::string data_key;
    void *value;

    BTask(uint16_t type, const std::string &key, void *value = nullptr) : type(type), data_key(key), value(value) {}

    virtual ~BTask() {
    }

};

template<typename T>
class BQueue {
private:
    std::mutex d_mutex;
    std::condition_variable d_condition;
    std::deque<T> d_queue;
public:
    void push(T const &value) {
        {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            d_queue.push_front(value);
        }
        this->d_condition.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        this->d_condition.wait(lock, [=] { return !this->d_queue.empty(); });
        T rc(std::move(this->d_queue.back()));
        this->d_queue.pop_back();
        return rc;
    }
};


#endif //SSDB_BLOCKING_QUEUE_H
