#include "objectfinder.h"

// --------------------- ObjectHandler ---------------------
ObjectHandler::ObjectHandler(Experiment* experiment, Setup* s, unsigned long conditionFlags)
    : m_experiment(experiment), m_setup(s), m_conditionFlags(conditionFlags) {
    m_conditionFlags = conditionFlags;
    setup();
}

void ObjectHandler::setup() {
    if (FrameCount & m_conditionFlags) {
        add(frameCount);
    }
    if (FrameBeforeInlet & m_conditionFlags) {
        add(frameBeforeInlet);
    }
    if (FrameAfterOutlet & m_conditionFlags) {
        add(frameAfterOutlet);
    }
}

// --------------------- ObjectFinder ---------------------
ObjectFinder::ObjectFinder(Experiment* experiment, Setup* setup)
    : m_experiment(experiment), m_setup(setup) {
    // Initialize ConnectedComponents-DataContainer
    m_cc.setDataFlags(data::AllFlags);
    // Setup ObjectHandler helper-class
    handler = new ObjectHandler(experiment, setup);
    if (m_setup->classifyObjects) {
        // Setup Machinelearning helper-class
        ml_model = identifyModel(setup->modelPath);
        ml_model->loadModel(setup->modelPath);
    }
}

/**
 * @brief
 * @return Count of found objects
 */
int ObjectFinder::findObjects() {
    if (m_dataFlags != m_setup->dataFlags) {
        m_dataFlags = m_setup->dataFlags;
    }

    m_numObjects = mathlab::regionProps(m_processedImg, mathlab::WithoutPixelIdxList, m_cc);

    Tracker term(m_frameNum - 1);
    m_frameTracker = mathlab::find<Tracker>(m_trackerList, term);
    m_trackerList = m_frameTracker;

    for (int i = 0; i < m_numObjects; i++) {
        if (m_cellNum <= 0) {
            m_newObject = true;
        } else {
            if (m_frameTracker.empty()) {
                m_newObject = true;
            } else {
                // Find relative xpos and nearest object from previous frame
                m_centroid = m_cc[i]->getValue<cv::Point>(data::Centroid);
                m_xpos = mathlab::relativeX(m_centroid, m_experiment->inlet_line);
                auto res = findNearestObject(m_centroid, m_frameTracker);
                m_dist = res.first;
                m_track = m_frameTracker.at(res.second);

                // Set threshold
                double distThresh =
                    m_xpos < 0 ? m_setup->distanceThresholdInlet : m_setup->distanceThresholdPath;
                // Determine whether new or not from threshold
                m_newObject = m_dist > distThresh;
                m_frameTracker.at(res.second).found = !m_newObject;
            }
        }

        writeToDataVector(i, *m_experiment);
    }

    // Classify objects
    if (m_setup->classifyObjects) {
        // Go thru objects not found in this frame
        for (Tracker& t : m_frameTracker) {
            if (!t.found) {
                if (!handler->invoke_all(m_experiment->data[t.cell_no].get())) {
                    int type = ml_model->predictObject(*m_experiment->data[t.cell_no].get());
                    m_experiment->data[t.cell_no]->front()->setValue(data::OutputValue, (double) type);
                    //std::cout << "Cell classified as: " << type << "\n";
                }
            }
        }
    }

    m_frameNum++;
    return m_numObjects;
}

// -------------------------- Concurrent Object Finder --------------------------
/**
 *
 * @param targetImageCount
 */
void ObjectFinder::waitForThreadToFinish(int targetImageCount) {
    // Set target images that we require to be written to disk and wait for finished image
    // writing
    m_targetImageCount = targetImageCount;
    while (!m_finishedWriting)
        ;
}

/**
 *
 */
void ObjectFinder::findObjectsThreaded() {
    auto waitTime = std::chrono::microseconds(1);
    bool pImg_ready;
    bool rImg_ready;

    while (!(m_targetImageCount >= 0 &&
             m_targetImageCount <= m_experiment->m_currentProcessingFrame)) {
        if (m_forceStop) {
            goto asyncStop;
        }
        pImg_ready = m_experiment->processed.peek() != nullptr;
        rImg_ready = m_experiment->raw.peek() != nullptr;

        if (pImg_ready && rImg_ready) {
            // Images are ready, blocking-wait for load
            m_experiment->processed.wait_dequeue(m_processedImg);
            m_experiment->raw.wait_dequeue(m_rawImg);

            // Extract data if set
            if (m_setup->extractData) {
                findObjects();
            }

            // Done using them Pop images from raw and processed to write buffers
            if (m_setup->storeProcessed) {
                m_experiment->writeBuffer_processed.push(m_processedImg);
            }
            if (m_setup->storeRaw) {
                m_experiment->writeBuffer_raw.push(m_rawImg);
            }
            m_experiment->m_currentProcessingFrame++;
        } else {
            // A set of raw and processed images are not yet ready, so we wait
        }
    }
    // All frames analyzed - clean objects that don't meet conditions set in ObjectHandler
    if (m_setup->extractData) {
        cleanObjects();
    }

    m_finishedWriting = true;
// If m_forceStop is invoked, stop without calling cleanObjects()
asyncStop:
    m_running = false;
}

