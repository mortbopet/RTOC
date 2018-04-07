#ifndef PROCESSINTERFACE_H
#define PROCESSINTERFACE_H

#include <QObject>
#include <typeinfo>
#include "../lib/process.h"

class ProcessInterface : public QObject {
    Q_OBJECT
public:
    enum class Action { Up, Down, Remove, SetValue };
    enum class TypeAction { Create, GetName };
    ProcessInterface(processContainerPtr processContainer);

    processContainerPtr getContainerPtr() { return m_container; }
    std::string doActionForType(const std::string& typeName, TypeAction action, int index = -1);
    void doAction(Action action, int processIndex = -1, int parameterIndex = -1,
                  std::string valueStr = std::string());
    void setParameter(int processIndex, int parameterIndex, std::string valueString);
    const std::vector<std::string>& getProcessTypes() { return ProcessBase::get_processes(); }
    void emitDataChanged() { emit dataChanged(); }

signals:
    void dataChanged(void);

private:
    processContainerPtr m_container;
    void push_back(std::unique_ptr<ProcessBase> p) { m_container->push_back(std::move(p)); }
    void clear() { m_container->clear(); }

    template <typename T>
    std::string executeActionForType(ProcessInterface::TypeAction action, int index);
};

#endif  // PROCESSINTERFACE_H
