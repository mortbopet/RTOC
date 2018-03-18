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

#include "../acquisitor_src/DisplayWrapper.h"
#include "../acquisitor_src/DmaMemWrapper.h"

#include "../src/logger.h"

class Acquisitor : public QObject {
    Q_OBJECT
public:
    static Acquisitor* get() {
        // lazy initialized of singleton
        static Acquisitor acq;
        return &acq;
    }

    void setLog(Logger* log) { m_logger = log; }

public slots:
    int initialize();
    int deInitialize();

    void startAcq();
    void stopAcq();

signals:
    void initialized(bool);
    void acquisitionStateChanged(bool);  // true = running, false = stopped

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

    Logger* m_logger;
};
