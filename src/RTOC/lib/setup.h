#ifndef RTOC_SETUP_H
#define RTOC_SETUP_H

#include <string>

#include <utility>

#include <boost/serialization/serialization.hpp>
#include "boost/serialization/nvp.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

class Setup {
public:
    Setup() {}
    bool runProcessing;
    bool extractData;
    bool classifyObjects = false; // eg use machinelearning module
    bool storeRaw;
    bool storeProcessed;
    int countThreshold;
    double distanceThresholdInlet;
    double distanceThresholdPath;
    unsigned long dataFlags = 0;
    unsigned long conditionFlags = 0;
    unsigned int recordingTime = 0;  // in ms
    std::pair<int, int> inlet;
    std::pair<int, int> outlet;
    std::string rawPrefix;
    std::string processedPrefix;
    std::string outputPath;
    std::string modelPath;
    std::string experimentName;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& BOOST_SERIALIZATION_NVP(runProcessing);
        ar& BOOST_SERIALIZATION_NVP(extractData);
        ar& BOOST_SERIALIZATION_NVP(storeRaw);
        ar& BOOST_SERIALIZATION_NVP(storeProcessed);
        ar& BOOST_SERIALIZATION_NVP(dataFlags);
        ar& BOOST_SERIALIZATION_NVP(conditionFlags);
        ar& BOOST_SERIALIZATION_NVP(countThreshold);
        ar& BOOST_SERIALIZATION_NVP(distanceThresholdInlet);
        ar& BOOST_SERIALIZATION_NVP(distanceThresholdPath);

        ar& BOOST_SERIALIZATION_NVP(inlet);
        ar& BOOST_SERIALIZATION_NVP(outlet);

        ar& BOOST_SERIALIZATION_NVP(rawPrefix);
        ar& BOOST_SERIALIZATION_NVP(processedPrefix);
        ar& BOOST_SERIALIZATION_NVP(outputPath);
        ar& BOOST_SERIALIZATION_NVP(experimentName);
    }
};

#endif  // RTOC_SETUP_H
