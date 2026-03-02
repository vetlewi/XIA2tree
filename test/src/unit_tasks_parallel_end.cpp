// Tests for Task::ParallelTasksEnd container

#include "doctest/doctest.h"

#include "Tasks/Tasks.h"

namespace {

struct FakeQueue {
    int id = 0;
};

struct FakeTask {
    FakeQueue& queue;
    CLI::Options options;

    FakeTask(FakeQueue& q, const CLI::Options& opt)
        : queue(q), options(opt) {}
};

} // anonymous namespace

TEST_CASE("ParallelTasksEnd: creates expected number of tasks") {
    FakeQueue q;
    CLI::Options opt;

    constexpr size_t num_parallel = 4;
    Task::ParallelTasksEnd<FakeQueue, FakeTask> cont(q, num_parallel, opt);

    // Check indexing
    for (size_t i = 0; i < num_parallel; ++i) {
        FakeTask& t = cont[i];
        CHECK(&t.queue == &q);
    }
}

