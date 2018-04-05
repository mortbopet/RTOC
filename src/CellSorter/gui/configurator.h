#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QWidget>
#include <string>
#include "processinterface.h"
#include "treemodel.h"

namespace Ui {
class Configurator;
}

class Configurator : public QWidget {
    Q_OBJECT

public:
    explicit Configurator(ProcessInterface* interface, QWidget* parent = 0);
    ~Configurator();

private slots:
    void on_add_clicked();
    void on_remove_clicked();

    void updateModel();

private:
    TreeModel* m_model;
    Ui::Configurator* ui;
    ProcessInterface* m_interface;

    void insertChild(const QModelIndex& index, QList<QVariant> values);
    void insertRow(const QModelIndex& index, QList<QVariant> values);
    void removeRow(const QModelIndex& index);
};

#endif  // CONFIGURATOR_H
