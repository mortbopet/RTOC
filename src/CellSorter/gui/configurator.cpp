#include "configurator.h"
#include "ui_configurator.h"

#include <QHeaderView>

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

    // Setup tree view
    QStringList headers;
    headers << "Process"
            << "Value";
    m_model = new TreeModel(m_interface->getContainerPtr(), interface);
    ui->tree->setModel(m_model);
    connect(m_interface, &ProcessInterface::dataChanged, this, &Configurator::updateModel);
    ui->tree->header()->setSectionResizeMode(QHeaderView::Stretch);

    // Reload model
    updateModel();
}

void Configurator::updateModel() {
    while (m_model->rowCount() > 0) {
        int row = m_model->rowCount() - 1;
        removeRow(m_model->index(row, 0));
    }

    // populate with current process
    for (auto& process : *m_interface->getContainerPtr()) {
        QString type = QString::fromStdString(
            m_interface->doAction(process->getTypeName(), ProcessInterface::Action::GetName));

        insertRow(m_model->index(m_model->rowCount() - 1, 0), QList<QVariant>() << type << "");
        auto parameters = process->getParameters();
        for (const auto& parameter : parameters) {
            auto optionStream = parameter->getOptions();
            std::string type, name;
            optionStream >> type >> name;
            if (type == typeid(int).name()) {
                // GUI can spawn an editor that accepts integer values in
                int high, low;
                string value = parameter->getValueStr();
                getRange<int>(optionStream, low, high);

                QList<QVariant> values;
                values << QString::fromStdString(name).replace('_', ' ') << stoi(value);
                insertChild(m_model->index(m_model->rowCount() - 1, 0), values);

            } else if (type == typeid(double).name()) {
                // GUI can spawn an editor that accepts floating point values
                double high, low;
                std::string value = parameter->getValueStr();
                getRange<double>(optionStream, low, high);

                QList<QVariant> values;
                values << QString::fromStdString(name).replace('_', ' ') << stod(value);
                insertChild(m_model->index(m_model->rowCount() - 1, 0), values);
            } else {
                // GUI can spawn an editor that accepts ENUMs - gui will generate a combobox with
                // values that options provides
                std::vector<string> options;
                getOptions(optionStream, options);
                string value = parameter->getValueStr();

                QList<QVariant> values;
                values << QString::fromStdString(options[0]).replace('_', ' ')
                       << QString::fromStdString(value);
                insertChild(m_model->index(m_model->rowCount() - 1, 0), values);
            }
        }
    }
}

void Configurator::insertChild(const QModelIndex& index, QList<QVariant> values) {
    QAbstractItemModel* model = ui->tree->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, values[column], Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, values[column], Qt::EditRole);
    }

    ui->tree->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                QItemSelectionModel::ClearAndSelect);
}

void Configurator::insertRow(const QModelIndex& index, QList<QVariant> values) {
    QAbstractItemModel* model = ui->tree->model();

    if (!model->insertRow(index.row() + 1, index.parent()))
        return;

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row() + 1, column, index.parent());
        model->setData(child, values[column], Qt::EditRole);
    }
}

void Configurator::removeRow(const QModelIndex& index) {
    QAbstractItemModel* model = ui->tree->model();
    model->removeRow(index.row(), index.parent());
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

namespace {
int getRootSelectedIndex(QItemSelectionModel* model) {
    auto indexes = model->selectedIndexes();
    if (!indexes.isEmpty()) {
        QModelIndex index = indexes.first();
        if (index.isValid()) {
            while (index.parent().isValid()) {
                index = index.parent();
            }
        }
        return index.row();
    } else {
        return -1;
    }
}
}  // namespace

void Configurator::on_remove_clicked() {
    // Get selected indexes. Only óne row can be selected at a time
    int row = getRootSelectedIndex(ui->tree->selectionModel());
    if (row >= 0) {
        m_interface->doAction(std::string(typeid(Morph).name()), ProcessInterface::Action::Remove,
                              row);
    }
}

void Configurator::reorder(ProcessInterface::Action dir) {
    Q_ASSERT(dir == ProcessInterface::Action::Up || dir == ProcessInterface::Action::Down);

    // Get selected indexes. Only óne row can be selected at a time
    int row = getRootSelectedIndex(ui->tree->selectionModel());
    if (row >= 0) {
        m_interface->doAction(std::string(typeid(Morph).name()), dir, row);
    }
}

void Configurator::on_up_clicked() {
    reorder(ProcessInterface::Action::Up);
}

void Configurator::on_down_clicked() {
    reorder(ProcessInterface::Action::Down);
}
