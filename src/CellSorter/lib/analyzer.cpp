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
    resetAnalyzer();                   \
    function();                        \
    return;

// End statement for a CHECK_ASYNC_STOP function, with no sideeffects
#define ASYNC_END    \
    return;          \
    async_stop:      \
    resetAnalyzer(); \
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
    resetAnalyzer();

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
            m_bg = m_img;
        } else {
            m_experiment.raw.push(m_img.clone());
            if (m_setup.runProcessing) {
                processImage(m_img, m_bg);
                m_experiment.processed.push(m_img.clone());
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

void Analyzer::resetAnalyzer() {
    m_experiment.reset();
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

    while (!m_experiment.processed.empty() && !m_experiment.raw.empty()) {
        CHECK_ASYNC_STOP
        if (m_setup.extractData) {
            m_objectFinder.setRawImage(m_experiment.raw.front());
            m_objectFinder.setProcessedImage(m_experiment.processed.front());
            m_objectFinder.findObjects(m_experiment, m_setup);
        }

        // Pop images from raw and processed to write buffers
        if (m_setup.storeProcessed)
            m_experiment.writeBuffer_processed.push(m_experiment.processed.front());
        if (m_setup.storeRaw)
            m_experiment.writeBuffer_raw.push(m_experiment.raw.front());

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
 * @brief Clean Object-vector from objects with insufficient data
 * @details
 *
 * @warning NOT FULLY IMPLEMENTED
 */
void Analyzer::cleanObjects() {
    if (m_setup.extractData) {
        unsigned int count_threshold = 25;

        auto n = m_experiment.data.size();
        std::vector<bool> remove(n);

        for (unsigned long i = 0; i < n; i++) {
            CHECK_ASYNC_STOP
            DataContainer* dc = m_experiment.data[i].get();
            cv::Rect bb_i = (*dc).front()->getValue<cv::Rect>(data::BoundingBox);
            cv::Rect bb_o = (*dc).back()->getValue<cv::Rect>(data::BoundingBox);
            remove.at(i) = (*dc).size() < 25 || ((bb_i.x + bb_i.width) > m_experiment.inlet - 1) ||
                           ((bb_o.x + bb_o.width) < m_experiment.outlet);
        }

        /// Following should be part of dependent experiment subclass
        /// Where subclass type_of_experiment includes the different remove-conditions ig
        /// __def: remove_cond1():__
        ///     [thresh = count_threshold](const auto& dc) {
        ///         return (*dc).size() < thresh;
        ///     }
        ///
        // Remove objects with less than #count_threshold frames
        m_experiment.data.erase(std::remove_if(m_experiment.data.begin(), m_experiment.data.end(),
                                               [thresh = count_threshold](const auto& dc) {
                                                   return (*dc).size() < thresh;
                                               }),
                                m_experiment.data.end());

        // Remove objects not represented before inlet
        m_experiment.data.erase(
            std::remove_if(m_experiment.data.begin(), m_experiment.data.end(),
                           [inlet = m_experiment.inlet](const auto& dc) {
                               cv::Rect bb_i =
                                   (*dc).front()->template getValue<cv::Rect>(data::BoundingBox);
                               return ((bb_i.x + bb_i.width) > inlet - 1);
                           }),
            m_experiment.data.end());

        // Remove objects not represented after outlet
        m_experiment.data.erase(
            std::remove_if(m_experiment.data.begin(), m_experiment.data.end(),
                           [outlet = m_experiment.outlet](const auto& dc) {
                               cv::Rect bb_o =
                                   (*dc).back()->template getValue<cv::Rect>(data::BoundingBox);
                               return ((bb_o.x + bb_o.width) < outlet);
                           }),
            m_experiment.data.end());
    }
    ASYNC_END
}

/**
 * @brief function for storing data from experiment
 * @details Stores the contents of `m_experiment.data` to some external file
 *
 * @param path
 * @return
 */
bool Analyzer::storeData(const std::string& path) {
    return false;
}

/// Debug helpers
void Analyzer::showImg(const int& delay) {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", m_img);
    cv::waitKey(delay);
}
/// Debug helpers
void Analyzer::showImg(const cv::Mat& img, const int& delay) {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", img);
    cv::waitKey(delay);
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
