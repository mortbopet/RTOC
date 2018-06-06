#include "catch.hpp"

#include "../lib/process.h"

#include <boost/serialization/unique_ptr.hpp>

#include <boost/serialization/vector.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <fstream>

/*
namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& ar, std::vector<unique_ptr<int>>& g, const unsigned int version) {
    ar& g;
}

}  // namespace serialization
}  // namespace boost

TEST_CASE("Process serialization", "[process]") {
    // Instantiate all available processes
    std::vector<std::unique_ptr<int>> v_out;
    const std::string archName = "test_arch.xml";

    v_out.push_back(make_unique<int>(1));
    v_out.push_back(make_unique<int>(2));
    v_out.push_back(make_unique<int>(3));

    std::ofstream ofs(archName);

    {
        boost::archive::xml_oarchive oa(ofs);
        // oa << BOOST_SERIALIZATION_NVP(v_out);
    }
    ofs.close();

    std::vector<std::unique_ptr<int>> v_in;

    std::ifstream ifs(archName);
    {
        boost::archive::xml_iarchive ia(ifs);
#ifndef __linux__  //
https://stackoverflow.com/questions/50038329/serializing-stdvector-of-unique-ptr-using-boostserialization-fails-on-linux
        ia >> BOOST_SERIALIZATION_NVP(v_in);
#endif
    }
    ifs.close();

    remove(archName.c_str());
}

*/
