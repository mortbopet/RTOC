#include "catch.hpp"

#include <iostream>
#include <memory>
#include <string>
#include "../lib/parameter.h"

// Unit testing for Parameter class

TEST_CASE("Parameter test", "[full], [parameter]") {
    std::vector<ParameterBase*> parameters;
    ValueParameter<int> intParm = ValueParameter<int>(parameters, "Integer parameter");
    ValueParameter<double> doubleParm = ValueParameter<double>(parameters, "Double parameter");
    enum TestEnum { A, B, C, D };
    EnumParameter<TestEnum> enumParm = EnumParameter<TestEnum>(parameters, "Enumerated parameter");

    SECTION("Test Integer value parameters") { REQUIRE_THROWS(intParm.getValue()); }

    intParm.setRange(0, 100);
    intParm.setValue(30);

    SECTION("Test Double value parameters") {
        REQUIRE_THROWS(doubleParm.getValue());
    }

    doubleParm.setRange(-143.40, 1341.40);
    doubleParm.setValue(134.1345);

    SECTION("Test Enum parameters") { REQUIRE_THROWS(enumParm.getValue()); }

    enumParm.setOptions(map<TestEnum, string>{{TestEnum::A, "Value_A"},
                                              {TestEnum::B, "Value_B"},
                                              {TestEnum::C, "Value_C"},
                                              {TestEnum::D, "Value_D"}});
    // And its value
    enumParm.setValue(TestEnum::B);

    // Loop through $parameters, deduce the parameter type and print the valid options/ranges

    SECTION("Test parameter retrieval and type deduction") {
        for (auto& parameter : parameters) {
            auto optionStream = parameter->getOptions();
            std::string type;
            optionStream >> type;
            if (type == typeid(int).name()) {
                // GUI can spawn an editor that accepts integer values in
                int high, low;
                getRange<int>(optionStream, low, high);
                REQUIRE((low == 0 && high == 100));
                REQUIRE(stoi(parameter->getValueStr()) == 30);
            } else if (type == typeid(double).name()) {
                // GUI can spawn an editor that accepts floating point values
                double high, low;
                getRange<double>(optionStream, low, high);
                REQUIRE((low == -143.40 && high == 1341.40));
                REQUIRE(stod(parameter->getValueStr()) == 134.1345);
            } else {
                // GUI can spawn an editor that accepts ENUMs - gui will generate a combobox with
                // values that options provides
                std::vector<string> options;
                getOptions(optionStream, options);
                REQUIRE(options == std::vector<string>{"Value_A", "Value_B", "Value_C", "Value_D"});
                REQUIRE(parameter->getValueStr() == "Value_B");
            }
        }
    }
}
int test_parameterClass() {
    return 0;
}
