#include "configurator.h"
#include "ui_configurator.h"

Configurator::Configurator(QWidget* parent) : QWidget(parent), ui(new Ui::Configurator) {
    ui->setupUi(this);
}

Configurator::~Configurator() {
    delete ui;
}

void Configurator::setOptions(const std::vector<std::string>& options) {
    for (const auto& item : options)
        ui->options->addItem(QString::fromStdString(item));
}

void Configurator::on_add_clicked() {
    // Get currently selected option
    const QString option = ui->
}

void Configurator::on_remove_clicked() {}
