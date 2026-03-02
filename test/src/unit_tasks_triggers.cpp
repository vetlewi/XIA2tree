// Basic construction tests for Task::Triggers and Task::STrigger

#include "doctest/doctest.h"

#include "Tasks/Trigger.h"
#include "Tasks/Queue.h"
#include "Tools/CommandLineInterface.h"

TEST_CASE("Triggers: can be constructed and produce STrigger instances") {
    using namespace Task;

    MCEventQueue_t input;
    Triggers triggers(input, 1500.0, DetectorType::eDet, CLI::sort_type::coincidence, 1024);

    auto* t1 = triggers.GetNewTrigger();
    auto* t2 = triggers.GetNewTrigger();

    CHECK(t1 != nullptr);
    CHECK(t2 != nullptr);
    CHECK(t1 != t2);
}

