#ifndef PROCESSINTERFACE_H
#define PROCESSINTERFACE_H

#include <QObject>
#include <typeinfo>
#include "../lib/process.h"

#include "../lib/analyzer.h"

class ProcessInterface : public QObject {
    Q_OBJECT
public:
    enum class Action { Up, Down, Remove, SetValue };
    ProcessInterface(Analyzer* analyzer);

    processContainerPtr getContainerPtr() { return m_analyzer->getProcessContainerPtr(); }
    void doAction(Action action, int processIndex = -1, int parameterIndex = -1,
                  std::string valueStr = std::string());
    std::string doActionForType(const std::string& typeName, ProcessTypeAction action);
    void setParameter(int processIndex, int parameterIndex, std::string valueString);
    const std::vector<std::string>& getProcessTypes() { return ProcessBase::get_processes(); }
    void emitDataChanged() { emit dataChanged(); }

public slots:
    void storeSetup(const QString& path) const;
    void loadSetup(const QString& path);

signals:
    void dataChanged(void);

private:
    Analyzer* m_analyzer;
    void push_back(std::unique_ptr<ProcessBase> p) {
        m_analyzer->getProcessContainerPtr()->push_back(std::move(p));
    }
    void clear() { m_analyzer->getProcessContainerPtr()->clear(); }
};

#endif  // PROCESSINTERFACE_H
