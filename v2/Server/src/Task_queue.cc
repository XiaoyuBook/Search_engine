#include "../include/Task_queue.h"
#include <cstddef>
#include <mutex>


Task_queue::Task_queue(size_t capa) :
m_capacity(capa),
m_queue(),
m_mutex(),
m_notempty(),
m_notfull(),
m_flag(true){

}

Task_queue::~Task_queue() {

}

void Task_queue::push(ElemType &&task) {
    std::unique_lock<mutex> ul(m_mutex);
    while(full()) {
        m_notfull.wait(ul);
    }
    m_queue.push(std::move(task));
    m_notempty.notify_one();
}


ElemType Task_queue::pop() {
    unique_lock<mutex> ul(m_mutex);
    while(empty() && m_flag) {
        m_notempty.wait(ul);
    }


    if(m_flag) {
        ElemType tmp = m_queue.front();
        m_queue.pop();
        m_notfull.notify_one();
        return tmp;
    } else {
        return nullptr;
    }
}



bool Task_queue::empty() const
{
    /* return _que.empty(); */
    return m_queue.size() == 0;
}

bool Task_queue::full() const
{
    return m_queue.size() == m_capacity;
}

//将所有等待在非空条件变量上的线程唤醒
void Task_queue::wakeup()
{
    m_flag = false;
    m_notempty.notify_all();
}
