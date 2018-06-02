#ifndef CAMERADISPLAYERWIDGET_H
#define CAMERADISPLAYERWIDGET_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTimer>

class CameraDisplayerWidget : public QGraphicsScene {
    Q_OBJECT
public:
    CameraDisplayerWidget();

    void startRequestingImages();
    void stopRequestingImages();

public slots:
    void setImage(const std::vector<char>&);
    void setImageDimensions(QPair<int, int>);
    void setImageInterval(int freq);
private slots:
    void tryRequestImage();

signals:
    void requestImage();

private:
    QPair<int, int> m_dim{0, 0};
    QTimer m_imageRequester;
    QGraphicsPixmapItem m_image;
    bool m_settingImage = false;
};

#endif  // CAMERADISPLAYERWIDGET_H
