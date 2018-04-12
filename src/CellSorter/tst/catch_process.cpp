#include "catch.hpp"

#include "../lib/process.h"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <fstream>

TEST_CASE("Process serialization", "[process]") {
    Morph m;
    Binarize b;

    std::vector<ProcessBase*> v;
    v.push_back(&m);
    v.push_back(&b);

    std::ofstream ofs("test_arch.xml");

    // save data to archive
    {
        boost::archive::xml_oarchive oa(ofs);
        // write class instance to archive
        oa << BOOST_SERIALIZATION_NVP(m);
        oa << BOOST_SERIALIZATION_NVP(b);
        // archive and stream closed when destructors are called
    }
    ofs.close();
}
/// INSERT TEST CASES HERE
