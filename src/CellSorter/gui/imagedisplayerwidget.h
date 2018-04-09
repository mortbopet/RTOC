#ifndef IMAGEDISPLAYERWIDGET_H
#define IMAGEDISPLAYERWIDGET_H

#include <QDir>
#include <QStringList>
#include <QTimer>
#include <QWidget>

namespace Ui {
class ImageDisplayerWidget;
}

class ImageDisplayerWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImageDisplayerWidget(QWidget* parent = 0);
    ~ImageDisplayerWidget();

public slots:
    void setPath(const QString& path);

private slots:
    void on_play_clicked();

    void on_imageSlider_sliderMoved(int position);

    void displayImage(int index);
    void playTimerTimeout();
    void on_ips_editingFinished();

private:
    void indexDirectory();

    Ui::ImageDisplayerWidget* ui;

    int m_nImages;
    QDir m_dir;
    QFileInfoList m_imageFileList;

    QTimer m_playTimer;
};

#endif  // IMAGEDISPLAYERWIDGET_H
