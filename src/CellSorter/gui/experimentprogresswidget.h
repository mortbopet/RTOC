#ifndef EXPERIMENTPROGRESSWIDGET_H
#define EXPERIMENTPROGRESSWIDGET_H

#include <QAbstractButton>
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

public slots:
    void reject() override;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

    void timerElapsed();

    void on_buttonBox_clicked(QAbstractButton* button);

private:
    QTimer* m_timer;
    QTime m_time;
};

#endif  // EXPERIMENTPROGRESSWIDGET_H
