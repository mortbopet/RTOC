#ifndef EXPERIMENTPROGRESSWIDGET_H
#define EXPERIMENTPROGRESSWIDGET_H

#include <QDialog>
#include <QTime>
#include <QWidget>

namespace Ui {
class ExperimentProgressWidget;
}

class ExperimentProgressWidget : public QDialog {
    Q_OBJECT

public:
    explicit ExperimentProgressWidget(QWidget* parent = 0);
    ~ExperimentProgressWidget();
    Ui::ExperimentProgressWidget* ui;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

    void timerElapsed();

private:
    QTimer* m_timer;
    QTime m_time;
};

#endif  // EXPERIMENTPROGRESSWIDGET_H
