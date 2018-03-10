#include <iostream>
#include <memory>
#include <string>
#include "../lib/parameter.h"

// Unit testing for Parameter class

int tst_parameterClass() {
    std::vector<ParameterBase*> parameters;

    // Create various parameters for testing - this is how parameters are created in Process
    // classes. Refer to the Process' parameter container, and use. since these parameters can be
    // placed in the public scope of a Process, we can directly access parameters through code
    ValueParameter<int> intParm = ValueParameter<int>(parameters, "Integer parameter");
    // We can directly go in and change the valid range of a ValueParameter, as well as set its
    // value
    intParm.setRange(0, 100);
    intParm.setValue(30);

    ValueParameter<double> doubleParm = ValueParameter<double>(parameters, "Double parameter");
    doubleParm.setRange(-143.40, 1341.40);

    enum TestEnum { A, B, C, D };
    EnumParameter<TestEnum> enumParm = EnumParameter<TestEnum>(parameters, "Enumerated parameter");
    // We can directly access the EnumParm to set its options
    enumParm.setOptions(map<TestEnum, string>{{TestEnum::A, "Value A"},
                                              {TestEnum::B, "Value B"},
                                              {TestEnum::C, "Value C"},
                                              {TestEnum::D, "Value D"}});
    // And its value
    enumParm.setValue(TestEnum::B);

    // Now, why go through all this trouble to create properties?

    // With a potential gui, we load in a predefined list of processes - each process has a range of
    // different parameters, and to avoid giant switch statements, we can deduce the parameter types
    // from ParameterBase, and get/set the options from there - using a limited amount of code.
    // We can then use the provided functions from ValueParameter/EnumParameter to write the GUI
    // code which handles user input in terms of allowable input range, and for enum types, to
    // display a combobox with the valid values for a parameter. All without actually having to know
    // the specific enum name used deep-down in the code.

    // Loop through $parameters, deduce the parameter type and print the valid options/ranges
    for (auto& parameter : parameters) {
        switch (parameter->getType()) {
            case ParameterType::INT: {
                auto p = static_cast<ValueParameter<int>*>(parameter);
                cout << p->getRange().first << " " << p->getRange().second << endl;
                break;
            }
            case ParameterType::CHAR: {
                auto p = static_cast<ValueParameter<char>*>(parameter);
                cout << p->getRange().first << " " << p->getRange().second << endl;
                break;
            }
            case ParameterType::FLOAT: {
                auto p = static_cast<ValueParameter<float>*>(parameter);
                cout << p->getRange().first << " " << p->getRange().second << endl;
                break;
            }
            case ParameterType::DOUBLE: {
                auto p = static_cast<ValueParameter<double>*>(parameter);
                cout << p->getRange().first << " " << p->getRange().second << endl;
                break;
            }
            case ParameterType::ENUM: {
                auto p = static_cast<EnumParameter<int>*>(parameter);
                vector<string> options = p->getOptions();
                for (const auto& option : options) {
                    cout << option << endl;
                }
                break;
            }
        }
    }

    return 0;
}
