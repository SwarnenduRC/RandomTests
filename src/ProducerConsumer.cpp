#include "ProducerConsumer.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <random>
#include <exception>
#include <thread>
#include <chrono>
#include <iostream>

std::queue<int> Data::m_dataQueue;
std::mutex Data::m_mtx;
std::condition_variable Data::m_cv;
std::atomic_bool Data::m_isDataReady(false);

/*static*/ int Data::getRandomNumber()
{
    static std::random_device rd;
    static std::uniform_int_distribution<int> distributor(1, 100);

    return distributor(rd);
}

void Producer::produce(const int val)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    m_dataQueue.emplace(val);
    std::cout << std::endl << "Producer[" << getID() << "] ";
    std::cout << "produces data[" << val << "] ";
    std::cout << "while running in thread " << std::this_thread::get_id() << "\n" << std::endl;
    m_cv.notify_one();
}

void Producer::consume()
{
    throw std::logic_error("A producer is not meant for consume data");
}

void Consumer::consume()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    m_cv.wait(lock, []{ return !m_dataQueue.empty() || m_isDataReady; });
    m_consumedDataQ.emplace(m_dataQueue.front());
    m_dataQueue.pop();
    std::cout << std::endl << "Consumer[" << getID() << "]";
    std::cout << "consumes data[" << m_consumedDataQ.front() << "] ";
    std::cout << "while running in thread " << std::this_thread::get_id() << "\n" << std::endl;
    m_consumedDataQ.pop();
    lock.unlock();
    m_cv.notify_all();
}

void Consumer::produce(const int)
{
    throw std::logic_error("A consumer not supposed to produce data");
}

