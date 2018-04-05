#include "processinterface.h"

#include <algorithm>
#include <memory>

ProcessInterface::ProcessInterface(processContainerPtr processContainer)
    : m_container(processContainer) {}

namespace {
#define TYPECHECK(typeInfo, T) typeInfo == std::string(typeid(T).name())
}

template <typename T>
std::string ProcessInterface::executeActionForType(ProcessInterface::Action action, int index) {
    switch (action) {
        case ProcessInterface::Action::GetName: {
            return T::getName();
            break;
        }
        case ProcessInterface::Action::Create: {
            m_container->push_back(std::make_unique<T>());
            break;
        }
        case ProcessInterface::Action::Remove: {
            // Thank you, smart pointers. The underlying data is destructed upon erasing
            m_container->erase(m_container->begin() + index);
            break;
        }
        case ProcessInterface::Action::Up: {
            auto item = m_container->begin() + index;
            if (item != m_container->begin()) {
                std::iter_swap(item, item - 1);
            }
            break;
        }
        case ProcessInterface::Action::Down: {
            auto item = m_container->begin() + index;
            if (item != (m_container->end() - 1)) {
                std::iter_swap(item, item + 1);
            }
            break;
        }
    }
    // Signal that changes has been made to the process and update the model
    emit dataChanged();
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
    } else if (TYPECHECK(typeName, FloodFillProcess)) {
        return executeActionForType<FloodFillProcess>(action, index);
    } else if (TYPECHECK(typeName, PropFilter)) {
        return executeActionForType<PropFilter>(action, index);
    } else if (TYPECHECK(typeName, Canny)) {
        return executeActionForType<Canny>(action, index);
    }
    Q_ASSERT(false);
}
