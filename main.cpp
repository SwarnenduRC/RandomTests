#include "ProducerConsumer.hpp"

#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <memory>
#include <vector>

#include <logger/LOGGER_MACROS.hpp>

//int main(int argc, char **argv)
int main()
{
    /*::testing::InitGoogleTest(&argc, argv); 
    auto retVal = RUN_ALL_TESTS();
    std::cout << std::endl << std::endl;
    return retVal; */

    logger::LOG_ENTRY();
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (size_t id = 1; id < 15; ++id)
    {
        std::unique_ptr<Producer> producer(new Producer());
        producer->setID(id);
        producers.emplace_back(std::thread(&Producer::produce, std::move(producer), Data::getRandomNumber()));
    }
    for (size_t id = 1; id < 15; ++id)
    {
        std::unique_ptr<Consumer> consumer(new Consumer());
        consumer->setID(id);
        consumers.emplace_back(std::thread(&Consumer::consume, std::move(consumer)));
    }

    for (auto& producer : producers)
    {
        producer.join();
    }
    std::lock_guard<std::mutex> lock(Data::m_mtx);
    Data::m_isDataReady = true;
    Data::m_cv.notify_all();

    for (auto& consumer : consumers)
    {
        consumer.join();
    }
}