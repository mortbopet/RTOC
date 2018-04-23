#ifndef EXPERIMENTPROGRESSWIDGET_H
#define EXPERIMENTPROGRESSWIDGET_H

#include <QAbstractButton>
#include <QDialog>
#include <QFutureWatcher>
#include <QTime>
#include <QWidget>

#include "../lib/analyzer.h"

namespace Ui {
class ExperimentProgressWidget;
}

class ExperimentProgressWidget : public QDialog {
    Q_OBJECT

public:
    explicit ExperimentProgressWidget(Analyzer* analyzer, Setup setup, QWidget* parent = 0);
    ~ExperimentProgressWidget();
    Ui::ExperimentProgressWidget* ui;

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
    Analyzer* m_analyzer;
    QTimer* m_acqTimer;
    QTimer* m_dataTimer;
    QTime m_time;
    QFutureWatcher<void> m_acqFutureWatcher;
    QFutureWatcher<void> m_dataFutureWatcher;
};

#endif  // EXPERIMENTPROGRESSWIDGET_H
