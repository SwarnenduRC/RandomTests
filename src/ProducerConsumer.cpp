#include "ProducerConsumer.hpp"
#include <random>

#include <logger/LOGGER_MACROS.hpp>

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
    logger::LOG_ENTRY();
    std::lock_guard<std::mutex> lock(m_mtx);
    m_dataQueue.emplace(val);
    std::ostringstream oss;
    oss << std::endl << "Producer[" << getID() << "] ";
    oss << "produces data[" << val << "] ";
    oss << "while running in thread " << std::this_thread::get_id() << "\n" << std::endl;
    logger::LOG_INFO(oss.str());
    m_cv.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    logger::LOG_EXIT();
}

void Producer::consume()
{
    throw std::logic_error("A producer is not meant for consume data");
}

void Consumer::consume()
{
    logger::LOG_ENTRY();
    std::unique_lock<std::mutex> lock(m_mtx);
    m_cv.wait(lock, []{ return !m_dataQueue.empty() || m_isDataReady; });
    m_consumedDataQ.emplace(m_dataQueue.front());
    m_dataQueue.pop();
    std::ostringstream oss;
    oss << std::endl << "Consumer[" << getID() << "] ";
    oss << "consumes data[" << m_consumedDataQ.front() << "] ";
    oss << "while running in thread " << std::this_thread::get_id() << "\n" << std::endl;
    logger::LOG_INFO(oss.str());
    m_consumedDataQ.pop();
    lock.unlock();
    m_cv.notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    logger::LOG_EXIT();
}

void Consumer::produce(const int)
{
    throw std::logic_error("A consumer not supposed to produce data");
}

