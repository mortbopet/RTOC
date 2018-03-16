#include "catch.hpp"

#include "../lib/framefinder.h"

TEST_CASE("exists()", "[framefinder]") {
    SECTION("basic operation") {
        REQUIRE(exists("./Makefile"));
        REQUIRE(!exists("./Makefsidfj"));
        REQUIRE(exists("./CMakeFiles/"));
        REQUIRE(exists("./CMakeFiles/CellSorter_test.dir/"));
    }
}

TEST_CASE("files_from_folder()", "[framefinder]") {
    std::vector<std::string> files;
    SECTION("check if able to find files in build dir") {
        REQUIRE(files_from_folder(files, "./") != 0);
    }
}

TEST_CASE("extractBetween", "[framefinder]") {
    std::string a = "some_test.here";
    std::string b = "some_other_test.here";
    std::string c = "some_test.here.right";
    std::string d = "some_other_test.here.right";
    std::string e = "some_best.other_test.here";
    std::string f = "some.stuff_not_found";
    SECTION("check correct string to pass (long function)") {
        REQUIRE(extractBetween(a, "_", ".") == "test");
        REQUIRE(extractBetween(b, "_", ".") == "test");
        REQUIRE(extractBetween(c, "_", ".") == "test");
        REQUIRE(extractBetween(d, "_", ".") == "test");
        REQUIRE(extractBetween(e, "_", ".") == "test");
        REQUIRE(extractBetween(a, "some", "test") == "_");
        REQUIRE(extractBetween(b, "some", "_test") == "_other");
        REQUIRE(extractBetween(c, "test", "here") == ".");
    }
    SECTION("check correct string to pass (short function)") {
        REQUIRE(extractBetween(a) == "test");
        REQUIRE(extractBetween(b) == "test");
        REQUIRE(extractBetween(c) == "test");
        REQUIRE(extractBetween(d) == "test");
        REQUIRE(extractBetween(e) == "test");
    }
    SECTION("check error/not_found output (long function)") {
        REQUIRE(extractBetween(a, "tst", "xtx") == "NOT_FOUND");
        REQUIRE(extractBetween(f, "_", ".") == "NOT_FOUND");
        REQUIRE(extractBetween(f, "a", "stuff") == "NOT_FOUND");
    }
    SECTION("check error/not_found output (short function)") {
        REQUIRE(extractBetween(f) == "NOT_FOUND");
    }
}
