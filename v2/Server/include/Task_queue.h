#pragma once

#include <cstddef>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

using std::queue;
using std::mutex;
using std::unique_lock;
using std::condition_variable;
using std::function;



using ElemType = function<void()>;

class Task_queue {
public:
    Task_queue(size_t capa);
    ~Task_queue();

    void push(ElemType && task);
    ElemType pop();

    bool empty() const;
    bool full() const;

    void wakeup();

private:
    size_t m_capacity;
    queue<ElemType> m_queue;
    mutex m_mutex;
    condition_variable m_notempty;
    condition_variable m_notfull;
    bool m_flag;
};