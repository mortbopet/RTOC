#include "configurator.h"
#include "ui_configurator.h"

#include "guihelpers.h"
#include "treeitem.h"

#include <QFileDialog>
#include <QHeaderView>

Configurator::Configurator(ProcessInterface* iface, Analyzer* analyzer, QWidget* parent)
    : m_interface(iface), QWidget(parent), ui(new Ui::Configurator) {
    ui->setupUi(this);

    // Set icons
    ui->up->setIcon(QIcon(":/icons/resources/up-arrow.svg"));
    ui->down->setIcon(QIcon(":/icons/resources/down-arrow.svg"));
    ui->add->setIcon(QIcon(":/icons/resources/add.svg"));
    ui->remove->setIcon(QIcon(":/icons/resources/delete.svg"));
    ui->load->setIcon(QIcon(":/icons/resources/load.svg"));
    ui->store->setIcon(QIcon(":/icons/resources/save.svg"));

    // create image displayer for previewing processed images
    m_imagedisplayer = new ImageDisplayerWidget();
    ui->previewLayout->addWidget(m_imagedisplayer);

    // Set an analyzer for the image configurator so it will preview processed images
    m_imagedisplayer->setAnalyzer(analyzer);

    // Gather options from interface
    auto processTypes = m_interface->getProcessTypes();
    for (const auto& type : processTypes) {
        QString processName =
            QString::fromStdString(m_interface->doActionForType(type, ProcessTypeAction::GetName));
        auto item = new QListWidgetItem();
        // Items are identified by their UserRole, which corresponds to they typeid(T).name()
        item->setData(Qt::DisplayRole, processName);
        ItemUserData itemData;
        itemData.processtype = QString::fromStdString(type);
        item->setData(Qt::UserRole, QVariant::fromValue(itemData));
        ui->options->addItem(item);
    }

    // Setup tree view
    m_model = new TreeModel(m_interface->getContainerPtr(), iface);
    ui->tree->setModel(m_model);
    connect(m_interface, &ProcessInterface::dataChanged, this, &Configurator::updateModel);

    // Setup tree header
    ui->tree->header()->setSectionsMovable(false);

    ui->tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tree->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->tree->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tree->header()->setSectionResizeMode(3, QHeaderView::Interactive);

    // Connect double-click in options list to create new processes
    connect(ui->options, &QListWidget::itemDoubleClicked, this, &Configurator::on_add_clicked);

    // Setup delegate for parameter tree
    m_delegate = new ParameterDelegate(m_model, ui->tree);
    ui->tree->setItemDelegate(m_delegate);

    // Reload model
    updateModel();
}

