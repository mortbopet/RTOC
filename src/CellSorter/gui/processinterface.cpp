#include "processinterface.h"

ProcessInterface::ProcessInterface(std::vector<ProcessBase*>* processContainer)
    : m_container(processContainer) {}

namespace {
#define TYPECHECK(typeInfo, T) std::string(typeInfo.c_str()) == std::string(typeid(T).name())
}

template <typename T>
std::string ProcessInterface::executeActionForType(ProcessInterface::Action action, int index) {
    switch (action) {
        case ProcessInterface::Action::GetName: {
            return T::getName();
            break;
        }
        case ProcessInterface::Action::Create: {
            m_container->push_back(new T());
            break;
        }
    }
    return std::string();
}

std::string ProcessInterface::doAction(const std::string& typeName, Action action, int index) {
    if (TYPECHECK(typeName, Morph)) {
        return executeActionForType<Morph>(action, index);
    } else if (TYPECHECK(typeName, Binarize)) {
        return executeActionForType<Binarize>(action, index);
    } else if (TYPECHECK(typeName, Normalize)) {
        return executeActionForType<Normalize>(action, index);
    } else if (TYPECHECK(typeName, SubtractBG)) {
        return executeActionForType<SubtractBG>(action, index);
    } else if (TYPECHECK(typeName, ClearBorder)) {
        return executeActionForType<ClearBorder>(action, index);
    } else if (TYPECHECK(typeName, FloodFill)) {
        return executeActionForType<FloodFill>(action, index);
    } else if (TYPECHECK(typeName, PropFilter)) {
        return executeActionForType<PropFilter>(action, index);
    }
    return std::string();
}
