#include "parameterdelegate.h"
#include "configurator.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

ParameterDelegate::ParameterDelegate(QAbstractItemModel* model, QObject* parent)
    : QStyledItemDelegate(parent), m_model(model) {}

ParameterDelegate::~ParameterDelegate() {}

QWidget* ParameterDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const {
    // Query index for type
    if (!index.isValid())
        return QStyledItemDelegate::createEditor(parent, option, index);

    ItemUserData itemData = m_model->data(index, Qt::UserRole).value<ItemUserData>();

    // Create proper editor for type, and set range/options
    switch (itemData.type) {
        case ParameterType::Int: {
            QSpinBox* sb = new QSpinBox(parent);
            sb->setRange(itemData.low, itemData.high);
            return sb;
        }
        case ParameterType::Double: {
            QDoubleSpinBox* sb = new QDoubleSpinBox(parent);
            sb->setRange(itemData.low, itemData.high);
            return sb;
        }
        case ParameterType::Enum: {
            QComboBox* cb = new QComboBox(parent);
            for (const auto& opt : itemData.options) {
                cb->addItem(QString::fromStdString(opt));
            }
            return cb;
        }
    }
    Q_ASSERT(false);
    return nullptr;
}

void ParameterDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    // Query index for type
    ItemUserData itemData = m_model->data(index, Qt::UserRole).value<ItemUserData>();

    // Create proper editor for type, and set range/options
    switch (itemData.type) {
        case ParameterType::Int: {
            if (QSpinBox* sb = qobject_cast<QSpinBox*>(editor)) {
                int currentValue = index.data(Qt::EditRole).toString().toInt();
                sb->setValue(currentValue);
                return;
            }
        }
        case ParameterType::Double: {
            if (QDoubleSpinBox* sb = qobject_cast<QDoubleSpinBox*>(editor)) {
                double currentValue = index.data(Qt::EditRole).toString().toDouble();
                sb->setValue(currentValue);
                return;
            }
        }
        case ParameterType::Enum: {
            if (QComboBox* cb = qobject_cast<QComboBox*>(editor)) {
                // get the index of the text in the combobox that matches the current value of the
                // itenm
                QString currentText = index.data(Qt::EditRole).toString();
                int cbIndex = cb->findText(currentText);
                // if it is valid, adjust the combobox
                if (cbIndex >= 0)
                    cb->setCurrentIndex(cbIndex);
                return;
            }
        }
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void ParameterDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                     const QModelIndex& index) const {
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor)) {
        // save the current text of the combo box as the current value of the item
        model->setData(index, cb->currentText(), Qt::EditRole);
    } else if (QSpinBox* sb = qobject_cast<QSpinBox*>(editor)) {
        model->setData(index, QString::number(sb->value()), Qt::EditRole);
    } else if (QDoubleSpinBox* sb = qobject_cast<QDoubleSpinBox*>(editor)) {
        model->setData(index, QString::number(sb->value()), Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
