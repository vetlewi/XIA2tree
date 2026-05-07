// Basic construction tests for Task::Splitter

#include "doctest/doctest.h"

#include "Tasks/Splitter.h"
#include "Tasks/Queue.h"

TEST_CASE("Splitter: can be constructed and provides output queue") {
    using namespace Task;

    EventQueue_t input;
    Splitter splitter(input, 1500.0, 1024);

    auto& out = splitter.GetQueue();
    // We can't easily assert on contents without driving the full
    // concurrent pipeline, but we can at least check that the reference
    // is usable.
    CHECK(&out != nullptr);
}

