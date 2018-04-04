#include "processinterface.h".h "

ProcessInterface::ProcessInterface(std::vector<ProcessBase*>* processContainer)
    : m_container(processContainer) {}

namespace {
#define TYPECHECK(typeInfo, T)                            \
    QString::fromStdString(std::string(typeInfo.c_str())) \
        .contains(QString::fromStdString(std::string(typeid(T).name())))
}

std::string ProcessInterface::info(const std::string& typeName, Action action) {
    qDebug() << typeName.c_str();
    qDebug() << typeid(Morph).name();
    if (TYPECHECK(typeName, Morph)) {
        if (action == Action::GetName) {
            return Morph::getName();
        }
    } else if (TYPECHECK(typeName, Binarize)) {
        if (action == Action::GetName) {
            return Binarize::getName();
        }
    }
    return std::string();
}
