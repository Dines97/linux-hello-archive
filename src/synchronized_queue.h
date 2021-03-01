#ifndef LINUXHELLO_SYNCHRONIZED_QUEUE_H
#define LINUXHELLO_SYNCHRONIZED_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

template <class T>
class synchronized_queue {
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable cv;

    size_t s = 0;

   public:
    synchronized_queue();

    void enqueue(T& t);

    T wait_dequeue();

    size_t size();
};

template <class T>
synchronized_queue<T>::synchronized_queue() = default;

template <class T>
void synchronized_queue<T>::enqueue(T& t) {
    std::unique_lock<std::mutex> lock(_mutex);

    this->_queue.push(t);
    this->s++;

    cv.notify_all();
}

template <class T>
T synchronized_queue<T>::wait_dequeue() {
    std::unique_lock<std::mutex> lock(_mutex);

    cv.wait(lock, [this] { return this->size() >= 1; });

    T t = this->_queue.front();
    this->_queue.pop();
    this->s--;

    cv.notify_all();

    return t;
}

template <class T>
size_t synchronized_queue<T>::size() {
    return this->s;
}

#endif  // LINUXHELLO_SYNCHRONIZED_QUEUE_H
