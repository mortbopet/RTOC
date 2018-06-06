#include "catch.hpp"

#include "../lib/framefinder.h"

TEST_CASE("exists()", "[full], [framefinder]") {
    SECTION("basic operation") {
        REQUIRE(framefinder::exists("./Makefile"));
        REQUIRE(!framefinder::exists("./Makefsidfj"));
        REQUIRE(framefinder::exists("./CMakeFiles/"));
        REQUIRE(framefinder::exists("./CMakeFiles/CellSorter_test.dir/"));
    }
}

TEST_CASE("files_from_folder()", "[full], [framefinder]") {
    std::vector<std::string> files;
    SECTION("check if able to find files in build dir") {
        REQUIRE(framefinder::files_from_folder(files, "./") != 0);
    }
    SECTION("check error output when folder doesn't exist") {
        REQUIRE(framefinder::files_from_folder(files, "./x_x_x/") == -1);
    }
}

TEST_CASE("framefinder::extractBetween", "[full], [framefinder]") {
    std::string a = "some_test.here";
    std::string b = "some_other_test.here";
    std::string c = "some_test.here.right";
    std::string d = "some_other_test.here.right";
    std::string e = "some_best.other_test.here";
    std::string f = "some.stuff_not_found";
    SECTION("check correct string to pass (long function)") {
        REQUIRE(framefinder::extractBetween(a, "_", ".") == "test");
        REQUIRE(framefinder::extractBetween(b, "_", ".") == "test");
        REQUIRE(framefinder::extractBetween(c, "_", ".") == "test");
        REQUIRE(framefinder::extractBetween(d, "_", ".") == "test");
        REQUIRE(framefinder::extractBetween(e, "_", ".") == "test");
        REQUIRE(framefinder::extractBetween(a, "some", "test") == "_");
        REQUIRE(framefinder::extractBetween(b, "some", "_test") == "_other");
        REQUIRE(framefinder::extractBetween(c, "test", "here") == ".");
    }
    SECTION("check correct string to pass (short function)") {
        REQUIRE(framefinder::extractBetween(a) == "test");
        REQUIRE(framefinder::extractBetween(b) == "test");
        REQUIRE(framefinder::extractBetween(c) == "test");
        REQUIRE(framefinder::extractBetween(d) == "test");
        REQUIRE(framefinder::extractBetween(e) == "test");
    }
    SECTION("check error/not_found output (long function)") {
        REQUIRE(framefinder::extractBetween(a, "tst", "xtx") == "NOT_FOUND");
        REQUIRE(framefinder::extractBetween(f, "_", ".") == "NOT_FOUND");
        REQUIRE(framefinder::extractBetween(f, "a", "stuff") == "NOT_FOUND");
    }
    SECTION("check error/not_found output (short function)") {
        REQUIRE(framefinder::extractBetween(f) == "NOT_FOUND");
    }
}

// TEST_CASE("get_files", "[full], [framefinder]") {
//     std::vector<Frame> frames;
//     std::string folder = "./";
// }
