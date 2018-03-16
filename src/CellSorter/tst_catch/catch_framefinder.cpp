#include "catch.hpp"

#include "../lib/framefinder.h"

TEST_CASE("framefinder", "[framefinder]") {
    SECTION("framefinder: exists()", "[framefinder]") {
        REQUIRE(exists("./Makefile"));
        REQUIRE(!exists("./Makefsidfj"));
        REQUIRE(exists("./CMakeFiles/"));
    }

    SECTION("framefinder: files_from_folder()", "[framefinder]") {
        std::vector<std::string> files;
        REQUIRE(files_from_folder(files, "./") != 0);
    }

    SECTION("framefinder: extractBetween()", "[framefinder]") {
        std::string a = "some_test.here";
        std::string b = "some_other_test.here";
        std::string c = "some_test.here.right";
        std::string d = "some_other_test.here.right";
        REQUIRE(extractBetween(a) == "test");
        REQUIRE(extractBetween(b) == "test");
        REQUIRE(extractBetween(c) == "test");
        REQUIRE(extractBetween(d) == "test");
        REQUIRE(extractBetween(a, "_", ".") == "test");
        REQUIRE(extractBetween(b, "_", ".") == "test");
        REQUIRE(extractBetween(c, "_", ".") == "test");
        REQUIRE(extractBetween(d, "_", ".") == "test");
    }
}

/*  std::string default_testdir = "./";

    /// Filedetector test
    if (!exists("./Makefile")) {
        return -1;
    }
    if (exists("./Makefilex")) {
        return -1;
    }
    if (!exists("./CMakeFiles/")) {
        return -1;
    }
    if (exists("./CMakeFilesx/")) {
        return -1;
    }
 */
