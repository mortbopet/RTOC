#include "acquisitor.h"

#include "../acquisitor_src/acquisitionhelpers.cpp"

#include <QtConcurrent/qtconcurrentrun.h>
#include <QtGlobal>

#include <sstream>
Acquisitor::Acquisitor(QObject* parent) : QObject(parent) {}
Acquisitor::~Acquisitor() {
    if (m_thread) {
        // Stop and delete acquisitor thread
        m_thread->quit();
        m_thread->wait();
        delete m_thread;
    }
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

void Acquisitor::getCameraDimensions(SgcCameraHandle* camera, int64_t& width, int64_t& height) {
    int err;
    err = Sgc_getIntegerValue(camera, "Width", &width);
    if (err != 0) {
        throw std::runtime_error(Sgc_getErrorDescription(err));
    }
    err = Sgc_getIntegerValue(camera, "Height", &height);
    if (err != 0) {
        throw std::runtime_error(Sgc_getErrorDescription(err));
    }
}

void Acquisitor::configureFgDimensions(SgcCameraHandle* camera, Fg_Struct* handle) {
    // sgc requires int64_t, Fg requires int, *sigh*
    int64_t _width, _height;
    getCameraDimensions(camera, _width, _height);
    int width = _width;
    int height = _height;
    Fg_setParameter(handle, FG_WIDTH, &width, 0);
    Fg_setParameter(handle, FG_HEIGHT, &height, 0);
    emit imageDimensionsChanged(QPair<int, int>{width, height});
}

int Acquisitor::initialize(const QString& xmlPath, const QString& configPath, bool useConfigFile) {
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
            emit writeToLog("Framegrabber libraries loaded successfully");

            /*Initialize framegrabber struct with default applet. Assume board is at index 0 (single
             * board in computer*/
            m_FgHandle = FgWrapper::create("Acq_SingleCXP6x4AreaGray8.dll", 0);
            if (m_FgHandle->getFgHandle() == NULL) {
                emit writeToLog(
                    QString("Error in Fg_Init(): %s\n").arg(Fg_getLastErrorDescription(NULL)));
                setState(AcqState::Idle);
                return -1;
            }
            emit writeToLog("Framegrabber handle created successfully");

            m_board = initialiseBoard(m_FgHandle->getFgHandle());
            emit writeToLog("Board initialized successfully");
            m_camera = selectCamera(m_board);
            emit writeToLog("Camera detected successfully");

            // Use input configuration file to configure camera
            if (useConfigFile) {
                int err;

                err = Sgc_connectCameraWithExternalXml(m_camera, xmlPath.toStdString().c_str());
                if (err != 0) {
                    throw std::runtime_error(Sgc_getErrorDescription(err));
                }
                err = Sgc_LoadCameraParameters(m_camera, configPath.toStdString().c_str(), 0);
                if (err != 0) {
                    throw std::runtime_error(Sgc_getErrorDescription(err));
                }
                emit writeToLog("User specified configuration files sucessfully written to camera");
            }

            // Configure frame grabber to the same dimensions as the camera
            configureFgDimensions(m_camera, m_FgHandle->getFgHandle());
            emit writeToLog("Framegrabber successfully configured to camera dimensions");

            /*FgDmaChannelExample shows dma initialization.*/
            m_dmaHandle = DmaMemWrapper::create(m_FgHandle, m_dmaPort);
            emit writeToLog("DMA memory created successfully");

            // Resize m_image according to the framegrabber image size
            m_image.resize(m_FgHandle->getPayloadSize(m_dmaPort));

            // std::vector<std::pair<std::string, SgcCameraHandle*>> cameras =
            // discoverCameras(board);

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

        if (m_requestImage) {
            // Access a pointer to an image in the buffer denoted by bufNr
            void* pos = Fg_getImagePtrEx(fgHandle, bufNr, m_dmaPort, dmaHandle);

            // Copy image buffer to m_image
            std::memcpy(m_image.data(), pos, m_image.size());
            emit sendImageData(m_image);
            m_requestImage = false;
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

std::vector<char>* Acquisitor::requestImageDataBlocking(bool& successful) {
    // Request a pointer to the most recently acquisited image. This function does not use the Qt
    // event loop, and is blocking until the data has been copied
    m_requestImage = true;
    while (m_requestImage)
        ;
    successful = true;  // a bit hacky but we should always do something with the input successful
                        // variable, to adhere to the interface in AcquisitionInterface
    return &m_image;
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
        m_acqFuture = QtConcurrent::run(this, &Acquisitor::acquisitionSgc);
    }
}

void Acquisitor::stopAcq() {
    if (m_acqState == AcqState::Acquiring) {
        emit writeToLog("Stopping acquisition");
        setState(AcqState::Initialized);
        m_acqFuture.waitForFinished();
    }
}
