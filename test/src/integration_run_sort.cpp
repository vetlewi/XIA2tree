// High-level integration tests placeholder for the full sorting pipeline.
//
// The main orchestration function RunSort lives in main.cpp together
// with ROOT and I/O heavy dependencies. Wiring that directly into a
// library for testing requires a small refactor of the production code,
// so for now we keep this file as a placeholder which can be extended
// once that refactor is done.

#include "doctest/doctest.h"

TEST_CASE("RunSort integration: placeholder") {
    //DOCTEST_SKIP("RunSort integration tests require exposing RunSort from a library target.");
}

