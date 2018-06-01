#ifndef GUIHELPERS_H
#define GUIHELPERS_H

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>

#include "boost/serialization/nvp.hpp"
#include "boost/serialization/serialization.hpp"

// To have template definitions of widget serializers in .cpp file, we need to explicitely
// instantiate for the required archives (in our case, xml archives)
#define EXPLICIT_INSTANTIATE_XML_ARCHIVE(classname)                   \
    template void classname::serialize<boost::archive::xml_iarchive>( \
        boost::archive::xml_iarchive&, const unsigned int) const;     \
    template void classname::serialize<boost::archive::xml_oarchive>( \
        boost::archive::xml_oarchive&, const unsigned int) const;

int GetIndexOfItem(const QComboBox& box, const QVariant& item);

// Serialization operators for various GUI types to load/store gui state
namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& ar, QCheckBox& o, const unsigned int version) {
    auto v = o.isChecked();
    ar& BOOST_SERIALIZATION_NVP(v);
    o.setChecked(v);
}

template <class Archive>
void serialize(Archive& ar, QComboBox& o, const unsigned int version) {
    auto v = o.currentText().toStdString();
    ar& BOOST_SERIALIZATION_NVP(v);
    o.setCurrentIndex(o.findText(QString::fromStdString(v)));
}

template <class Archive>
void serialize(Archive& ar, QGroupBox& o, const unsigned int version) {
    auto v = o.isChecked();
    ar& BOOST_SERIALIZATION_NVP(v);
    o.setChecked(v);
}

template <class Archive>
void serialize(Archive& ar, QLineEdit& o, const unsigned int version) {
    auto v = o.text().toStdString();
    ar& BOOST_SERIALIZATION_NVP(v);
    o.setText(QString::fromStdString(v));
}

}  // namespace serialization
}  // namespace boost

#endif  // GUIHELPERS_H
