#ifndef LINUXHELLO_SYNCHRONIZED_QUEUE_H
#define LINUXHELLO_SYNCHRONIZED_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

template <class T>
class synchronized_queue {
    std::queue<T> my_queue;
    std::mutex my_mutex;
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
    std::unique_lock<std::mutex> lock(my_mutex);

    this->my_queue.push(t);
    this->s++;

    cv.notify_all();

}

template <class T>
T synchronized_queue<T>::wait_dequeue() {
    std::unique_lock<std::mutex> lock(my_mutex);

    cv.wait(lock, [this] { return this->s >= 1; });

    T t = this->my_queue.front();
    this->my_queue.pop();
    this->s--;


    return t;
}

template <class T>
size_t synchronized_queue<T>::size() {
    return this->s;
}

#endif  // LINUXHELLO_SYNCHRONIZED_QUEUE_H
