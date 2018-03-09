#include <memory>
#include "../lib/parameter.h"

// Unit testing for Parameter class

int tst_parameterClass() {
    std::vector<ParameterBase*> parameters;

    // Create various parameters for testing
    IntParmPtr intParm = std::make_unique<Parameter<int>>(parameters, "Integer parameter");
    intParm->setRange(0, 100);

    DoubleParmPtr doubleParm = std::make_unique<Parameter<double>>(parameters, "Double parameter");
    doubleParm->setRange(-143.40, 1341.40);

    enum TestEnum { A, B, C, D };
    using TestEnumParmPtr = std::unique_ptr<Parameter<TestEnum>>;
    TestEnumParmPtr enumParm =
        std::make_unique<Parameter<TestEnum>>(parameters, "Enumerated parameter");
    enumParm->setOptions(std::vector<std::pair<int, std::string>>{{TestEnum::A, "Value A"},
                                                                  {TestEnum::B, "Value B"},
                                                                  {TestEnum::C, "Value C"},
                                                                  {TestEnum::D, "Value D"}});

    // Loop through $parameters, deduce the parameter type and print the valid options/ranges
    for (auto& parameter : parameters) {
        parameter->printInfo();
    }

    return 0;
}
