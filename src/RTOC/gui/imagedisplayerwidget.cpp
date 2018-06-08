#include "imagedisplayerwidget.h"
#include "ui_imagedisplayerwidget.h"

#include "guihelpers.h"

#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>

ImageDisplayerWidget::ImageDisplayerWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::ImageDisplayerWidget) {
    ui->setupUi(this);

    ui->image->setAlignment(Qt::AlignCenter);

    // set icons
    ui->play->setIcon(QIcon(":/icons/resources/play-button.svg"));
    ui->loop->setIcon(QIcon(":/icons/resources/loop-button.svg"));

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
    ui->loop->setEnabled(false);

    // By default, do not show "view unprocessed image" on a standard imagedisplayerwidget
    ui->showUnprocessed->hide();
    ui->unprocessed->hide();

    connect(ui->showUnprocessed, &QCheckBox::toggled, this, &ImageDisplayerWidget::refreshImage);
    connect(ui->showUnprocessed, &QCheckBox::toggled,
            [=](bool checked) { ui->unprocessed->setVisible(checked); });

    // setup directory indexing future - this is used to be able to display a progress widget while
    // indexing into a directory, indexing into directories with a large number of images (30k+) on
    // slow hard drives can take multiple seconds.
    connect(&m_directoryIndexingWatcher, &QFutureWatcher<void>::finished, this,
            &ImageDisplayerWidget::directoryIndexingFinished);

    // Set default image text
    ui->image->setText("Please set an image folder path");

    // Initialize
    reset();
}

ImageDisplayerWidget::~ImageDisplayerWidget() {
    delete ui;
}

void ImageDisplayerWidget::setPath(const QString& path) {
    m_dir.setPath(path);

    // Create busy-progress widget
    m_directoryIndexingProgress = new QProgressDialog("Indexing files...", QString(), 0, 0, this);
    m_directoryIndexingProgress->setWindowModality(Qt::WindowModal);
    m_directoryIndexingProgress->setAutoReset(false);
    m_directoryIndexingProgress->setMinimumDuration(0);

    // Display progress dialog
    m_directoryIndexingProgress->setValue(0);

    // Asynchronously call indexDirectory, so as to not block the gui event loop while indexing
    // (failing to do so will make the progress dialog not update)
    QFuture<void> future = QtConcurrent::run(this, &ImageDisplayerWidget::indexDirectory);
    m_directoryIndexingWatcher.setFuture(future);
}

void ImageDisplayerWidget::directoryIndexingFinished() {
    // Async call to ImageDisplayerWidget::indexDirectory has finished,

    m_directoryIndexingProgress->reset();
    delete m_directoryIndexingProgress;

    // Set slider range
    ui->imageSlider->setRange(1, m_nImages);

    ui->ips->setRange(1, m_nImages);

    // Load first image
    ui->imageSlider->setValue(1);
    on_imageSlider_sliderMoved(ui->imageSlider->value());

    // enable slider & play button
    ui->imageSlider->setEnabled(true);
    ui->play->setEnabled(true);
    ui->loop->setEnabled(true);
}

void ImageDisplayerWidget::refreshImage() {
    // reload/process the current image
    displayImage(ui->imageSlider->value());
}

void ImageDisplayerWidget::displayImage(int index) {
    ui->image->setText("");
    // display image at index from the current selected directory
    if (!m_imageFileList.isEmpty() && m_imageFileList.size() > index) {
        if (m_analyzer != nullptr) {
            // An analyzer has been set, load image as a cv::mat and process it through analyzer
            m_image = cv::imread(m_imageFileList[index].absoluteFilePath().toStdString(),
                                 cv::IMREAD_GRAYSCALE);
            // Render unprocessed image
            if (ui->showUnprocessed->isChecked()) {
                ui->unprocessed->setPixmap(QPixmap::fromImage(
                    QImage(m_image.data, m_image.cols, m_image.rows, QImage::Format_Grayscale8)));
            }
            // Process image and set preview image
            m_analyzer->processSingleFrame(m_image);
            ui->image->setPixmap(QPixmap::fromImage(
                QImage(m_image.data, m_image.cols, m_image.rows, QImage::Format_Grayscale8)));
        } else {
            // Regular mode, just display the image onto the label
            ui->image->setPixmap(QPixmap(m_imageFileList[index].absoluteFilePath()));
        }
    }
}

void ImageDisplayerWidget::setAnalyzer(Analyzer* analyzer) {
    m_analyzer = analyzer;
    // Show the "show unprocessed image" checkbox
    ui->showUnprocessed->show();

    // Display images from the test image directory
    setPath("./test_images");
}

cv::Mat* ImageDisplayerWidget::getNextImage(bool& successful) {
    successful = true;

    if (m_acqIndex >= m_imageFileList.size()) {
        if (ui->loop->isChecked()) {
            m_acqIndex = 0;
        } else {
            successful = false;
            return &m_image;
        }
    }

    m_image = cv::imread(m_imageFileList[m_acqIndex++].absoluteFilePath().toStdString(),
                         cv::IMREAD_GRAYSCALE);

    if (!m_image.data) {
        successful = false;
        m_acqIndex = 0;
    }

    return &m_image;
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
    framefinder::sort_qfilelist(m_imageFileList);

    m_nImages = m_imageFileList.size();
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
        if (ui->loop->isChecked()) {
            ui->imageSlider->setValue(0);
            on_imageSlider_sliderMoved(ui->imageSlider->value());
        } else {
            ui->play->setChecked(false);
            on_play_clicked();
            m_playTimer.stop();
        }
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

    ui->ips->clearFocus();  // remove focus from spinbox after edit
}

void ImageDisplayerWidget::on_setPath_clicked() {
    auto dirName = QFileDialog::getExistingDirectory(this, "Select image directory", "/",
                                                     QFileDialog::ShowDirsOnly);
    if (!dirName.isNull()) {
        ui->path->setText(dirName);
        setPath(dirName);
    }
}

template <class Archive>
void ImageDisplayerWidget::save(Archive& ar, const unsigned int version) const {
    SERIALIZE_LINEEDIT(ar, ui->path, path);
    SERIALIZE_SPINBOX(ar, ui->ips, ips);
    SERIALIZE_CHECKBOX(ar, ui->showUnprocessed, showUnprocessed);
    SERIALIZE_SPINBOX(ar, ui->imageSlider, imageSlider);
}

template <class Archive>
void ImageDisplayerWidget::load(Archive& ar, const unsigned int version) {
    SERIALIZE_LINEEDIT(ar, ui->path, path);
    setPath(ui->path->text());
    SERIALIZE_SPINBOX(ar, ui->ips, ips);
    SERIALIZE_CHECKBOX(ar, ui->showUnprocessed, showUnprocessed);
    SERIALIZE_SPINBOX(ar, ui->imageSlider, imageSlider);
}

// Explicit instantiation of template functions
template void
ImageDisplayerWidget::save<boost::archive::xml_iarchive>(boost::archive::xml_iarchive&,
                                                         const unsigned int) const;
template void
ImageDisplayerWidget::save<boost::archive::xml_oarchive>(boost::archive::xml_oarchive&,
                                                         const unsigned int) const;
template void
ImageDisplayerWidget::load<boost::archive::xml_iarchive>(boost::archive::xml_iarchive&,
                                                         const unsigned int);
template void
ImageDisplayerWidget::load<boost::archive::xml_oarchive>(boost::archive::xml_oarchive&,
                                                         const unsigned int);
