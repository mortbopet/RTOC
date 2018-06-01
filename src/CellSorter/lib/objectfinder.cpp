#include "objectfinder.h"

/**
 * @brief
 * @return
 */
int ObjectFinder::findObjects(Experiment& experiment) {
    bool newObject;
    int numObjects = matlab::regionProps(m_frame.image, 0xffff, m_connectedComponents);

    for (int i = 0; i < numObjects; i++) {
        if (m_cellNum <= 0) {
            newObject = true;
        } else {
            Tracker term(m_frameNum - 1);

            m_frameTracker = matlab::find<Tracker>(m_trackerList, term);

            if (m_frameTracker.empty()) {
                newObject = true;
            } else {
                m_centroid = m_connectedComponents[i]->getValue<cv::Point>(data::Centroid);
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
                newObject = m_dist > m_distThreshold;
            }
        }

        writeToDataVector(newObject, i, experiment);
        if (newObject) {
            m_cellNum++;
        }
    }
    m_frameNum++;
    return numObjects;
}

unsigned long ObjectFinder::cleanObjects(Experiment& e) {
    unsigned long length = e.data.size();

    e.data.erase(std::remove_if(e.data.begin(), e.data.end(), [&](const auto& dc) -> bool {
        cv::Rect bb_o = (*dc).back()->template getValue<cv::Rect>(data::BoundingBox);
        return ((bb_o.x + bb_o.width) < e.outlet);
    }), e.data.end());
    e.data.erase(std::remove_if(e.data.begin(), e.data.end(), [&](const auto& dc) -> bool  {
        cv::Rect bb_i = (*dc).front()->template getValue<cv::Rect>(data::BoundingBox);
        return ((bb_i.x + bb_i.width) > e.inlet - 1);
    }), e.data.end());
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
        auto dist = matlab::dist(object, cc.centroid);
        if (dist < -10)
            dist = 100;
        d.push_back(dist);
    }
    std::pair<double, unsigned long> p = matlab::min<double>(d);
    return {p.first, listOfObjects.at(p.second)};
}

/**
 * @brief
 * @param image
 */
void ObjectFinder::setFrame(const cv::Mat& image) {
    m_frame = {image,"",m_frameNum,true};
}

void ObjectFinder::writeToDataVector(const bool& newObject, const int& cc_number, Experiment& experiment) {
    if (newObject) {

        experiment.data.emplace_back(new DataContainer(0xffff));
        experiment.data[m_cellNum]->appendNew();

        (*experiment.data[m_cellNum])[0]->setValue(data::Inlet, experiment.inlet);
        (*experiment.data[m_cellNum])[0]->setValue(data::Outlet, experiment.outlet);
        (*experiment.data[m_cellNum])[0]->setValue(data::Label, m_cellNum);

        (*experiment.data[m_cellNum])[0]->setValue(data::Frame, m_frameNum);
        (*experiment.data[m_cellNum])[0]->setValue(
                data::Centroid, m_connectedComponents[cc_number]->getValue<cv::Point>(data::Centroid));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::BoundingBox, m_connectedComponents[cc_number]->getValue<cv::Rect>(data::BoundingBox));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::Major_axis, m_connectedComponents[cc_number]->getValue<double>(data::Major_axis));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::Eccentricity, m_connectedComponents[cc_number]->getValue<double>(data::Eccentricity));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::Circularity, m_connectedComponents[cc_number]->getValue<double>(data::Circularity));
        (*experiment.data[m_cellNum])[0]->setValue(data::Symmetry,
                                                   m_connectedComponents[cc_number]->getValue<double>(data::Symmetry));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::GradientScore, m_connectedComponents[cc_number]->getValue<double>(data::GradientScore));

        m_track.cell_no = m_cellNum;

    } else {

        experiment.data[m_track.cell_no]->appendNew();
        auto index = experiment.data[m_track.cell_no]->size() - 1;

        (*experiment.data[m_track.cell_no])[index]->setValue(data::Frame, m_frameNum);
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Centroid, m_connectedComponents[cc_number]->getValue<cv::Point>(data::Centroid));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::BoundingBox, m_connectedComponents[cc_number]->getValue<cv::Rect>(data::BoundingBox));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Major_axis, m_connectedComponents[cc_number]->getValue<double>(data::Major_axis));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Eccentricity, m_connectedComponents[cc_number]->getValue<double>(data::Eccentricity));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Circularity, m_connectedComponents[cc_number]->getValue<double>(data::Circularity));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Symmetry, m_connectedComponents[cc_number]->getValue<double>(data::Symmetry));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::GradientScore, m_connectedComponents[cc_number]->getValue<double>(data::GradientScore));
    }
    m_track.frame_no = m_frameNum;
    m_track.centroid = m_connectedComponents[cc_number]->getValue<cv::Point>(data::Centroid);
    m_trackerList.push_back(m_track);
}
