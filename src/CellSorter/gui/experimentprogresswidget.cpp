#include "experimentprogresswidget.h"
#include "ui_experimentprogresswidget.h"

#include <QPushButton>
#include <QTimer>

ExperimentProgressWidget::ExperimentProgressWidget(QWidget* parent)
    : QDialog(parent), ui(new Ui::ExperimentProgressWidget) {
    ui->setupUi(this);

    // change "Abort" button to stop acquisition
    ui->buttonBox->button(QDialogButtonBox::Abort)->setText("Finish acquisition");

    ui->acqCount->setText(0);
    ui->dataextraction->setEnabled(false);

    // Setup time elapsed timer
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &ExperimentProgressWidget::timerElapsed);
    // start time
    m_time.start();
    m_timer->start();
}

ExperimentProgressWidget::~ExperimentProgressWidget() {
    delete ui;
}

void ExperimentProgressWidget::on_pushButton_2_clicked() {}

void ExperimentProgressWidget::on_pushButton_clicked() {}

void ExperimentProgressWidget::timerElapsed() {
    ui->elapsed->setText(QString::number(m_time.elapsed() / 1000));
}
