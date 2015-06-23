#pragma  once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T> class synchronized_queue
{
public:
    T& front(void)
    {
        wait_for_item();
        qmutex.lock();
        T& item = queue.front();
        qmutex.unlock();
        return item;
    }

    void pop(void)
    {
        wait_for_item();
        qmutex.lock();
        queue.pop();
        qmutex.unlock();
    }

    void push(const T& v)
    {
        qmutex.lock();
        queue.push(v);
        qmutex.unlock();
        ecv.notify_one();
    }

private:
    void wait_for_item(void)
    {
        qmutex.lock();
        bool empty = queue.empty();
        qmutex.unlock();
        if (empty) {
            std::unique_lock<std::mutex> lk(emutex);
            ecv.wait(lk, [this]{ return !queue.empty(); });
            lk.unlock();
        }
    }

    std::queue<T> queue;
    std::mutex qmutex, emutex;
    std::condition_variable ecv;
};
