#pragma once
#include <mutex>
#include <condition_variable>
#include <deque>

namespace LazyParkingWindowsCPPClientBL
{
    // This is Dietmar Kuhl's implementation (slightly edited) from https://stackoverflow.com/questions/12805041/c-equivalent-to-javas-blockingqueue
    template <typename T>
    class BlockingQueue
    {
    private:
        std::mutex              m_Mutex;
        std::condition_variable m_Condition;
        std::deque<T>           m_Queue;
    
    public:
        BlockingQueue() = default;
        BlockingQueue(const BlockingQueue&) = delete;

        void Enqueue(const T& i_Value) 
        {
            {
                std::lock_guard<std::mutex> lock(this->m_Mutex);
                m_Queue.push_front(i_Value);
            }

            this->m_Condition.notify_one();
        }

        T Dequeue() 
        {
            std::unique_lock<std::mutex> lock(this->m_Mutex);
            this->m_Condition.wait(lock, [&] { return !this->m_Queue.empty(); });
            T rc(std::move(this->m_Queue.back()));
            this->m_Queue.pop_back();
            return rc;
        }

        void Clear()
        {
            std::lock_guard<std::mutex> lock(this->m_Mutex);
            m_Queue.clear();
        }
    };
}