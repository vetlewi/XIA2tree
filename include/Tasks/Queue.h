//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#ifndef QUEUE_H
#define QUEUE_H


#include <Format/entry.h>
#include <Format/event.h>
#include <Format/xiaformat.h>

#include "SPSCQueue.h"
#include "SPMCQueue.h"

#include <blockingconcurrentqueue.h>

//#define SIZE 16384
#define SIZE 2048
//#define SIZE 32

namespace Task {

    using XIAQueue_t = SPSCBlockingQueue<const XIA_base_t *, SIZE>;
    using EntryQueue_t = SPSCBlockingQueue<Entry_t, SIZE>;
    using EventQueue_t = SPSCBlockingQueue<std::vector<Entry_t>, SIZE>;
    using MCEventQueue_t = SPSCBlockingQueue<std::vector<Entry_t>, SIZE>;
    using TEventQueue_t = SPSCBlockingQueue<std::pair<std::vector<Entry_t>, int>, SIZE>;
    using MTEventQueue_t = moodycamel::BlockingConcurrentQueue<std::pair<std::vector<Entry_t>, int>>;
}

#endif // QUEUE_H
