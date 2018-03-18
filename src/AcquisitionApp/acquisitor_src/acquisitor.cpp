#include "acquisitor.h"

#include "../acquisitor_src/acquisitionhelpers.cpp"

#include <QtGlobal>

#include <sstream>

Acquisitor::Acquisitor(QObject* parent) : QObject(parent) {}

int Acquisitor::initialize() {
    if (!m_isInitialized) {
        m_logger->writeLineToLog("Initializing framegrabber...");
        try {
            // initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if (status != FG_OK) {
                m_logger->writeLineToLog("Cannot initialize Fg libraries.");
                return -1;
            }
            /*Initialize framegrabber struct with default applet. Assume board is at index 0 (single
             * board in computer*/
            m_FgHandle = FgWrapper::create("Acq_SingleCXP6x4AreaGray8.dll", 0);
            if (m_FgHandle->getFgHandle() == NULL) {
                m_logger->writeLineToLog(
                    QString("Error in Fg_Init(): %s\n").arg(Fg_getLastErrorDescription(NULL)));
                return -1;
            }

            /*FgDmaChannelExample shows dma initialization.*/
            m_dmaHandle = DmaMemWrapper::create(m_FgHandle, m_dmaPort);
            m_display = DisplayWrapper::create(std::weak_ptr<DmaMemWrapper>(m_dmaHandle), m_dmaPort,
                                               std::weak_ptr<FgWrapper>(m_FgHandle));
            m_board = initialiseBoard(m_FgHandle->getFgHandle());
            // std::vector<std::pair<std::string, SgcCameraHandle*>> cameras =
            // discoverCameras(board);

            m_camera = selectCamera(m_board);

        } catch (std::exception& e) {
            // releases internal structures of the library
            Fg_FreeLibraries();
            m_logger->writeLineToLog("Initialization failed: " + QString(e.what()));
            emit initialized(false);
            return -1;
        }

        m_isInitialized = true;
        emit initialized(m_isInitialized);
    }
    return 0;
}

int Acquisitor::deInitialize() {
    Q_ASSERT(m_isInitialized);
    try {
        Sgc_freeBoard(m_board);

        // releases internal structures of the library
        Fg_FreeLibraries();
    } catch (std::exception& e) {
        // releases internal structures of the library
        Fg_FreeLibraries();
        std::cout << "Example failed: " << e.what() << std::endl;
        emit initialized(false);
        return -1;
    }
    m_isInitialized = false;
    emit initialized(m_isInitialized);
    return 0;
}

void Acquisitor::acquisitionSgc() {
    // Acquisition function - spawned as a separate thread
    uint32_t MaxPics = GRAB_INFINITE;

    frameindex_t bufNr = 0;
    bool isError = false;
    const uint32_t waitDurationInMs = 10;

    Fg_Struct* fgHandle = m_FgHandle->getFgHandle();
    dma_mem* dmaHandle = m_dmaHandle->getMemHandle();

    if (Fg_AcquireEx(fgHandle, m_dmaPort, MaxPics, ACQ_BLOCK, dmaHandle) != FG_OK)
        throwLastFgError();

    /*Start the camera acquisition*/
    Sgc_startAcquisition(m_camera, 0);

    while (!isError && m_acquire) {
        bufNr = Fg_getImageEx(fgHandle, SEL_ACT_IMAGE, 0, m_dmaPort, waitDurationInMs, dmaHandle);

        if (bufNr < 0) {
            if (bufNr == FG_TIMEOUT_ERR) {
                // No more frame
                break;
            } else {
                throwLastFgError();
            }
        }

        drawBuffer((int)bufNr);

        // Access a pointer to an image in the buffer denoted by bufNr
        void* pos = Fg_getImagePtrEx(fgHandle, bufNr, m_dmaPort, dmaHandle);

        // Unblock the buffer
        Fg_setStatusEx(fgHandle, FG_UNBLOCK, bufNr, m_dmaPort, dmaHandle);
    }

    /*Stop framegrabber */
    if (Fg_stopAcquireEx(fgHandle, m_dmaPort, dmaHandle, 0) != FG_OK)
        throwLastFgError();

    /*Stop the camera acquisition*/
    Sgc_stopAcquisition(m_camera, 0);
}

void Acquisitor::throwLastFgError() {
    throwLastFgError(m_FgHandle->getFgHandle());
}

void Acquisitor::throwLastFgError(Fg_Struct* fgHandle) {
    std::stringstream ss;
    ss << "Error (" << Fg_getLastErrorNumber(fgHandle) << ") "
       << Fg_getLastErrorDescription(fgHandle) << std::endl;
    throw std::runtime_error(ss.str());
}

void Acquisitor::startAcq() {
    // start thread
    m_acquire = true;
    m_acqThread = std::thread{&Acquisitor::acquisitionSgc, this};
    emit acquisitionStateChanged(m_acquire);
}

void Acquisitor::stopAcq() {
    m_acquire = false;
    m_acqThread.join();
    emit acquisitionStateChanged(m_acquire);
}
