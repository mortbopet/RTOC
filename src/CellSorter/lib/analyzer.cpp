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
    double dist_thres;
    std::vector<double> d;
    bool newcell = false;
    int cellNum = -1;
    int sameCell = 0;
    int frameNo = 0;
    Tracker t;
    std::vector<Tracker> tracker, currentCells;
    int numObjects = 0;
    DataContainer dc(0xffff);

    for (const Frame& f : m_Experiment.processed) {
        cv::Mat img_to_show = f.image;

        // Get data from blobs in frame
        numObjects = matlab::regionProps(f.image, 0xffff, dc);

        for (int i = 0; i < numObjects; i++) {
            if (cellNum < 0) {
                newcell = true;
            } else {
                Tracker term(frameNo-1);

                currentCells = matlab::find<Tracker>(tracker, term);

                if (currentCells.empty()) {
                    newcell = true;
                } else {

                    // Calculate distances
                    for (const Tracker &cc : currentCells) {
                        centroid = dc[i]->getValue<cv::Point>(data::Centroid);
                        dist = matlab::dist(centroid, cc.centroid);
                        if (dist < -10) dist = 100;
                        d.push_back(dist);
                    }

                    std::pair<double,unsigned long> p = matlab::min<double>(d);
                    dist = p.first;
                    t = currentCells.at(p.second);

                    d.clear();
                    currentCells.clear();

                    // Set threshold
                    if (centroid.x <= m_Experiment.inlet - 5) {
                        dist_thres = 5.0;   // Should be a settable variable
                    } else {
                        dist_thres = 20.0;  // Should be a settable variable
                    }

                    // Determine whether new or not from threshold
                    newcell = dist > dist_thres;
                }
            }
            if (newcell) {
                cellNum++;

                m_Experiment.data.emplace_back(new DataContainer(0xffff));
                m_Experiment.data[cellNum]->appendNew();

                (*m_Experiment.data[cellNum])[0]->setValue(data::Inlet, m_Experiment.inlet);
                (*m_Experiment.data[cellNum])[0]->setValue(data::Outlet, m_Experiment.outlet);
                // yref ???
                (*m_Experiment.data[cellNum])[0]->setValue(data::Label, cellNum);

                (*m_Experiment.data[cellNum])[0]->setValue(data::Frame, frameNo);
                (*m_Experiment.data[cellNum])[0]->setValue(data::Centroid, dc[i]->getValue<cv::Point>(data::Centroid));
                (*m_Experiment.data[cellNum])[0]->setValue(data::BoundingBox, dc[i]->getValue<cv::Rect>(data::BoundingBox));
                (*m_Experiment.data[cellNum])[0]->setValue(data::Major_axis, dc[i]->getValue<double>(data::Major_axis));
                (*m_Experiment.data[cellNum])[0]->setValue(data::Eccentricity, dc[i]->getValue<double>(data::Eccentricity));
                (*m_Experiment.data[cellNum])[0]->setValue(data::Circularity, dc[i]->getValue<double>(data::Circularity));
                (*m_Experiment.data[cellNum])[0]->setValue(data::Symmetry, dc[i]->getValue<double>(data::Symmetry));
                (*m_Experiment.data[cellNum])[0]->setValue(data::GradientScore, dc[i]->getValue<double>(data::GradientScore));

                t.cell_no = cellNum;
            } else {
                sameCell = t.cell_no;


                m_Experiment.data[sameCell]->appendNew();
                auto index = m_Experiment.data[sameCell]->size() - 1;

                (*m_Experiment.data[sameCell])[index]->setValue(data::Frame, frameNo);
                (*m_Experiment.data[sameCell])[index]->setValue(data::Centroid, dc[i]->getValue<cv::Point>(data::Centroid));
                (*m_Experiment.data[sameCell])[index]->setValue(data::BoundingBox, dc[i]->getValue<cv::Rect>(data::BoundingBox));
                (*m_Experiment.data[sameCell])[index]->setValue(data::Major_axis, dc[i]->getValue<double>(data::Major_axis));
                (*m_Experiment.data[sameCell])[index]->setValue(data::Eccentricity, dc[i]->getValue<double>(data::Eccentricity));
                (*m_Experiment.data[sameCell])[index]->setValue(data::Circularity, dc[i]->getValue<double>(data::Circularity));
                (*m_Experiment.data[sameCell])[index]->setValue(data::Symmetry, dc[i]->getValue<double>(data::Symmetry));
                (*m_Experiment.data[sameCell])[index]->setValue(data::GradientScore, dc[i]->getValue<double>(data::GradientScore));
            }
            t.frame_no = frameNo;
            t.centroid = dc[i]->getValue<cv::Point>(data::Centroid);
            tracker.push_back(t);

            cv::putText(img_to_show, std::to_string(t.cell_no), dc[i]->getValue<cv::Point>(data::Centroid),cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(128), 3);
        }

        frameNo++;

        cv::namedWindow("show blob",cv::WINDOW_AUTOSIZE);
        cv::imshow("show blob", img_to_show);
        cv::waitKey(1);

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
