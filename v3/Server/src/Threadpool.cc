#include "../include/Threadpool.h"


#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread>
using std::cout;
using std::endl;


Thread_pool::Thread_pool(size_t thread_num, size_t queue_size) :
m_thread_num(thread_num),
m_threads(),
m_queue_size(queue_size),
m_task_queue(m_queue_size),
m_isExit(false){

}

Thread_pool::~Thread_pool(){}

void Thread_pool::start() {
    for(size_t idx = 0; idx < m_thread_num; idx++) {
        m_threads.push_back(thread(&Thread_pool::dotask,this));
    }
}

void Thread_pool::stop() {
    while(!m_task_queue.empty()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    m_isExit = true;
    m_task_queue.wakeup();

    for(auto &th : m_threads) {
        th.join();
    }
}


void Thread_pool::add_task(Task &&task) {
    if(task) {
        m_task_queue.push(std::move(task));
    }
}

Task Thread_pool::getTask(){
    return m_task_queue.pop();
}

void Thread_pool::dotask() {
    while(!m_isExit) {
        Task taskcb = getTask();
        if(taskcb) {
            taskcb();
        }else {
            cout << "nullptr == taskcb " << endl;
        }
    }
}