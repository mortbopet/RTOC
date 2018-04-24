#include "objectfinder.h"

/**
 * @brief
 * @return
 */
int ObjectFinder::findObjects(Experiment& experiment) {
    bool newcell;
    DataContainer dc(0xffff);
    int numObjects = matlab::regionProps(m_frame.image, 0xffff, connectedComponents);

    for (int i = 0; i < numObjects; i++) {
        if (m_cellNum <= 0) {
            newcell = true;
        } else {
            Tracker term(m_frameNum - 1);

            m_frameTracker = matlab::find<Tracker>(m_trackerList, term);

            if (m_frameTracker.empty()) {
                newcell = true;
            } else {
                m_centroid = dc[i]->getValue<cv::Point>(data::Centroid)
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
                newcell = m_dist > m_distThreshold;
            }
        }

        writeToDataVector(newcell, i, experiment);
        m_cellNum++;
    }


    m_frameNum++;

    return numObjects;
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
    return (p.first, listOfObjects.at(p.second));
}

/**
 * @brief
 * @param image
 */
void ObjectFinder::setFrame(const cv::Mat& image) {
    m_frame = {image,"",m_frameNum,true};
}

void ObjectFinder::writeToDataVector(const bool& newcell, const int& i, Experiment& experiment) {
    if (newcell) {

        experiment.data.emplace_back(new DataContainer(0xffff));
        experiment.data[m_cellNum]->appendNew();

        (*experiment.data[m_cellNum])[0]->setValue(data::Inlet, experiment.inlet);
        (*experiment.data[m_cellNum])[0]->setValue(data::Outlet, experiment.outlet);
        // yref ???
        (*experiment.data[m_cellNum])[0]->setValue(data::Label, m_cellNum);

        (*experiment.data[m_cellNum])[0]->setValue(data::Frame, m_frameNum);
        (*experiment.data[m_cellNum])[0]->setValue(
                data::Centroid, connectedComponents[i]->getValue<cv::Point>(data::Centroid));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::BoundingBox, connectedComponents[i]->getValue<cv::Rect>(data::BoundingBox));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::Major_axis, connectedComponents[i]->getValue<double>(data::Major_axis));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::Eccentricity, connectedComponents[i]->getValue<double>(data::Eccentricity));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::Circularity, connectedComponents[i]->getValue<double>(data::Circularity));
        (*experiment.data[m_cellNum])[0]->setValue(data::Symmetry,
                                                   connectedComponents[i]->getValue<double>(data::Symmetry));
        (*experiment.data[m_cellNum])[0]->setValue(
                data::GradientScore, connectedComponents[i]->getValue<double>(data::GradientScore));

        m_track.cell_no = m_cellNum++;

    } else {

        experiment.data[m_track.cell_no]->appendNew();
        auto index = experiment.data[m_track.cell_no]->size() - 1;

        (*experiment.data[m_track.cell_no])[index]->setValue(data::Frame, m_frameNum);
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Centroid, connectedComponents[i]->getValue<cv::Point>(data::Centroid));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::BoundingBox, connectedComponents[i]->getValue<cv::Rect>(data::BoundingBox));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Major_axis, connectedComponents[i]->getValue<double>(data::Major_axis));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Eccentricity, connectedComponents[i]->getValue<double>(data::Eccentricity));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Circularity, connectedComponents[i]->getValue<double>(data::Circularity));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::Symmetry, connectedComponents[i]->getValue<double>(data::Symmetry));
        (*experiment.data[m_track.cell_no])[index]->setValue(
                data::GradientScore, connectedComponents[i]->getValue<double>(data::GradientScore));
    }
    m_track.frame_no = m_frameNum;
    m_track.centroid = connectedComponents[i]->getValue<cv::Point>(data::Centroid);
    m_trackerList.push_back(m_track);
}
