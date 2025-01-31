#ifndef PRODUCER_CONSUMER_HPP
#define PRODUCER_CONSUMER_HPP

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
class Data
{
    public:
        virtual void produce(const int val) = 0;
        virtual void consume() = 0;
        static int getRandomNumber();
        inline void setID(const std::size_t id) noexcept { m_id = id; }
        inline std::size_t getID() const noexcept { return m_id; }

        Data() = default;
        Data(const Data&) = delete;
        Data& operator=(const Data&) = delete;
        virtual ~Data() = default;
    
        static std::mutex m_mtx;
        static std::atomic_bool m_isDataReady;
        static std::condition_variable m_cv;
        static std::queue<int> m_dataQueue;
    
    private:
        std::size_t m_id;
};

class Producer : public Data
{
    public:
        void produce(const int val) override;
    private:
        void consume() override;
};

class Consumer : public Data
{
    public:
        int getData() noexcept;
        void consume() override;
    private:
        void produce(const int) override;
        std::queue<int> m_consumedDataQ;
};

#endif