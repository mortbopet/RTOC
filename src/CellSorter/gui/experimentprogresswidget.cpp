#include "experimentprogresswidget.h"
#include "ui_experimentprogresswidget.h"

#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

ExperimentProgressWidget::ExperimentProgressWidget(QWidget* parent)
    : QDialog(parent), ui(new Ui::ExperimentProgressWidget) {
    ui->setupUi(this);

    // change "Abort" button to stop acquisition
    ui->buttonBox->button(QDialogButtonBox::Abort)->setText("Finish acquisition");

    ui->acqCount->setText(0);
    ui->dataextraction->setEnabled(false);

    // Setup gui update timer - used for querying analyzer for current progress and whether it is
    // done with acquisition
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
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

void ExperimentProgressWidget::reject() {
    QString warning =
        "Are you sure you want to stop the experiment? Images will NOT be saved to the disk";
    if (QMessageBox::question(this, "Stop experiment", warning) == QMessageBox::Yes) {
        QDialog::reject();
    }
}

void ExperimentProgressWidget::on_buttonBox_clicked(QAbstractButton* button) {
    if (button == ui->buttonBox->button(QDialogButtonBox::Abort)) {
        // Stop acquisition and run data processing
    } else if (button == ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        this->reject();
    }
}
