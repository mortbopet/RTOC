#include "processinterface.h"

#include <algorithm>
#include <memory>

#include <QMessageBox>

ProcessInterface::ProcessInterface(Analyzer* analyzer) : m_analyzer(analyzer) {}

// Type agnostic actions
void ProcessInterface::doAction(Action action, int processIndex, int parameterIndex,
                                std::string value) {
    switch (action) {
        case ProcessInterface::Action::Remove: {
            // Thank you, smart pointers. The underlying data is destructed upon erasing
            m_analyzer->getProcessContainerPtr()->erase(
                m_analyzer->getProcessContainerPtr()->begin() + processIndex);
            break;
        }
        case ProcessInterface::Action::Up: {
            auto item = m_analyzer->getProcessContainerPtr()->begin() + processIndex;
            if (item != m_analyzer->getProcessContainerPtr()->begin()) {
                std::iter_swap(item, item - 1);
            }
            break;
        }
        case ProcessInterface::Action::Down: {
            auto item = m_analyzer->getProcessContainerPtr()->begin() + processIndex;
            if (item != (m_analyzer->getProcessContainerPtr()->end() - 1)) {
                std::iter_swap(item, item + 1);
            }
            break;
        }
        case ProcessInterface::Action::SetValue: {
            auto parameters =
                (*m_analyzer->getProcessContainerPtr())[processIndex]->getParameters();

            parameters[parameterIndex]->setValueStr(value);
            break;
        }
    }
    // Signal that changes has been made to the process and update the model
    emit dataChanged();
}

std::string ProcessInterface::doActionForType(const string& typeName, ProcessTypeAction action) {
    auto retVal = P::queryActionForType(m_analyzer->getProcessContainerPtr(), typeName, action);
    if (action == ProcessTypeAction::Create)
        emit dataChanged();
    return retVal;
}

void ProcessInterface::loadSetup(const QString& path) {
    if (!m_analyzer->loadSetup(path.toStdString())) {
        QMessageBox::warning(nullptr, QString("Error"), QString("Could not load setup file"));
    } else {
        emit dataChanged();
    }
}

void ProcessInterface::storeSetup(const QString& path) const {
    if (!m_analyzer->storeSetup(path.toStdString())) {
        QMessageBox::warning(nullptr, QString("Error"), QString("Could not store setup file"));
    }
}
