#ifndef EXPERIMENTRUNNER_H
#define EXPERIMENTRUNNER_H

#include <QAbstractButton>
#include <QDialog>
#include <QFutureWatcher>
#include <QTime>
#include <QWidget>

#include "../lib/analyzer.h"

namespace Ui {
class ExperimentRunner;
}

class ExperimentRunner : public QDialog {
    Q_OBJECT

public:
    explicit ExperimentRunner(Analyzer* analyzer, Setup setup, QWidget* parent = 0);
    ~ExperimentRunner();
    Ui::ExperimentRunner* ui;

public slots:
    void reject() override;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void acquisitionFinished();
    void dataExtractionFinished();

    void acqTimerElapsed();
    void dataTimerElapsed();

    void on_buttonBox_clicked(QAbstractButton* button);

private:
    Setup m_setup;

    Analyzer* m_analyzer;
    QTimer* m_acqTimer;
    QTimer* m_dataTimer;
    QTime m_time;
    QFutureWatcher<void> m_acqFutureWatcher;
    QFutureWatcher<void> m_dataFutureWatcher;
};

#endif  // EXPERIMENTRUNNER_H
