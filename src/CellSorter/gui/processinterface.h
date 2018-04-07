#ifndef PROCESSINTERFACE_H
#define PROCESSINTERFACE_H

#include <QObject>
#include <typeinfo>
#include "../lib/process.h"

class ProcessInterface : public QObject {
    Q_OBJECT
public:
    enum class Action { Up, Down, Remove, Create, GetName };
    ProcessInterface(processContainerPtr processContainer);

    processContainerPtr getContainerPtr() { return m_container; }
    std::string doAction(const std::string& typeName, Action action, int index = -1);
    void setParameter(int processIndex, int parameterIndex, std::string valueString);
    const std::vector<std::string>& getProcessTypes() { return ProcessBase::get_processes(); }
    void emitDataChanged() { emit dataChanged(); }

    processContainerPtr m_container;
signals:
    void dataChanged(void);

private:
    void push_back(std::unique_ptr<ProcessBase> p) { m_container->push_back(std::move(p)); }
    void clear() { m_container->clear(); }

    template <typename T>
    std::string executeActionForType(ProcessInterface::Action action, int index);
};

#endif  // PROCESSINTERFACE_H
