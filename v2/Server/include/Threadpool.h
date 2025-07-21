#pragma once

#include "Task_queue.h"
#include <cstddef>
#include <thread>
#include <vector>
#include <functional>

using std::thread;
using std::vector;
using std::function;

using Task = function<void()>;

class Thread_pool {
public:
    Thread_pool(size_t thread_num, size_t queue_size);
    ~Thread_pool();

    void start();
    void stop();
    void add_task(Task && task);
    Task getTask();

private:
    void dotask();
    size_t m_thread_num;
    vector<thread> m_threads;
    size_t m_queue_size;
    Task_queue m_task_queue;
    bool m_isExit;
};
