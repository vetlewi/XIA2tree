// Placeholder for MergeFiles integration tests.
//
// MergeFiles is currently defined in main.cpp alongside the program's
// entry point. To test it directly we would need to expose it via a
// separate library target. Until that refactor is in place, this test
// is marked as skipped so the suite still builds and runs cleanly.

#include "doctest/doctest.h"

TEST_CASE("MergeFiles integration: placeholder") {
    //DOCTEST_SKIP("MergeFiles integration tests require factoring the function into a library.");
}

