//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <Task.h>
#include <Queue.h>

#include <deque>


namespace Task {

    class Buffer : public Base
    {

    private:
        EntryQueue_t &input_queue;
        EventQueue_t output_queue;

        const size_t size;
        std::vector<Entry_t> buffer;

    public:

        Buffer(EntryQueue_t &input, const size_t &buf_size = 65536/*196608*/, const size_t &cap = 1024);
        EventQueue_t &GetQueue(){ return output_queue; }
        void Run() override;

    };

}

#endif //TDR2TREE_BUFFER_H
