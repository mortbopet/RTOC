#include "processinterface.h"

#include <algorithm>
#include <memory>

#include <QDebug>

ProcessInterface::ProcessInterface(processContainerPtr processContainer)
    : m_container(processContainer) {}

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

            parameters[parameterIndex]->setValueStr(value);
            break;
        }
    }
    // Signal that changes has been made to the process and update the model
    emit dataChanged();
}

std::string ProcessInterface::doActionForType(const string& typeName, ProcessTypeAction action) {
    auto retVal = P::queryActionForType(m_container, typeName, action);
    if (action == ProcessTypeAction::Create)
        emit dataChanged();
    return retVal;
}
