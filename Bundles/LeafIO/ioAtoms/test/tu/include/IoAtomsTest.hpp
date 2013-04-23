
#ifndef __IOATOMS_UT_IOATOMSTEST_HPP__
#define __IOATOMS_UT_IOATOMSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ioAtoms
{
namespace ut
{

/**
 * @brief Test basic reader/writer for PatientDB.
 */
class IoAtomsTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IoAtomsTest );
    CPPUNIT_TEST( JSONTest );
    CPPUNIT_TEST( JSONZTest );
    CPPUNIT_TEST( XMLTest );
    CPPUNIT_TEST( XMLZTest );
    CPPUNIT_TEST( HDF5Test );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void JSONTest();
    void JSONZTest();
    void XMLTest();
    void XMLZTest();
    void HDF5Test();



};

} // namespace ut
} // namespace ioAtoms

#endif //__IOBASIC_UT_IOBASICTEST_HPP__
