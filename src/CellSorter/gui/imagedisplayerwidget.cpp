#include "imagedisplayerwidget.h"
#include "ui_imagedisplayerwidget.h"

#include <QFileDialog>


ImageDisplayerWidget::ImageDisplayerWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::ImageDisplayerWidget) {
    ui->setupUi(this);

    ui->image->setAlignment(Qt::AlignCenter);

    // set icons
    ui->play->setIcon(QIcon(":/icons/resources/play-button.svg"));

    // set image counter default text
    ui->imageCounter->setText("Image: #/#");

    int defaultInterval = 60;
    ui->ips->setRange(1, 100);
    ui->ips->setValue(defaultInterval);

    // connect play timer
    m_playTimer.setInterval(defaultInterval);
    connect(&m_playTimer, &QTimer::timeout, this, &ImageDisplayerWidget::playTimerTimeout);

    // Disable slider & play button by default
    ui->imageSlider->setEnabled(false);
    ui->play->setEnabled(false);
}

ImageDisplayerWidget::~ImageDisplayerWidget() {
    delete ui;
}

void ImageDisplayerWidget::setPath(const QString& path) {
    m_dir.setPath(path);
    indexDirectory();
}

void ImageDisplayerWidget::displayImage(int index) {
    // display image at index from the current selected directory
    if (!m_imageFileList.isEmpty() && m_imageFileList.size() > index) {
        ui->image->setPixmap(QPixmap(m_imageFileList[index].absoluteFilePath()));
    }
}

cv::Mat& ImageDisplayerWidget::getNextImage(bool& successful) {
    successful = true;

    if (m_acqIndex < m_imageFileList.size()) {
        m_image = cv::imread(m_imageFileList[m_acqIndex++].absoluteFilePath().toStdString(),
                             cv::IMREAD_GRAYSCALE);
    } else {
        successful = false;
        return m_image;
    }

    if (!m_image.data) {
        successful = false;
        m_acqIndex = 0;
    }

    return m_image;
}

void ImageDisplayerWidget::reset() {
    m_acqIndex = 0;
}

void ImageDisplayerWidget::indexDirectory() {
    m_imageFileList.clear();

    // Set image filters
    QStringList filters;
    filters << "*.png"
            << "*.jpg"
            << "*.bmp";
    m_imageFileList = m_dir.entryInfoList(filters, QDir::Files);
    sort_qfilelist(m_imageFileList,"_",".");

    m_nImages = m_imageFileList.size();

    // Set slider range
    ui->imageSlider->setRange(1, m_nImages);

    ui->ips->setRange(1, m_nImages);

    // Load first image
    ui->imageSlider->setValue(1);
    on_imageSlider_sliderMoved(ui->imageSlider->value());

    // enable slider & play button
    ui->imageSlider->setEnabled(true);
    ui->play->setEnabled(true);
}

void ImageDisplayerWidget::on_play_clicked() {
    if (ui->play->isChecked()) {
        ui->play->setIcon(QIcon(":/icons/resources/pause-button.svg"));
        m_playTimer.start();
    } else {
        ui->play->setIcon(QIcon(":/icons/resources/play-button.svg"));
        m_playTimer.stop();
    }
}

void ImageDisplayerWidget::playTimerTimeout() {
    // stop timer if last indexed image is reached
    if (ui->imageSlider->value() == m_nImages) {
        ui->play->setChecked(false);
        on_play_clicked();
        m_playTimer.stop();
    } else {
        ui->imageSlider->setValue(ui->imageSlider->value() + 1);
        on_imageSlider_sliderMoved(ui->imageSlider->value());
    }
}

void ImageDisplayerWidget::on_imageSlider_sliderMoved(int position) {
    displayImage(position - 1);
    ui->imageCounter->setText(QString("Image: %1/%2").arg(position).arg(m_nImages));
}

void ImageDisplayerWidget::on_ips_editingFinished() {
    // convert fps to ms and set interval
    m_playTimer.stop();
    int interval = 1000.0 / ui->ips->value();
    m_playTimer.setInterval(interval);
    m_playTimer.start();
}

void ImageDisplayerWidget::on_setPath_clicked() {
    auto dirName = QFileDialog::getExistingDirectory(this, "Select image directory", "/",
                                                     QFileDialog::ShowDirsOnly);
    if (!dirName.isNull()) {
        ui->path->setText(dirName);
        setPath(dirName);
    }
}
