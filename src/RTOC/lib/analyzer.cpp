#include "analyzer.h"

#include <boost/filesystem.hpp>

#include <chrono>

void Analyzer::stopAnalyzer() {
    // Sets async stop flag. This flag is checked various places in the different analysis steps. If
    // flag is detected, CHECK_ASYNC_STOP is called, and given function will jump to its ASYNC_END
    // exit-strategy
    m_asyncStopAnalyzer = true;
}

#define CHECK_ASYNC_STOP       \
    if (m_asyncStopAnalyzer) { \
        goto async_stop;       \
    }

// End statement for a CHECK_ASYNC_STOP function, with side effects specified in the lambda function
#define ASYNC_END_SIDEEFFECT(function) \
    return;                            \
    async_stop:                        \
    asyncStop();                       \
    reset();                           \
    function();                        \
    return;

// End statement for a CHECK_ASYNC_STOP function, with no sideeffects
#define ASYNC_END \
    return;       \
    async_stop:   \
    asyncStop();  \
    reset();      \
    return;

/**
 * @brief
 *
 * @warning NOT IMPLEMENTED
 */
void Analyzer::loadImagesFromText() {
    std::string accepted;  // Path to accepted text-file
    std::string rejected;  // Path to rejected text-file
    // Compile img_folder path with filenames from text-files
    // and push frames to m_experiment
}

/**
 * @brief Sets analyzer background
 * @details
 *
 * @param bg : background image to be used
 */
void Analyzer::setBG(const cv::Mat& bg) {
    m_bg = bg;
}

/**
 * @brief Run all processes on
 * @details
 *
 */
void Analyzer::runProcesses() {
    processImage(m_img, m_bg);
}

/**
 * @brief Process a single frame
 * @details Assuming background is set in Analyzer
 *
 * @param img
 */
void Analyzer::processSingleFrame(cv::Mat& img) {
    if (m_bg.empty() || (m_bg.rows != img.rows || m_bg.cols != img.cols)) {
        m_bg = img.clone();
    }
    processImage(img, m_bg);
}

/**
 * @brief Overload: Process a single frame
 * @details
 *
 * @param img
 * @param bg
 */
void Analyzer::processSingleFrame(cv::Mat& img, cv::Mat& bg) {
    processImage(img, bg);
}

/**
 * @brief
 * @details
 *
 */

namespace {
namespace fs = boost::filesystem;
typedef std::chrono::high_resolution_clock Clock;
}  // namespace

using namespace std::chrono;

void Analyzer::spinLockWait(int micros) const {
    auto since_epoch = Clock::now().time_since_epoch();

    int cnt = 0;
    while ((duration_cast<microseconds>(Clock::now().time_since_epoch()).count() -
            duration_cast<microseconds>(since_epoch).count()) < micros) {
        cnt++;
    };
}

void Analyzer::runAnalyzer(Setup setup) {
    reset();

    // Set setup. This will be used other subsequent actions in an analyzer call
    m_setup = setup;

    // Start image writers if we are doing asynchronous image writing
    if (m_setup.asyncImageWrite) {
        writeImages(false);
    }

    bool success;
    while (true) {
        // Wait until we are supposed to get the next image
        // spinLockWait(100);

        m_img = m_imageGetterFunction(success);

        if (!success || m_asyncStopAnalyzer) {
            m_asyncStopAnalyzer = false;  // reset, such that the same flag can be used to stop
                                          // further execution steps
            break;
        }

        if (m_bg.cols != m_img.cols || m_bg.rows != m_img.rows) {
            m_img.copyTo(m_bg);
        } else {
            m_experiment.raw.enqueue(m_img.clone());
            if (m_setup.runProcessing) {
                processImage(m_img, m_bg);
                m_experiment.processed.enqueue(m_img.clone());
            }
            m_imageCnt++;
        }
    }
}

/**
 * @brief Saves current images in rawBuffer and processed to disk
 * @param setup
 */
void Analyzer::writeImages(bool waitForFinish) {
    fs::path experimentFolder = fs::path(m_setup.outputPath) / fs::path(m_setup.experimentName);
    fs::path rawPath = experimentFolder / fs::path(m_setup.rawPrefix);
    fs::path processedPath = experimentFolder / fs::path(m_setup.processedPrefix);

    if (m_setup.storeProcessed)
        m_experiment.writeBuffer_processed.startWriting(processedPath, m_setup.processedPrefix);
    if (m_setup.storeRaw)
        m_experiment.writeBuffer_raw.startWriting(rawPath, m_setup.rawPrefix);

    if (waitForFinish) {
        // Wait for finished. This is NOT done when asynchronous image writing is enabled
        if (m_setup.storeProcessed)
            m_experiment.writeBuffer_processed.finishWriting(m_imageCnt);
        if (m_setup.storeRaw)
            m_experiment.writeBuffer_raw.finishWriting(m_imageCnt);
    }
}

