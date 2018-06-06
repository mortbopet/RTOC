#ifndef PARAMETERDELEGATE_H
#define PARAMETERDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

#include "../lib/parameter.h"
#include "treeitem.h"

class ParameterDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ParameterDelegate(QAbstractItemModel* model, QObject* parent = 0);
    ~ParameterDelegate();

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model,
                              const QModelIndex& index) const;

private:
    QAbstractItemModel* m_model;
};

#endif  // PARAMETERDELEGATE_H
