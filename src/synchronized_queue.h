#ifndef LINUXHELLO_SYNCHRONIZED_QUEUE_H
#define LINUXHELLO_SYNCHRONIZED_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

template <class T>
class synchronized_queue {
    std::queue<T> _queue;
    std::mutex _mutex;

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
    std::lock_guard<std::mutex> lock(_mutex);

    this->_queue.push(t);
    this->s++;
}

template <class T>
T synchronized_queue<T>::wait_dequeue() {
    while (this->s < 1) continue;

    std::lock_guard<std::mutex> lock(_mutex);

    T t = this->_queue.front();
    this->_queue.pop();
    this->s--;

    return t;
}

template <class T>
size_t synchronized_queue<T>::size() {
    return this->s;
}

#endif  // LINUXHELLO_SYNCHRONIZED_QUEUE_H
