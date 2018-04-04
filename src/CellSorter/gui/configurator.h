#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QWidget>
#include <string>
#include "processinterface.h"

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

private:
    Ui::Configurator* ui;
    ProcessInterface* m_interface;
};

#endif  // CONFIGURATOR_H
