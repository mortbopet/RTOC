#include "processinterface.h"

#include <algorithm>
#include <memory>

ProcessInterface::ProcessInterface(processContainerPtr processContainer)
    : m_container(processContainer) {}

namespace {
#define TYPECHECK(typeInfo, T) typeInfo == std::string(typeid(T).name())
}

template <typename T>
std::string ProcessInterface::executeActionForType(ProcessInterface::TypeAction action, int index) {
    Q_UNUSED(index)
    switch (action) {
        case ProcessInterface::TypeAction::GetName: {
            return T::getName();
            break;
        }
        case ProcessInterface::TypeAction::Create: {
            m_container->push_back(std::make_unique<T>());
            break;
        }
    }
    // Signal that changes has been made to the process and update the model
    emit dataChanged();
    return std::string();
}

// Type agnostic actions
void ProcessInterface::doAction(Action action, int processIndex, int parameterIndex,
                                std::string value) {
    switch (action) {
        case ProcessInterface::Action::Remove: {
            // Thank you, smart pointers. The underlying data is destructed upon erasing
            m_container->erase(m_container->begin() + processIndex);
            break;
        }
        case ProcessInterface::Action::Up: {
            auto item = m_container->begin() + processIndex;
            if (item != m_container->begin()) {
                std::iter_swap(item, item - 1);
            }
            break;
        }
        case ProcessInterface::Action::Down: {
            auto item = m_container->begin() + processIndex;
            if (item != (m_container->end() - 1)) {
                std::iter_swap(item, item + 1);
            }
            break;
        }
        case ProcessInterface::Action::SetValue: {
            auto parameters = (*m_container)[processIndex]->getParameters();

            parameters[processIndex]->setValueStr(value);
            break;
        }
    }
    // Signal that changes has been made to the process and update the model
    emit dataChanged();
}

// Type specific actions
std::string ProcessInterface::doActionForType(const std::string& typeName, TypeAction action,
                                              int index) {
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
    } else if (TYPECHECK(typeName, FloodFillProcess)) {
        return executeActionForType<FloodFillProcess>(action, index);
    } else if (TYPECHECK(typeName, PropFilter)) {
        return executeActionForType<PropFilter>(action, index);
    } else if (TYPECHECK(typeName, Canny)) {
        return executeActionForType<Canny>(action, index);
    }
    Q_ASSERT(false);
}
