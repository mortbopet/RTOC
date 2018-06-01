#ifndef EXPERIMENTSETUP_H
#define EXPERIMENTSETUP_H

#include <QWidget>

#include "../lib/analyzer.h"
#include "acquisitioninterface.h"

#include "boost/serialization/nvp.hpp"
#include "boost/serialization/serialization.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "guihelpers.h"

namespace Ui {
class ExperimentSetup;
}

class ExperimentSetup : public QWidget {
    Q_OBJECT

public:
    explicit ExperimentSetup(Analyzer* analyzer, AcquisitionInterface* iface, QWidget* parent = 0);
    explicit ExperimentSetup() {
        /*Default constructor required for serialization but should never be used*/
        Q_ASSERT(false);
    }

    ~ExperimentSetup();
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) const;

private slots:
    void on_run_clicked();

    void on_setExperimentPath_clicked();
    bool verifyCanRunExperiment();
    void updateCurrentSetup();
    void connectWidgets();

private:
    void setToolTips();
    void setupDataOptions();
    Ui::ExperimentSetup* ui;

    Analyzer* m_analyzer;
    AcquisitionInterface* m_interface;
    QString m_experimentPath;

    Setup m_currentSetup;

    // Functions related to running the experiment
    bool setupExperimentDirectory() const;
};

#endif  // EXPERIMENTSETUP_H
