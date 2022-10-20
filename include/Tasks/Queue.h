//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#ifndef QUEUE_H
#define QUEUE_H

#include <atomic>

#include <Format/entry.h>
#include <Format/event.h>
#include <Format/xiaformat.h>

#include <readerwriterqueue.h>
#include <blockingconcurrentqueue.h>
#include <readerwritercircularbuffer.h>
#include <concurrentqueue.h>

namespace Task {

    template<typename T>
    struct QueueWrapper
    {
        T queue;
        std::atomic<bool> done;

        template<typename U>
        inline bool try_enqueue(U const &u){ return queue.try_enqueue(u); }

        template<typename U>
        inline bool try_enqueue(U &&u){ return queue.try_enqueue(u); }

        template<typename U>
        inline auto enqueue(U const &u){ return queue.enqueue(u); }

        template<typename U>
        inline auto enqueue(U &&u){ return queue.enqueue(u); }

        template<typename U>
        inline void wait_enqueue(U const &u){ queue.wait_enqueue(u); }

        template<typename U>
        inline void wait_enqueue(U &&u){ queue.wait_enqueue(u); }

        template <typename U, typename V>
        inline bool wait_enqueue_timed(U &u, V const &v){ return queue.wait_enqueue(u); }

        template <typename U, typename V>
        inline bool wait_enqueue_timed(U &&u, V const &v){ return queue.wait_enqueue(u); }

        template<typename U, typename V>
        inline bool wait_dequeue_timed(U &u, V const &v){ return queue.wait_dequeue_timed(u, v); }

        QueueWrapper(const size_t &capacity) : queue( capacity ), done( false ){}
    };

    //using EntryQueue_t = QueueWrapper<moodycamel::BlockingReaderWriterCircularBuffer<entry_t>>;
    using XIAQueue_t = moodycamel::BlockingReaderWriterCircularBuffer<const XIA_base_t *>;
    using EntryQueue_t = moodycamel::BlockingReaderWriterCircularBuffer<Entry_t>;
    using EventQueue_t = moodycamel::BlockingReaderWriterCircularBuffer<std::vector<Entry_t>>;
    //using TEventQueue_t = moodycamel::BlockingReaderWriterCircularBuffer<std::pair<std::vector<Entry_t>, size_t>>;
    using MCEventQueue_t = moodycamel::BlockingConcurrentQueue<std::vector<Entry_t>>;
    using TEventQueue_t = moodycamel::BlockingConcurrentQueue<std::pair<std::vector<Entry_t>, size_t>>;
    //using TEventQueue_t = moodycamel::BlockingReaderWriterCircularBuffer<Triggered_event>;
}

#endif // QUEUE_H
