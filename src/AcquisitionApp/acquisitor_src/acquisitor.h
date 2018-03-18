#pragma once

/*This example demonstrates the acquisition with siso_genicam.*/
#include <stdio.h>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <QObject>

#include <SisoDisplay.h>
#include <fgrab_define.h>
#include <fgrab_prototyp.h>
#include <fgrab_struct.h>
#include <siso_genicam.h>
#include <stdexcept>

#include <thread>

#include <QThread>

#include "../acquisitor_src/DisplayWrapper.h"
#include "../acquisitor_src/DmaMemWrapper.h"

#include "../src/logger.h"

/**
 * @brief The Acquisitor class
 * @abstract The Acquisitor class is used for facilitating all contact with the framegrabber. This
 * includes initialization, parametrization and acquisition.
 * The Acquisitor is intended to live in a separate thread, and communicates to the GUI thread via.
 * queued signals and slots ( http://doc.qt.io/archives/qt-4.8/thread-basics.html ). As per Qt, it
 * is stated that
 *  "" QMetaObject::invokeMethod() calls a slot via the event loop. The worker object's methods
 * should not be called directly after the object has been moved to another thread. ""
 */
class Acquisitor : public QObject {
    Q_OBJECT
public:
    static Acquisitor* get();
    ~Acquisitor();

public slots:
    int initialize();
    int deInitialize();

    void startAcq();
    void stopAcq();

signals:
    void initialized(bool);
    void acquisitionStateChanged(bool);  // true = running, false = stopped
    void writeToLog(QString msg);

protected:
    void acquisitionSgc();

private:
    Acquisitor(QObject* parent = nullptr);

    void drawBuffer(int32_t bufferIndex) { m_display->drawBuffer(bufferIndex); }

    void throwLastFgError(Fg_Struct* fgHandle);
    void throwLastFgError();

    std::shared_ptr<FgWrapper> m_FgHandle;
    std::shared_ptr<DmaMemWrapper> m_dmaHandle;
    std::shared_ptr<DisplayWrapper> m_display;
    SgcCameraHandle* m_camera;
    SgcBoardHandle* m_board;

    int m_dmaPort = 0;

    bool m_isInitialized = false;
    bool m_acquire = false;

    std::thread m_acqThread;

    QThread* m_thread;
};
