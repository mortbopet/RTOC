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

#include <fgrab_define.h>
#include <fgrab_prototyp.h>
#include <fgrab_struct.h>
#include <siso_genicam.h>
#include <stdexcept>

#include <thread>

#include <QMutex>
#include <QThread>

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

typedef struct {
    uint32_t width;
    uint32_t height;
    int32_t format;
} FgValues;

enum class AcqState { Idle, Initializing, Initialized, Acquiring };

Q_DECLARE_METATYPE(AcqState)

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

    // Called when GUI wants an image, initializes copying of an image in the acquisition loop
    void requestImageData() { m_requestImage = true; }

signals:
    void writeToLog(QString msg);

    void stateChanged(AcqState);

    // Emitted when an image has been stored from acquisition and can be accessed from gui
    void sendImageData(const std::vector<char>&);

protected:
    void acquisitionSgc();

private:
    Acquisitor(QObject* parent = nullptr);

    void throwLastFgError(Fg_Struct* fgHandle);
    void throwLastFgError();
    void setState(AcqState state);

    std::shared_ptr<FgWrapper> m_FgHandle;
    std::shared_ptr<DmaMemWrapper> m_dmaHandle;
    SgcCameraHandle* m_camera;
    SgcBoardHandle* m_board;

    AcqState m_acqState = AcqState::Idle;

    /* We have no mutexes on m_image, since we assume that the protocol:
     *  1: GUI requests image;
     *  2: Acquisitor stores copies image data into m_image std::vector<char> (contiguous memory)
     *  3: Acquisitor emits reference to m_image
     *  4: GUI accesses m_image memory
     * is kept, thus no simultanious accesses to the shared data.s
     */
    std::vector<char> m_image;
    bool m_requestImage = false;

    int m_dmaPort = 0;

    std::thread m_acqThread;

    QThread* m_thread;

    FgValues m_fgValues;
};
