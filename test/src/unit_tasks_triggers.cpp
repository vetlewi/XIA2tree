// Basic construction tests for Task::Triggers and Task::STrigger

#include "doctest/doctest.h"

#include "Tasks/Trigger.h"
#include "Tasks/Queue.h"
#include "Tools/CommandLineInterface.h"

TEST_CASE("Triggers: can be constructed and produce STrigger instances") {
    using namespace Task;

    MCEventQueue_t input;
    Trigger triggers(input, 1500.0, DetectorType::eDet, CLI::sort_type::coincidence);
    auto& output = triggers.GetQueue();
}

