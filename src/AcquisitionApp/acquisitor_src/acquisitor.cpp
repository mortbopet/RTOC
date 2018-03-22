#include "acquisitor.h"

#include "../acquisitor_src/acquisitionhelpers.cpp"

#include <QtGlobal>

#include <sstream>

Acquisitor::Acquisitor(QObject* parent) : QObject(parent) {}
Acquisitor::~Acquisitor() {
    // Stop and delete acquisitor thread
    m_thread->quit();
    m_thread->wait();
    delete m_thread;
}

Acquisitor* Acquisitor::get() {
    // lazy initialized of singleton acquisiton. Upon initialization, the acquisitor is moved to a
    // separate thread.
    static Acquisitor* acq;
    if (acq == nullptr) {
        acq = new Acquisitor();
        acq->m_thread = new QThread();
        acq->moveToThread(acq->m_thread);
        acq->m_thread->start();
    }
    return acq;
}

void Acquisitor::setState(AcqState state) {
    // sets acquisitor state and notifies the change to GUI
    m_acqState = state;
    emit stateChanged(m_acqState);
}

int Acquisitor::initialize() {
    if (m_acqState == AcqState::Idle) {
        emit writeToLog("Initializing framegrabber");
        setState(AcqState::Initializing);
        try {
            // initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if (status != FG_OK) {
                emit writeToLog("Cannot initialize Fg libraries.");
                setState(AcqState::Idle);
                return -1;
            }
            /*Initialize framegrabber struct with default applet. Assume board is at index 0 (single
             * board in computer*/
            m_FgHandle = FgWrapper::create("Acq_SingleCXP6x4AreaGray8.dll", 0);
            if (m_FgHandle->getFgHandle() == NULL) {
                emit writeToLog(
                    QString("Error in Fg_Init(): %s\n").arg(Fg_getLastErrorDescription(NULL)));
                setState(AcqState::Idle);
                return -1;
            }

            /*FgDmaChannelExample shows dma initialization.*/
            m_dmaHandle = DmaMemWrapper::create(m_FgHandle, m_dmaPort);

            // Resize m_image according to the framegrabber image size
            m_image.resize(m_FgHandle->getPayloadSize(m_dmaPort));

            m_board = initialiseBoard(m_FgHandle->getFgHandle());
            // std::vector<std::pair<std::string, SgcCameraHandle*>> cameras =
            // discoverCameras(board);

            m_camera = selectCamera(m_board);

        } catch (std::exception& e) {
            // releases internal structures of the library
            Fg_FreeLibraries();
            emit writeToLog("Initialization failed: " + QString(e.what()));
            setState(AcqState::Idle);
            return -1;
        }
        emit writeToLog("Initialization successfull. Detected camera:");
        emit writeToLog(getCameraInfo(m_camera));
        setState(AcqState::Initialized);
    }
    return 0;
}

int Acquisitor::deInitialize() {
    setState(AcqState::Idle);
    try {
        Sgc_freeBoard(m_board);

        // releases internal structures of the library
        Fg_FreeLibraries();
    } catch (std::exception& e) {
        // releases internal structures of the library
        Fg_FreeLibraries();
        std::cout << "Example failed: " << e.what() << std::endl;
        return -1;
    }
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

    while (!isError && m_acqState == AcqState::Acquiring) {
        bufNr = Fg_getImageEx(fgHandle, SEL_ACT_IMAGE, 0, m_dmaPort, waitDurationInMs, dmaHandle);

        if (bufNr < 0) {
            if (bufNr == FG_TIMEOUT_ERR) {
                // No more frame
                break;
            } else {
                throwLastFgError();
            }
        }

        // Access a pointer to an image in the buffer denoted by bufNr
        void* pos = Fg_getImagePtrEx(fgHandle, bufNr, m_dmaPort, dmaHandle);

        if (m_requestImage) {
            // Copy image buffer to m_image
            std::memcpy(m_image.data(), pos, m_image.size());
            m_requestImage = false;
            emit sendImageData(m_image);
        }
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
    if (m_acqState == AcqState::Initialized) {
        emit writeToLog("Starting acquisition...");
        setState(AcqState::Acquiring);
        m_acqThread = std::thread{&Acquisitor::acquisitionSgc, this};
    }
}

void Acquisitor::stopAcq() {
    if (m_acqState == AcqState::Acquiring) {
        emit writeToLog("Stopping acquisition");
        setState(AcqState::Initialized);
        m_acqThread.join();
    }
}
