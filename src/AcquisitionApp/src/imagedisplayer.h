#ifndef IMAGEDISPLAYER_H
#define IMAGEDISPLAYER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTimer>

class ImageDisplayer : public QGraphicsScene {
    Q_OBJECT
public:
    ImageDisplayer();

    void startRequestingImages();
    void stopRequestingImages();

public slots:
    void setImage(const std::vector<char>&);

signals:
    void requestImage();

private:
    QTimer m_imageRequester;
    QGraphicsPixmapItem m_image;
};

#endif  // IMAGEDISPLAYER_H
