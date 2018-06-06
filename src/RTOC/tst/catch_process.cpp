#include "catch.hpp"

#include "../lib/process.h"

#include <boost/serialization/unique_ptr.hpp>

#include <boost/serialization/vector.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <fstream>

TEST_CASE("Process serialization", "[process]") {
    // Test serialization of processes through deduction of the underlying process (a vector of
    // ProcessBase* are serialized)

    // Instantiate all available processes
    std::vector<std::unique_ptr<ProcessBase>> v_out;
    v_out.push_back(std::make_unique<Morph>());
    v_out.push_back(std::make_unique<Binarize>());
    v_out.push_back(std::make_unique<Normalize>());
    v_out.push_back(std::make_unique<SubtractBG>());
    v_out.push_back(std::make_unique<ClearBorder>());
    v_out.push_back(std::make_unique<FloodFillProcess>());
    v_out.push_back(std::make_unique<PropFilter>());
    v_out.push_back(std::make_unique<Canny>());

    const std::string archName = "test_arch.xml";

    std::ofstream ofs(archName);

    // Test archive saving
    {
        boost::archive::xml_oarchive oa(ofs);
        // write class instance to archive
        oa << BOOST_SERIALIZATION_NVP(v_out);
    }
    ofs.close();

    // Test archive loading
    std::vector<std::unique_ptr<ProcessBase>> v_in;

    std::ifstream ifs(archName);
    {
        boost::archive::xml_iarchive ia(ifs);
        // Read all classes from process archive
        // ia >> BOOST_SERIALIZATION_NVP(v_in);
    }
    ifs.close();

    // cleanup
    remove(archName.c_str());
}
/// INSERT TEST CASES HERE
