#ifndef EXPERIMENTSETUP_H
#define EXPERIMENTSETUP_H

#include <QWidget>

#include "../lib/analyzer.h"

namespace Ui {
class ExperimentSetup;
}

class ExperimentSetup : public QWidget {
    Q_OBJECT

public:
    explicit ExperimentSetup(Analyzer* analyzer, QWidget* parent = 0);
    ~ExperimentSetup();

private slots:
    void on_groupBox_toggled(bool arg1);

private:
    void setToolTips();
    void setupDataOptions();
    Ui::ExperimentSetup* ui;

    Analyzer* m_analyzer;
};

#endif  // EXPERIMENTSETUP_H
