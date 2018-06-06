#include "objectfinder.h"

/**
 * @brief Main method of objectfinder
 * @return Count of found objects
 */
int ObjectFinder::findObjects(Experiment& experiment, const Setup& setup) {
    if (m_dataFlags != setup.dataFlags) {
        m_dataFlags = setup.dataFlags;
    }
    //
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
                if (m_centroid.x <= experiment.inlet - 5) {
                    m_distThreshold = 5.0;  // Should be a settable variable
                } else {
                    m_distThreshold = 20.0;  // Should be a settable variable
                }

                // Determine whether new or not from threshold
                m_newObject = m_dist > m_distThreshold;
            }
        }

        writeToDataVector(i, experiment);
    }
    m_frameNum++;
    return m_numObjects;
}

/**
 * @brief Removes objects from foundObject-list from Experiment e
 * @param e Experiment
 * @return How many objects that have been removed
 */
unsigned long ObjectFinder::cleanObjects(Experiment& e) {
    unsigned long length = e.data.size();

    // Check if object reached outlet
    e.data.erase(std::remove_if(e.data.begin(), e.data.end(), [&](const auto& dc) -> bool {
        cv::Rect bb_o = (*dc).back()->template getValue<cv::Rect>(data::BoundingBox);
        return ((bb_o.x + bb_o.width) < e.outlet);
    }), e.data.end());
    // Check if object found before inlet
    e.data.erase(std::remove_if(e.data.begin(), e.data.end(), [&](const auto& dc) -> bool  {
        cv::Rect bb_i = (*dc).front()->template getValue<cv::Rect>(data::BoundingBox);
        return ((bb_i.x + bb_i.width) > e.inlet - 1);
    }), e.data.end());
    // Check if object has more than m_countThreshold
    e.data.erase(std::remove_if(e.data.begin(), e.data.end(), [&](const auto& dc) -> bool {
        return (*dc).size() < m_countThreshold;
    }), e.data.end());

    return length - e.data.size();
}

/**
 * @brief
 * @warning NOT REALLY IMPLEMENTED
 * @param experiment
 */
void ObjectFinder::setConditions(const Experiment& experiment) {
    m_conditions.emplace_back(
            [&](const DataContainer* dc) -> bool {
                cv::Rect bb_o = (*dc).back()->template getValue<cv::Rect>(data::BoundingBox);
                return ((bb_o.x + bb_o.width) < experiment.outlet);
            });

    m_conditions.emplace_back(
            [&](const DataContainer* dc) -> bool  {
                cv::Rect bb_i = (*dc).front()->template getValue<cv::Rect>(data::BoundingBox);
                return ((bb_i.x + bb_i.width) > experiment.inlet - 1);
            });

    m_conditions.emplace_back(
            [&](const DataContainer* dc) -> bool {
                return (*dc).size() < m_countThreshold;
            });

}

/**
 * @brief
 * @param listOfObjects
 * @return <distance, Tracker-object>
 */
std::pair<double, Tracker> ObjectFinder::findNearestObject(const cv::Point& object, const std::vector<Tracker>& listOfObjects) {
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
    assert(i < experiment.data.size());
    experiment.data[i]->appendNew();

    // Get some data (should be moved)
    double gradientScore = mathlab::gradientScore(*m_rawImg, m_cc[cc_i]->getValue<cv::Rect>(data::BoundingBox));
    double symmetry = mathlab::verticalSymmetry(*m_rawImg,
                                              m_cc[cc_i]->getValue<cv::Rect>(data::BoundingBox),
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

ObjectFinder::ObjectFinder() {
    m_cc.setDataFlags(data::AllFlags);
}

void ObjectFinder::setImages(const cv::Mat &raw, const cv::Mat &processed) {
    m_rawImg = &raw;
    m_processedImg = &processed;
}

