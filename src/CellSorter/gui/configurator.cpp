#include "configurator.h"
#include "ui_configurator.h"

Configurator::Configurator(ProcessInterface* interface, QWidget* parent)
    : m_interface(interface), QWidget(parent), ui(new Ui::Configurator) {
    ui->setupUi(this);

    // Gather options from interface
    auto processTypes = m_interface->getProcessTypes();
    for (const auto& type : processTypes) {
        QString processName =
            QString::fromStdString(m_interface->doAction(type, ProcessInterface::Action::GetName));
        auto item = new QListWidgetItem();
        // Items are identified by their UserRole, which corresponds to they typeid(T).name()
        item->setData(Qt::DisplayRole, processName);
        item->setData(Qt::UserRole, QString::fromStdString(type));
        ui->options->addItem(item);
    }
}

Configurator::~Configurator() {
    delete ui;
}

void Configurator::on_add_clicked() {
    // Get currently selected option
    auto selectedItem = ui->options->selectedItems().first();
    if (selectedItem != nullptr) {
        // Get class typeid name and create a new process
        QString typeName = selectedItem->data(Qt::UserRole).toString();
        m_interface->doAction(typeName.toStdString(), ProcessInterface::Action::Create);
    }
}

void Configurator::on_remove_clicked() {}
