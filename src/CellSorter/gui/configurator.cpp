#include "configurator.h"
#include "ui_configurator.h"

Configurator::Configurator(ProcessInterface* interface, QWidget* parent)
    : m_interface(interface), QWidget(parent), ui(new Ui::Configurator) {
    ui->setupUi(this);

    // Gather options from interface
    auto processTypes = m_interface->getProcessTypes();
    for (const auto& type : processTypes) {
        QString processName =
            QString::fromStdString(m_interface->info(type, ProcessInterface::Action::GetName));
        ui->options->addItem(processName);
    }
}

Configurator::~Configurator() {
    delete ui;
}

void Configurator::on_add_clicked() {
    // Get currently selected option
}

void Configurator::on_remove_clicked() {}
