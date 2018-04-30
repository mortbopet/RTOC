#include "analyzer.h"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/filesystem.hpp>

#include <chrono>

/**
 * @brief
 * @param img_path
 */
void Analyzer::loadExperimentPreset(const std::string& img_path) {
    m_experiment.defaultSettings(img_path);
}

/**
 * @brief
 *
 * @warning DEPRECATED
 */
void Analyzer::loadImagesFromFolder() {
    std::vector<framefinder::Frame> frames;
    std::string img_folder = m_experiment.imagePath;
    get_files(frames, img_folder);
    accept_or_reject(frames, img_folder, m_experiment.intensity_threshold);
    get_accepted(frames, m_experiment.acc);
    get_rejected(frames, m_experiment.dis);
}

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
 * @brief
 *
 * @warning DEPRECATED
 */
void Analyzer::selectBG() {
    m_bg = m_experiment.dis[10].image;  // Sets as background
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
    if (m_bg.empty()) {
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

namespace fs = boost::filesystem;

namespace {
typedef std::chrono::high_resolution_clock Clock;
}

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

    bool success;
    while (true) {
        // Wait until we are supposed to get the next image
        // spinLockWait(100);

        m_img = m_imageGetterFunction(success);

        if (!success || m_asyncStopAnalyzer) {
            break;
        }
        if (m_setup.storeRaw) {
            m_experiment.rawBuffer.push_back(m_img.clone());
        }

        if (m_bg.dims == 0) {
            m_bg = m_img;
        } else {
            if (m_setup.runProcessing) {
                processImage(m_img, m_bg);
                m_experiment.processed.push_back({m_img, "", m_experiment.cellNum++, true});
            }
        }
    }
}
/**
 * @brief Saves current images in rawBuffer and processed to disk
 * @param setup
 */
void Analyzer::writeImages() {
    fs::path experimentFolder = fs::path(m_setup.outputPath) / fs::path(m_setup.experimentName);
    fs::path rawPath = experimentFolder / fs::path(m_setup.rawPrefix);
    fs::path processedPath = experimentFolder / fs::path(m_setup.processedPrefix);

    m_currentProcessingFrame = 0;

    if (m_setup.storeRaw) {
        int index = 0;
        for (const auto& image : m_experiment.rawBuffer) {
            m_currentProcessingFrame++;
            // store raw
            std::string filepath =
                (rawPath / fs::path(m_setup.rawPrefix + "_" + to_string(index) + ".png")).string();
            cv::imwrite(filepath, image);
            index++;
        }
    }

    // store processed
    if (m_setup.storeProcessed) {
        int index = 0;
        for (const auto& frame : m_experiment.processed) {
            m_currentProcessingFrame++;
            std::string filepath = (processedPath / fs::path(m_setup.processedPrefix + "_" +
                                                             to_string(index) + ".png"))
                                       .string();
            cv::imwrite(filepath, frame.image);
            index++;
        }
    }
}

void Analyzer::resetAnalyzer() {
    m_experiment.reset();
    m_asyncStopAnalyzer = false;
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
    if (m_setup.extractData) {
        for (const framefinder::Frame& f : m_experiment.processed) {
            m_objectFinder.setFrame(f.image);
            m_objectFinder.findObjects(m_experiment);
        }

        m_objectFinder.cleanObjects(m_experiment);
    }
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
    std::vector<std::string> attributes = m_experiment.data[0]->extractAttributeName();
    std::ofstream out(path);
    // Adds list of attributes
    for (int i = 0; i < attributes.size(); i++) {
        out << attributes[i] << "'";
    }
    out << "\n";

    // Adds number number of values for chosen attributes
    std::vector<int> attributeLengths = m_experiment.data[0]->extractAttributeLengths();
    for (const auto& value : attributeLengths) {
        out << value << " ";
    }

    // Adds number of containers
    out << "\n" << m_experiment.data.size() << "\n";

    // Goes through all containers
    for (int i = 0; i < m_experiment.data.size(); i++) {
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
}

void Analyzer::processImage(cv::Mat& img, cv::Mat& bg) {
    for (const auto& process : m_processes) {
        process->doProcessing(img, bg, m_experiment);
    }
}
