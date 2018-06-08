#ifndef GUIHELPERS_H
#define GUIHELPERS_H

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>

#include "boost/serialization/nvp.hpp"
#include "boost/serialization/serialization.hpp"
#include "boost/serialization/split_free.hpp"

int GetIndexOfItem(const QComboBox& box, const QVariant& item);

// To have template definitions of widget serializers in .cpp file, we need to explicitely
// instantiate for the required archives (in our case, xml archives)
#define EXPLICIT_INSTANTIATE_CONST_XML_ARCHIVE(classname)             \
    template void classname::serialize<boost::archive::xml_iarchive>( \
        boost::archive::xml_iarchive&, const unsigned int) const;     \
    template void classname::serialize<boost::archive::xml_oarchive>( \
        boost::archive::xml_oarchive&, const unsigned int) const;

#define EXPLICIT_INSTANTIATE_XML_ARCHIVE(classname)                   \
    template void classname::serialize<boost::archive::xml_iarchive>( \
        boost::archive::xml_iarchive&, const unsigned int);           \
    template void classname::serialize<boost::archive::xml_oarchive>( \
        boost::archive::xml_oarchive&, const unsigned int);

// Serialization macros for various GUI types to load/store gui state
#define SERIALIZE_CHECKBOX(ar, o, name) \
    auto name = o->isChecked();         \
    ar& BOOST_SERIALIZATION_NVP(name);  \
    o->setChecked(name);

#define SERIALIZE_COMBOBOX(ar, o, name) \
    auto name = o->currentText();       \
    ar& BOOST_SERIALIZATION_NVP(name);  \
    o->setCurrentIndex(o->findText(name));

#define SERIALIZE_LINEEDIT(ar, o, name) \
    auto name = o->text();              \
    ar& BOOST_SERIALIZATION_NVP(name);  \
    o->setText(name);

#define SERIALIZE_SPINBOX(ar, o, name) \
    auto name = o->value();            \
    ar& BOOST_SERIALIZATION_NVP(name); \
    o->setValue(name);

namespace boost {
namespace serialization {

template <class Archive>
inline void save(Archive& ar, const QString& s, const unsigned int) {
    using boost::serialization::make_nvp;
    auto _s = s.toStdString();
    ar << make_nvp("value", _s);
}

template <class Archive>
inline void load(Archive& ar, QString& so, const unsigned int) {
    using boost::serialization::make_nvp;
    std::string s;
    ar >> make_nvp("value", s);
    so = QString::fromStdString(s);
}

template <class Archive>
inline void serialize(Archive& ar, QString& s, const unsigned int file_version) {
    boost::serialization::split_free(ar, s, file_version);
}

template <class Archive, typename T1, typename T2>
inline void serialize(Archive& ar, std::pair<T1, T2>& s, const unsigned int file_version) {
    ar& make_nvp("first", s.first);
    ar& make_nvp("second", s.second);
}

}  // namespace serialization
}  // namespace boost

#endif  // GUIHELPERS_H
