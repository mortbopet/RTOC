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

    void on_run_clicked();

    void on_setExperimentPath_clicked();
    bool verifyCanRunExperiment();

private:
    void setToolTips();
    void setupDataOptions();
    Ui::ExperimentSetup* ui;

    Analyzer* m_analyzer;

    QString m_experimentPath;

    // Functions related to running the experiment
    bool setupExperimentDirectory() const;
};

#endif  // EXPERIMENTSETUP_H