void Configurator::updateModel() {
    // prevent setData() calls in model to send data changes to the ProcessInterface
    m_model->setModelLoading(true);
    while (m_model->rowCount() > 0) {
        int row = m_model->rowCount() - 1;
        removeRow(m_model->index(row, 0));
    }

    // populate with current process
    int processIndex = 1;
    for (auto& process : *m_interface->getContainerPtr()) {
        QString type = QString::fromStdString(
            m_interface->doActionForType(process->getTypeName(), ProcessTypeAction::GetName));

        insertRow(m_model->index(m_model->rowCount() - 1, 0), QList<QVariant>()
                                                                  << processIndex << type << ""
                                                                  << "");
        auto parameters = process->getParameters();
        for (const auto& parameter : parameters) {
            auto optionStream = parameter->getOptions();
            std::string type, name;
            getParameterStreamToken(optionStream, type);
            getParameterStreamToken(optionStream, name);
            if (type == typeid(int).name()) {
                // GUI can spawn an editor that accepts integer values in
                int high, low;
                string value = parameter->getValueStr();
                getRange<int>(optionStream, low, high);

                QList<QVariant> values;
                values << ""
                       << "" << QString::fromStdString(name).replace('_', ' ') << stoi(value);
                QModelIndex child = insertChild(m_model->index(m_model->rowCount() - 1, 0), values);
                // Set tooltip for parameter
                m_model->getItem(child)->setData(
                    0, QString("Valid range: [%1;%2]").arg(low).arg(high), Qt::ToolTipRole);
                // Set data to be used by delegate
                ItemUserData itemData;
                itemData.low = low;
                itemData.high = high;
                itemData.type = ParameterType::Int;
                m_model->setData(child, QVariant::fromValue(itemData), Qt::UserRole);
            } else if (type == typeid(double).name()) {
                // GUI can spawn an editor that accepts floating point values
                double high, low;
                std::string value = parameter->getValueStr();
                getRange<double>(optionStream, low, high);

                QList<QVariant> values;
                // "" is added to account for the process index column
                values << ""
                       << "" << QString::fromStdString(name).replace('_', ' ') << stod(value);
                QModelIndex child = insertChild(m_model->index(m_model->rowCount() - 1, 0), values);

                // Set tooltip for parameter
                m_model->getItem(child)->setData(
                    0, QString("Valid range: [%1;%2]").arg(low).arg(high), Qt::ToolTipRole);
                // Set data to be used by delegate
                ItemUserData itemData;
                itemData.low = low;
                itemData.high = high;
                itemData.type = ParameterType::Double;
                m_model->setData(child, QVariant::fromValue(itemData), Qt::UserRole);
            } else {
                // GUI can spawn an editor that accepts ENUMs - gui will generate a combobox with
                // values that options provides
                std::vector<string> options;
                getOptions(optionStream, options);
                string value = parameter->getValueStr();

                QList<QVariant> values;
                values << ""
                       << "" << QString::fromStdString(name).replace('_', ' ')
                       << QString::fromStdString(value);
                QModelIndex child = insertChild(m_model->index(m_model->rowCount() - 1, 0), values);

                // No tooltip for enumeration values - combo box delegate parameters should be
                // sufficient
                // Set data to be used by delegate
                ItemUserData itemData;
                itemData.type = ParameterType::Enum;
                itemData.options = options;
                m_model->setData(child, QVariant::fromValue(itemData), Qt::UserRole);
            }
        }
        processIndex++;
    }
    m_model->setModelLoading(false);

    // refresh the preview image
    m_imagedisplayer->refreshImage();
}

QModelIndex Configurator::insertChild(const QModelIndex& index, QList<QVariant> values) {
    QAbstractItemModel* model = ui->tree->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return QModelIndex();
    }
    int childRow = m_model->getItem(index)->childCount();
    if (!model->insertRow(childRow, index))
        return QModelIndex();
    QModelIndex child;
    for (int column = 0; column < model->columnCount(index); ++column) {
        child = model->index(childRow, column, index);
        model->setData(child, values[column], Qt::EditRole | Qt::DisplayRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, values[column],
                                 Qt::EditRole | Qt::DisplayRole);
    }

    ui->tree->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                QItemSelectionModel::ClearAndSelect);
    return child;
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
        ItemUserData itemData = selectedItem->data(Qt::UserRole).value<ItemUserData>();
        m_interface->doActionForType(itemData.processtype.toStdString(), ProcessTypeAction::Create);
    }
}

void Configurator::on_remove_clicked() {
    // Get selected indexes. Only óne row can be selected at a time
    int row = getRootSelectedIndex(ui->tree->selectionModel());
    if (row >= 0) {
        m_interface->doAction(ProcessInterface::Action::Remove, row);
    }
}

void Configurator::reorder(ProcessInterface::Action dir) {
    Q_ASSERT(dir == ProcessInterface::Action::Up || dir == ProcessInterface::Action::Down);

    // Get selected indexes. Only óne row can be selected at a time
    int row = getRootSelectedIndex(ui->tree->selectionModel());
    if (row >= 0) {
        m_interface->doAction(dir, row);
    }
}

void Configurator::on_up_clicked() {
    reorder(ProcessInterface::Action::Up);
}

void Configurator::on_down_clicked() {
    reorder(ProcessInterface::Action::Down);
}

void Configurator::on_load_clicked() {
    auto filename = QFileDialog::getOpenFileName(this, "Open process configuration file",
                                                 m_current_dir, "pcs file (*.pcs)");
    if (!filename.isNull())
        m_interface->loadSetup(filename);
}

void Configurator::on_store_clicked() {
    auto defaultDir = QDir::currentPath();
    auto filename = QFileDialog::getSaveFileName(this, "Save process configuration file",
                                                 m_current_dir, "pcs file (*.pcs)");
    if (!filename.isNull())
        m_interface->storeSetup(filename);
}

template <class Archive>
void Configurator::serialize(Archive& ar, const unsigned int version) const {
    ar& boost::serialization::make_nvp("imagedisplayerwidget", *m_imagedisplayer);
}

EXPLICIT_INSTANTIATE_XML_ARCHIVE(Configurator)
