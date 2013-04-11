
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
    CPPUNIT_TEST( JSONMedicalDataTest );
    CPPUNIT_TEST( JSONZMedicalDataTest );
    CPPUNIT_TEST( XMLMedicalDataTest );
    CPPUNIT_TEST( XMLZMedicalDataTest );
    CPPUNIT_TEST( HDF5MedicalDataTest );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void writeReadMedicalDataTest();

    void JSONMedicalDataTest();
    void JSONZMedicalDataTest();
    void XMLMedicalDataTest();
    void XMLZMedicalDataTest();
    void HDF5MedicalDataTest();

    // Helper to test different extensions
    void writeReadFile( const ::boost::filesystem::path & filePath );
};

} // namespace ut
} // namespace ioAtoms

#endif //__IOBASIC_UT_IOBASICTEST_HPP__
