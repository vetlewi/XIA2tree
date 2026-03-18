//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#include "BufferSE.h"

using namespace Task;

BufferSE::BufferSE(EntryQueue_t &input, const size_t &buf_size, const size_t &cap)
    : input_queue( input )
    , output_queue( /*cap*/ )
    , size( buf_size )
{
}

void BufferSE::Run()
{
    QueueWorker worker(output_queue);
    Entry_t event;
    while ( input_queue.is_not_finish() || !input_queue.empty() ) {
        if ( !input_queue.try_pop( event )) {
            std::this_thread::yield();
            continue;
        }
        buffer.push(event);
        if ( buffer.size() > size ){
            output_queue.push(buffer.top());
            buffer.pop();
        }
    }

    while ( !buffer.empty() ) {
        output_queue.push(buffer.top());
        buffer.pop();
    }

    is_done = true;
    output_queue.mark_as_finish();
}