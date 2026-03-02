// Tests for CLI::ParseCLA and CLI::Options

#include "doctest/doctest.h"

#include "Tools/CommandLineInterface.h"

TEST_CASE("CLI: required arguments are parsed") {
    const char* argv[] = {
        "XIA2tree",
        "-i", "file1.lmd", "file2.lmd",
        "-o", "out.root",
        "-C", "cal/LaBr250MHz.yaml"
    };
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    CLI::Options opt = CLI::ParseCLA(argc, const_cast<char**>(argv));

    REQUIRE(opt.input.has_value());
    CHECK(opt.input->size() == 2);

    REQUIRE(opt.output.has_value());
    CHECK(*opt.output == "out.root");

    REQUIRE(opt.CalibrationFile.has_value());
    CHECK(*opt.CalibrationFile == "cal/LaBr250MHz.yaml");
}

TEST_CASE("CLI: defaults are set") {
    const char* argv[] = {
        "XIA2tree",
        "-i", "f.lmd",
        "-o", "out.root"
    };
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    CLI::Options opt = CLI::ParseCLA(argc, const_cast<char**>(argv));

    REQUIRE(opt.coincidenceTime.has_value());
    REQUIRE(opt.SplitTime.has_value());
    REQUIRE(opt.tree.has_value());
    REQUIRE(opt.sortType.has_value());
    REQUIRE(opt.Trigger.has_value());

    CHECK(opt.coincidenceTime.value() == doctest::Approx(1500.0));
    CHECK(opt.SplitTime.value() == doctest::Approx(1500.0));
    CHECK(opt.tree.value() == false);
    CHECK(opt.sortType.value() == CLI::sort_type::coincidence);
}

