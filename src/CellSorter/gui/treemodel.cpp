/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "treeitem.h"
#include "treemodel.h"

int getRootIndex(QModelIndex index) {
    if (index.isValid()) {
        while (index.parent().isValid()) {
            index = index.parent();
        }
        return index.row();
    } else {
        return -1;
    }
}

int getRootSelectedIndex(QItemSelectionModel* model) {
    auto indexes = model->selectedIndexes();
    if (!indexes.isEmpty()) {
        return getRootIndex(indexes.first());
    } else {
        return -1;
    }
}

//! [0]
TreeModel::TreeModel(processContainerPtr container, ProcessInterface* iface, QObject* parent)
    : m_container(container), m_interface(iface), QAbstractItemModel(parent) {
    QStringList headers{"Process name", "Parameters"};
    QVector<QVariant> rootData;
    foreach (QString header, headers)
        rootData << header;

    rootItem = new TreeItem(rootData);
}
//! [0]

//! [1]
TreeModel::~TreeModel() {
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex& /* parent */) const {
    return rootItem->columnCount();
}
//! [2]

QVariant TreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TreeItem* item = getItem(index);

    return item->data(index.column());
}

//! [3]
Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.parent().isValid()) {  // root items are not editable
        if (index.column() != 0) {
            // Items in column 0 is never editable
            flags |= Qt::ItemIsEditable;
        }
    }
    return flags;
}
//! [3]

//! [4]
TreeItem* TreeModel::getItem(const QModelIndex& index) const {
    if (index.isValid()) {
        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}
//! [4]

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

//! [5]
QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
    //! [5]

    //! [6]
    TreeItem* parentItem = getItem(parent);

    TreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

bool TreeModel::insertColumns(int position, int columns, const QModelIndex& parent) {
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex& parent) {
    TreeItem* parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

//! [7]
QModelIndex TreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    TreeItem* childItem = getItem(index);
    TreeItem* parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}
//! [7]

bool TreeModel::removeColumns(int position, int columns, const QModelIndex& parent) {
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex& parent) {
    TreeItem* parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

//! [8]
int TreeModel::rowCount(const QModelIndex& parent) const {
    TreeItem* parentItem = getItem(parent);

    return parentItem->childCount();
}
//! [8]

bool TreeModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role != Qt::EditRole)
        return false;
    if (m_modelIsLoading) {
        // We are loading data from the processInterface into the model
        TreeItem* item = getItem(index);
        bool result = item->setData(index.column(), value);

        if (result)
            emit dataChanged(index, index);

        return result;
    } else {
        // User has modified a value through GUI, transmit change to processInterface (which will
        // reload the model). Bad design to reload the entire model for each change, but it works
        // and is fast to implement
        // Get parent process index and parameter index
        int processIndex = getRootIndex(index);
        int parameterIndex = index.row();

        m_interface->doAction(ProcessInterface::Action::SetValue, processIndex, parameterIndex,
                              value.toString().toStdString());
        return true;
    }
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
                              int role) {
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}
