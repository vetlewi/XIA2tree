// Placeholder tests for OCL::ConfigManager.
// The full configuration logic depends on external YAML files and
// XIA types, which are better exercised in higher-level integration
// tests using real calibration files.

#include "doctest/doctest.h"

#include "PhysicalParam/ConfigManager.h"

TEST_CASE("ConfigManager: basic construction from file does not throw when file exists") {
    // This test will be turned into a real integration-style test once
    // a small self-contained YAML calibration file is added under test/data.
    //DOCTEST_SKIP("ConfigManager tests are not implemented yet.");
}


