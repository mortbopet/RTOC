#ifndef CELLSORTER_FOLDERACQUISITION_H
#define CELLSORTER_FOLDERACQUISITION_H

#include <QDir>
#include <QStringList>
#include <QTimer>

#include "opencv/cv.hpp"

#include "../lib/framefinder.h"


class FolderAcquisition {
public:
    cv::Mat* getNextImage(bool& successful);
    void reset();

    void setPath(const QString& path);

private:
    void indexDirectory();

    cv::Mat m_image;

    int m_acqIndex;
    int m_nImages;
    QDir m_dir;
    QFileInfoList m_imageFileList;

};


#endif //CELLSORTER_FOLDERACQUISITION_H
