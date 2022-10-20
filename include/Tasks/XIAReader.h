//
// Created by Vetle Wegner Ingeberg on 09/03/2022.
//

#ifndef XIAREADER_H
#define XIAREADER_H


#include <Task.h>
#include <Queue.h>

#include <map>

class ProgressUI;

/*namespace IO {
    class MemoryMap;
}*/
#include "Tasks/MemoryMap.h"

namespace Task {

    class XIAReader : public Base {
    private:
        std::vector<std::pair<std::string, std::unique_ptr<IO::MemoryMap>>> mapped_files;
        XIAQueue_t output_queue;
        ProgressUI *ui;

        void RunWithUI();
        void RunWithoutUI();

    public:
        XIAReader(const std::vector<std::string> &files, ProgressUI *ui = nullptr, const size_t &capacity = 131072);

        auto &GetQueue(){ return output_queue; }

        void Run() override;

    };

}

#endif // XIAREADER_H
