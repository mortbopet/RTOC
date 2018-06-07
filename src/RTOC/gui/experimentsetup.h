#ifndef EXPERIMENTSETUP_H
#define EXPERIMENTSETUP_H

#include <QWidget>

#include "../lib/analyzer.h"
#include "../lib/setup.h"
#include "acquisitioninterface.h"

#include "boost/serialization/nvp.hpp"
#include "boost/serialization/serialization.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "guihelpers.h"

enum class ExperimentTypes {
    Acquisition,
    AcquisitionAndProcessing,
    AcquisitionAndExtraction,
    AcquisitionAndRealTimeID
};

static QMap<ExperimentTypes, QString> etDescriptors{
    {ExperimentTypes::Acquisition, "Acquisition only"},
    {ExperimentTypes::AcquisitionAndProcessing, "Acquisition and image processing"},
    {ExperimentTypes::AcquisitionAndExtraction, "Acquisition and extraction"},
    {ExperimentTypes::AcquisitionAndRealTimeID, "Acquisition and realtime identification"}};

Q_DECLARE_METATYPE(ExperimentTypes)

namespace Ui {
class ExperimentSetup;
}

class ExperimentSetup : public QWidget {
    Q_OBJECT

public:
    explicit ExperimentSetup(Analyzer* analyzer, AcquisitionInterface* iface, QWidget* parent = 0);

    ~ExperimentSetup();
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) const;

public slots:
    void updateCurrentSetup();

private slots:
    void on_run_clicked();

    void on_setExperimentPath_clicked();
    bool verifyCanRunExperiment();
    void connectWidgets();

    void on_experimentType_currentIndexChanged(int index);

private:
    void setToolTips();
    void setupDataOptions();
    void setAllDataFlagsState(bool state);
    Ui::ExperimentSetup* ui;

    Analyzer* m_analyzer;
    AcquisitionInterface* m_interface;
    QString m_experimentPath;

    Setup m_currentSetup;

    // Functions related to running the experiment
    bool setupExperimentDirectory() const;
};

#endif  // EXPERIMENTSETUP_H
