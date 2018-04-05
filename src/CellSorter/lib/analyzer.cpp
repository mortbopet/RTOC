#include "analyzer.h"

void Analyzer::loadRBCPreset() {
    // Subtract background
    auto subtractbgDEFAULT = std::make_unique<SubtractBG>();
    subtractbgDEFAULT->m_edgeThreshold.setValue(0.272);
    m_processes.push_back(std::move(subtractbgDEFAULT));

    // Normalize
    auto normalizeDEFAULT = std::make_unique<Normalize>();
    normalizeDEFAULT->m_normalizeStrength.setValue(0xfff);
    m_processes.push_back(std::move(normalizeDEFAULT));

    // Binarize
    auto binarizeDEFAULT = std::make_unique<Binarize>();
    binarizeDEFAULT->m_maxVal.setValue(255);
    binarizeDEFAULT->m_edgeThreshold.setValue(50);
    m_processes.push_back(std::move(binarizeDEFAULT));

    // Morph open (imopen)
    auto morphDEFAULT = std::make_unique<Morph>();
    morphDEFAULT->m_morphType.setValue(cv::MORPH_OPEN);
    morphDEFAULT->m_morphValueX.setValue(3);
    morphDEFAULT->m_morphValueY.setValue(3);
    m_processes.push_back(std::move(morphDEFAULT));

    // Morph close (imclose)
    morphDEFAULT = std::make_unique<Morph>();
    morphDEFAULT->m_morphType.setValue(cv::MORPH_CLOSE);
    morphDEFAULT->m_morphValueX.setValue(15);
    morphDEFAULT->m_morphValueY.setValue(10);
    m_processes.push_back(std::move(morphDEFAULT));

    // Floodfill
    m_processes.push_back(std::make_unique<FloodFill>());

    // Clearborder
    auto borderDEFAULT = std::make_unique<ClearBorder>();
    borderDEFAULT->m_borderWidth.setValue(2);
    m_processes.push_back(std::make_unique<ClearBorder>());

    // bwpropfilt ConvexArea [200 1450]
    auto propFilt0 = std::make_unique<PropFilter>();
    propFilt0->m_regionPropsTypes.setValue(matlab::regionPropTypes::ConvexArea);
    propFilt0->m_lowerLimit.setValue(200);
    propFilt0->m_upperLimit.setValue(1450);
    m_processes.push_back(std::move(propFilt0));

    // bwpropfilt MajorAxisLength [0 65]
    auto propFilt1 = std::make_unique<PropFilter>();
    propFilt1->m_regionPropsTypes.setValue(matlab::regionPropTypes::Major_axis);
    propFilt1->m_lowerLimit.setValue(0);
    propFilt1->m_upperLimit.setValue(65);
    m_processes.push_back(std::move(propFilt1));
}

void Analyzer::loadExperimentPreset(const std::string& img_path) {
    m_Experiment.defaultSettings(img_path);
}

void Analyzer::loadImagesFromFolder() {
    std::vector<Frame> frames;
    std::string img_folder = m_Experiment.imagePath;
    get_files(frames, img_folder);
    accept_or_reject(frames, img_folder, m_Experiment.intensity_threshold);
    get_accepted(frames, m_Experiment.acc);
    get_rejected(frames, m_Experiment.dis);
}

/**
 * @todo: implement with real names
 */
void Analyzer::loadImagesFromText() {
    std::string accepted;   // Path to accepted text-file
    std::string rejected;   // Path to rejected text-file
    // Compile img_folder path with filenames from text-files
    // and push frames to m_Experiment
}

void Analyzer::selectBG() {
    m_bg = m_Experiment.dis[10].image;  // Sets as background
}

void Analyzer::runProcesses() {
    for (const auto& process : m_processes) {
        process->doProcessing(m_img, m_bg, m_Experiment);
    }
}

void Analyzer::resetProcesses() {
    m_processes.clear();
}

void Analyzer::findCells() {
    cv::Point centroid;
    double dist;
    std::vector<double> d;
    bool newcell = false;
    int cellNum = 0;
    Tracker t;
    std::vector<Tracker> tracker(m_Experiment.processed.size());
    DataContainer dc;

    for (const Frame& f : m_Experiment.processed) {
        // Get data from blobs in frame
        matlab::regionProps(f.image, 0xffff, dc);
        for (int i = 0; i < dc.size(); i++) {
            if (cellNum == 0) {
                newcell = true;
            } else {
                if (tracker.size() == 0) {
                    newcell = true;
                } else {
                    // Calculate distances
                    for (const Tracker& trk : tracker) {
                        centroid = dc[i]->getValue<cv::Point>(data::Centroid);
                        dist = matlab::dist(centroid, trk.centroid);
                        if (dist < -10) dist = 100;
                        d.push_back(dist);
                    }
                    if (d.size() > 0) {
                        auto p = matlab::min(d);
                        dist = p.first;
                        t = tracker[p.second];
                    }
                    // FROM LINE 177 in CELLSORTER **HERE**
                }
            }
        }
    }

}

void Analyzer::showImg(const int& delay) {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", m_img);
    cv::waitKey(delay);
}

void Analyzer::showImg(const cv::Mat& img, const int& delay) {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", img);
    cv::waitKey(delay);
}
