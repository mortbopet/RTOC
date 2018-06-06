#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QWidget>
#include <string>
#include "imagedisplayerwidget.h"
#include "parameterdelegate.h"
#include "processinterface.h"
#include "treemodel.h"

#include <QMetaType>

namespace Ui {
class Configurator;
}

class Configurator : public QWidget {
    Q_OBJECT

public:
    explicit Configurator(ProcessInterface* iface, Analyzer* analyzer, QWidget* parent = 0);
    ~Configurator();

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) const;

private slots:
    void on_add_clicked();
    void on_remove_clicked();
    void updateModel();
    void on_up_clicked();
    void on_down_clicked();
    void on_load_clicked();
    void on_store_clicked();

private:
    QString m_current_dir = QDir::currentPath() + "/presets";

    TreeModel* m_model;
    Ui::Configurator* ui;
    ProcessInterface* m_interface;
    ParameterDelegate* m_delegate;

    ImageDisplayerWidget* m_imagedisplayer;

    QModelIndex insertChild(const QModelIndex& index, QList<QVariant> values);
    void insertRow(const QModelIndex& index, QList<QVariant> values);
    void removeRow(const QModelIndex& index);
    void reorder(ProcessInterface::Action dir);
};

#endif  // CONFIGURATOR_H
