//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#include "Buffer.h"

using namespace Task;

Buffer::Buffer(EntryQueue_t &input, const size_t &buf_size, const size_t &cap)
    : input_queue( input )
    , output_queue( cap )
    , size( buf_size )
{
    buffer.reserve( 2*size );
}

void Buffer::Run()
{
    Entry_t input{};

    while ( !done ){

        // First we will check if the buffer is big enough for us to flush to the splitter.
        if ( buffer.size() > size ){
            std::sort(buffer.begin(), buffer.end(), [](const Entry_t &lhs, const Entry_t &rhs)
            { return (double(rhs.timestamp - lhs.timestamp) + (rhs.cfdcorr - lhs.cfdcorr)) > 0; });

            auto end = std::adjacent_find(buffer.begin()+size, buffer.end(), [](const Entry_t &lhs, const Entry_t &rhs)
            { return  (double(rhs.timestamp - lhs.timestamp) + (rhs.cfdcorr - lhs.cfdcorr)) > 50000; });

            output_queue.wait_enqueue(std::vector(buffer.begin(), end));
            buffer.erase(buffer.begin(), end);
        }

        if ( input_queue.wait_dequeue_timed(input, std::chrono::seconds(1)) ){
            buffer.push_back(input);
        } else {
            std::this_thread::yield();
        }
    }

    // We reach this point if we are told we are done. Flush buffer.
    if ( !output_queue.try_enqueue(buffer) ){
        // This is no really a problem. It doesn't make sense to kill the process because of this error.
        std::cerr << __PRETTY_FUNCTION__ << ": Unable to flush buffer." << std::endl;
        //throw std::runtime_error(std::string(__PRETTY_FUNCTION__)+"Unable to flush buffer");
    }
    is_done = true;
}