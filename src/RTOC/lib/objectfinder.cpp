#include "objectfinder.h"


// --------------------- ObjectHandler ---------------------
ObjectHandler::ObjectHandler(Experiment* experiment) : m_experiment(experiment) {}

void ObjectHandler::setup(long handleFlags) {
    if (FrameCount & handleFlags) {
        add(frameCount);
    }
    if (FrameBeforeInlet & handleFlags) {
        add(frameBeforeInlet);
    }
    if (FrameAfterOutlet & handleFlags) {
        add(frameAfterOutlet);
    }
}


// --------------------- ObjectFinder ---------------------
ObjectFinder::ObjectFinder(Experiment* experiment, Setup* setup) : m_experiment(experiment), m_setup(setup) {
    m_cc.setDataFlags(data::AllFlags);
    handler = new ObjectHandler(experiment);
    handler->setup(m_setup->conditionFlags); // set flags
}

/**
 * @brief Main method of objectfinder
 * @return Count of found objects
 */
int ObjectFinder::findObjects() {
    if (m_dataFlags != m_setup->dataFlags) {
        m_dataFlags = m_setup->dataFlags;
    }

    m_numObjects = mathlab::regionProps(*m_processedImg, 0xffff, m_cc);

    for (int i = 0; i < m_numObjects; i++) {
        if (m_cellNum <= 0) {
            m_newObject = true;
        } else {
            Tracker term(m_frameNum - 1);

            m_frameTracker = mathlab::find<Tracker>(m_trackerList, term);

            if (m_frameTracker.empty()) {
                m_newObject = true;
            } else {
                m_centroid = m_cc[i]->getValue<cv::Point>(data::Centroid);
                auto res = findNearestObject(m_centroid, m_frameTracker);
                m_dist = res.first;
                m_track = res.second;

                // Set threshold
                if (m_centroid.x <= m_experiment->inlet - 5) {
                    m_distThreshold = 5.0;  // Should be a settable variable
                } else {
                    m_distThreshold = 20.0;  // Should be a settable variable
                }

                // Determine whether new or not from threshold
                m_newObject = m_dist > m_distThreshold;
            }
        }

        writeToDataVector(i, *m_experiment);
    }
    m_frameNum++;
    return m_numObjects;
}

// -------------------------- Concurrent Object Finder --------------------------
void ObjectFinder::runThreaded() {
    m_experiment->m_currentProcessingFrame = 0;

    while (true) {
        if (m_forceStop) {
            // If we should stop
            break;
        }

        if (m_setup->extractData) {
            // Wait here until images are ready
            m_experiment->processed.wait_dequeue(m_processedImg);
            m_experiment->raw.wait_dequeue(m_rawImg);

            // Run ObjectFinder
            findObjects();
        }

        // Pop images from raw and processed to write buffers
        if (m_setup->storeProcessed) {
            m_experiment->writeBuffer_processed.push(m_experiment->processed.peek()->clone());
        }
        if (m_setup->storeRaw)
            m_experiment->writeBuffer_raw.push(m_experiment->raw.peek()->clone());

        // Pop from original queue
        m_experiment->processed.pop();
        m_experiment->raw.pop();

        m_experiment->m_currentProcessingFrame++;
    }

    if (m_setup->extractData) {
        cleanObjects();
    }

}

bool ObjectFinder::handleObject(const DataContainer& dataContainer) {
    return handler->invoke_all(&dataContainer);
}

/**
 * @brief Removes objects from foundObject-list from Experiment e
 * @param e Experiment
 * @return How many objects that have been removed
 */
unsigned long ObjectFinder::cleanObjects() {
    auto data = &m_experiment->data; // Increase readability

    unsigned long length = data->size();    // Get initial count of objects

    // Use erase, remove-if
    data->erase(std::remove_if(data->begin(), data->end(), [&](const auto& dc) -> bool {
        return handler->invoke_all(dc.get());
    }), data->end());

    return length - data->size();   // Return new count of objects
}


/**
 * @brief
 * @param listOfObjects
 * @return <distance, Tracker-object>
 */
std::pair<double, Tracker>
ObjectFinder::findNearestObject(const cv::Point& object,
                                const std::vector<Tracker>& listOfObjects) {
    std::vector<double> d;
    // Calculate distances
    for (const Tracker& cc : listOfObjects) {
        auto dist = mathlab::dist(object, cc.centroid);
        if (dist < -10)
            dist = 100;
        d.push_back(dist);
    }
    std::pair<double, unsigned long> p = mathlab::min<double>(d);
    return {p.first, listOfObjects.at(p.second)};
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
    assert(i < experiment.data.size()); // debug
    experiment.data[i]->appendNew();

    // Get some data (should be moved)
    double gradientScore =
        mathlab::gradientScore(*m_rawImg, m_cc[cc_i]->getValue<cv::Rect>(data::BoundingBox));
    double symmetry =
        mathlab::verticalSymmetry(*m_rawImg, m_cc[cc_i]->getValue<cv::Rect>(data::BoundingBox),
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
    dc_ptr->setValue(data::Inlet, experiment.inlet);
    dc_ptr->setValue(data::Outlet, experiment.outlet);
    dc_ptr->setValue(data::Label, m_cellNum);
    dc_ptr->setValue(data::Major_axis, m_cc[cc_i]->getValue<double>(data::Major_axis));
    dc_ptr->setValue(data::Minor_axis, m_cc[cc_i]->getValue<double>(data::Minor_axis));
    dc_ptr->setValue(data::Solidity, m_cc[cc_i]->getValue<double>(data::Solidity));
    dc_ptr->setValue(data::Symmetry, symmetry);
    dc_ptr->setValue(data::Perimeter, m_cc[cc_i]->getValue<double>(data::Perimeter));
    double outputValue = 0.0;
    dc_ptr->setValue(data::OutputValue, outputValue);

    m_track.frame_no = m_frameNum;
    m_track.centroid = m_centroid;
    m_trackerList.push_back(m_track);
}

/**
 * Resets ObjectFinder members, prior to new experiment
 */
void ObjectFinder::reset() {
    m_track = Tracker();

    m_trackerList.clear();
    m_frameTracker.clear();

    m_cellNum = 0;
    m_frameNum = 0;
    m_newObject = false;
    m_numObjects = 0;
    m_processedImg = nullptr;
    m_rawImg = nullptr;
}

void ObjectFinder::setImages(const cv::Mat* raw, const cv::Mat* processed) {
    m_rawImg = raw;
    m_processedImg = processed;
}