void Analyzer::asyncStop() {
    // stops all objects which are handled by analyzer
    // stop objectfinder before image writers!
    m_experiment.writeBuffer_processed.forceStop();
    m_experiment.writeBuffer_raw.forceStop();
}

void Analyzer::reset() {
    m_experiment.reset();
    m_objectFinder.reset();

    m_asyncStopAnalyzer = false;
    m_status = 0;
    m_imageCnt = 0;
}

/**
 * @brief Clears the process tree
 * @details
 *
 */
void Analyzer::resetProcesses() {
    m_processes.clear();
}

/**
 * @brief
 * @details
 *
 */
void Analyzer::findObjects() {
    m_currentProcessingFrame = 0;

    // Consume until queue is empty
    while (m_experiment.processed.peek() != nullptr) {
        CHECK_ASYNC_STOP
        if (m_setup.extractData) {
            m_objectFinder.setImages(m_experiment.raw.peek(), m_experiment.processed.peek());
            m_objectFinder.findObjects(m_experiment, m_setup);
        }

        // Pop images from raw and processed to write buffers
        if (m_setup.storeProcessed) {
            m_experiment.writeBuffer_processed.push(m_experiment.processed.peek()->clone());
        }
        if (m_setup.storeRaw)
            m_experiment.writeBuffer_raw.push(m_experiment.raw.peek()->clone());

        m_experiment.processed.pop();
        m_experiment.raw.pop();
        m_currentProcessingFrame++;
    }

    if (m_setup.extractData) {
        m_objectFinder.cleanObjects(m_experiment);
    }

    ASYNC_END
}

/**
 * @brief
 * @details
 *
 * @param path
 * @return
 */
bool Analyzer::storeSetup(const string& path) {
    // Serialize current processes in m_processes in .xml file
    try {
        std::ofstream ofs(path);
        {
            boost::archive::xml_oarchive oa(ofs);
            oa << BOOST_SERIALIZATION_NVP(m_processes);
        }
        ofs.close();
    } catch (...) {
        return false;
    }
    return true;
}

/**
 * @brief
 * @details
 *
 * @param path
 * @return
 */
bool Analyzer::loadSetup(const string& path) {
    try {
        m_processes.clear();
        std::ifstream ifs(path);
        {
            boost::archive::xml_iarchive ia(ifs);
#ifndef __linux__  // https://stackoverflow.com/questions/50038329/serializing-stdvector-of-unique-ptr-using-boostserialization-fails-on-linux
            ia >> BOOST_SERIALIZATION_NVP(m_processes);
#endif
        }
        ifs.close();
    } catch (...) {
        return false;
    }
    return true;
}

/**
 *
 * @param datacontainers
 * @param path
 * @return
 */

void Analyzer::exportExperiment(const string& path) {
    if (m_experiment.data.empty()) {
        // No objects to export
        m_status |= StatusBits::NoObjectsFound;
        return;
    }

    std::vector<std::string> attributes = m_experiment.data[0]->extractAttributeName();
    std::ofstream out(path);
    // Add list of attributes
    for (const auto& attribute : attributes) {
        out << attribute << "'";
    }
    out << "\n";

    // Add number number of values for chosen attributes
    std::vector<int> attributeLengths = m_experiment.data[0]->extractAttributeLengths();
    for (const auto& value : attributeLengths) {
        out << value << " ";
    }

    // Add number of containers
    out << "\n" << m_experiment.data.size() << "\n";

    // Goes through all containers
    for (int i = 0; i < m_experiment.data.size(); i++) {
        CHECK_ASYNC_STOP
        out << "Observation" << (i + 1) << " " << m_experiment.data[i]->size() << "\n";

        // Goes through all objects
        for (int j = 0; j < m_experiment.data[i]->size(); j++) {
            // Extracts vector of doubles from object
            std::vector<double> objectVector = m_experiment.data[i]->extractObjectInDoubles(j);
            for (const auto& item : objectVector) {
                out << item << " ";
            }
            out << "\n";
        }
    }
    out.close();
    ASYNC_END_SIDEEFFECT(([&out]() { out.close(); }))
}

void Analyzer::processImage(cv::Mat& img, cv::Mat& bg) {
    for (const auto& process : m_processes) {
        process->doProcessing(img, bg, m_experiment);
    }
}

long Analyzer::getSetupDataFlags() {
    return m_setup.dataFlags;
}
