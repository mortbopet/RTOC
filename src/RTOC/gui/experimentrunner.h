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

/**
 * @brief The ExperimentRunner class
 * Works by having various asynchronous calls to Analyzer functions. Each call has an accompanying
 * futureWatcher, which, when the future has returned, will trigger a state change in the
 * ExperimentRunner. It is in these state changes that new functions are called, and the GUI is
 * updated/modified.
 */
class ExperimentRunner : public QDialog {
    Q_OBJECT

public:
    explicit ExperimentRunner(Analyzer* analyzer, Setup setup, QWidget* parent = 0);
    ~ExperimentRunner();
    Ui::ExperimentRunner* ui;

public slots:
    void reject() override;

private slots:
    void guiUpdateTimerElapsed();
    void on_buttonBox_clicked(QAbstractButton* button);

private:
    enum class State { Acquiring, StoringImages, GettingData, StoringData, Finished };
    void stateChanged(State state);
    void checkAnalyzerStatusMessage(const int status) const;

    State m_state;
    Setup m_setup;

    Analyzer* m_analyzer;
    QTimer* m_timer;
    QTime m_time;

    // A future is set for each state change
    QFutureWatcher<void> m_acqFutureWatcher;
    QFutureWatcher<void> m_storeImageFutureWatcher;
    QFutureWatcher<void> m_storeDataFutureWatcher;
    QFutureWatcher<void> m_getDataFutureWatcher;
};

#endif  // EXPERIMENTRUNNER_H