void ObjectFinder::startThread() {
    if (!m_running) {
        m_running = true;
        m_finishedWriting = false;
        m_targetImageCount = -1;

        // Setup last parameters before starting thread
        if (m_dataFlags != m_setup->dataFlags) {
            m_dataFlags = m_setup->dataFlags;
        }
        m_experiment->m_currentProcessingFrame = 0;
        std::thread t(&ObjectFinder::findObjectsThreaded, this);
        t.detach();
    }
}

bool ObjectFinder::approveContainer(const DataContainer& dataContainer) {
    return handler->invoke_all(&dataContainer);
}

/**
 * @brief Removes objects from foundObject-list from Experiment e
 * @param e Experiment
 * @return How many objects that have been removed
 */
unsigned long ObjectFinder::cleanObjects() {
    auto data = &m_experiment->data;  // Increase readability

    unsigned long length = data->size();  // Get initial count of objects

    // Use erase, remove-if
    data->erase(
        std::remove_if(data->begin(), data->end(),
                       [&](const auto& dc) -> bool { return handler->invoke_all(dc.get()); }),
        data->end());

    return length - data->size();  // Return new count of objects
}

/**
 * @brief
 * @param objects
 * @return <distance, Tracker-object>
 */
std::pair<double, unsigned long>
ObjectFinder::findNearestObject(const cv::Point& object,
                                std::vector<Tracker>& objects) {
    std::vector<double> d;
    // Calculate distances
    for (const Tracker& cc : objects) {
        auto dist = mathlab::dist(object, cc.centroid);
        if (dist < -10) // Todo: Is this even necessary?
            dist = 100;
        d.push_back(dist);
    }
    std::pair<double, unsigned long> p = mathlab::min<double>(d);
    return {p.first, p.second};
}

/**
 * @brief
 * @param newObject
 * @param cc_i
 * @param experiment
 */
void ObjectFinder::writeToDataVector(const int& cc_i, Experiment& experiment) {
    int i;
    if (m_newObject) {
        experiment.data.emplace_back(new DataContainer(data::AllFlags));
        i = m_cellNum;
        m_track.cell_no = m_cellNum;
        m_cellNum++;
    } else {
        i = m_track.cell_no;
    }
    assert(i < experiment.data.size());  // debug
    experiment.data[i]->appendNew();

    // Get some data (should be moved)
    double gradientScore =
        mathlab::gradientScore(m_rawImg, m_cc[cc_i]->getValue<cv::Rect>(data::BoundingBox));
    double symmetry =
        mathlab::verticalSymmetry(m_rawImg, m_cc[cc_i]->getValue<cv::Rect>(data::BoundingBox),
                                  m_cc[cc_i]->getValue<double>(data::Major_axis));

    auto dc_ptr = (*experiment.data[i]).back();
    dc_ptr->setValue(data::Area, m_cc[cc_i]->getValue<double>(data::Area));
    dc_ptr->setValue(data::BoundingBox, m_cc[cc_i]->getValue<cv::Rect>(data::BoundingBox));
    dc_ptr->setValue(data::Centroid, m_centroid);
    dc_ptr->setValue(data::Circularity, m_cc[cc_i]->getValue<double>(data::Circularity));
    dc_ptr->setValue(data::ConvexArea, m_cc[cc_i]->getValue<double>(data::ConvexArea));
    dc_ptr->setValue(data::Eccentricity, m_cc[cc_i]->getValue<double>(data::Eccentricity));
    dc_ptr->setValue(data::Frame, m_frameNum);
    dc_ptr->setValue(data::GradientScore, gradientScore);
    dc_ptr->setValue(data::Inlet, m_setup->inlet);
    dc_ptr->setValue(data::Outlet, m_setup->outlet);
    dc_ptr->setValue(data::Label, m_cellNum);
    dc_ptr->setValue(data::Major_axis, m_cc[cc_i]->getValue<double>(data::Major_axis));
    dc_ptr->setValue(data::Minor_axis, m_cc[cc_i]->getValue<double>(data::Minor_axis));
    dc_ptr->setValue(data::Solidity, m_cc[cc_i]->getValue<double>(data::Solidity));
    dc_ptr->setValue(data::Symmetry, symmetry);
    dc_ptr->setValue(data::Perimeter, m_cc[cc_i]->getValue<double>(data::Perimeter));
    double outputValue = 0.0;
    dc_ptr->setValue(data::OutputValue, outputValue);
    dc_ptr->setValue(data::RelativeXpos, m_xpos);

    m_track.frame_no = m_frameNum;
    m_track.centroid = m_centroid;
    m_track.found = false;
    m_trackerList.push_back(m_track);
}

/**
 * Resets ObjectFinder members, prior to new experiment
 */
void ObjectFinder::reset() {
    m_trackerList.clear();
    m_frameTracker.clear();

    m_cellNum = 0;
    m_frameNum = 0;
    m_newObject = false;
    m_numObjects = 0;
    m_processedImg.release();
    m_rawImg.release();
}

/**
 * Lock to ensure thread is closed
 */
void ObjectFinder::waitForThreadToClose() {
    while (m_running)
        ;
}
