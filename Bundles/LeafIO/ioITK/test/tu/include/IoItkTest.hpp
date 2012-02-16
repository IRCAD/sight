
#ifndef _IOITK_TEST_TU_IOITKTEST_HPP_
#define _IOITK_TEST_TU_IOITKTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Patient.hpp>

/**
 * @brief Test many methods to create mesh.
 */
class IoItkTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IoItkTest );
    CPPUNIT_TEST( testImageWriterJPG );
    CPPUNIT_TEST( testPatientDBReaderJPG );
    CPPUNIT_TEST( testSaveLoadInr );
    CPPUNIT_TEST( testLoadInr );
//    CPPUNIT_TEST( testGeneratedPatient );
//    CPPUNIT_TEST( testLoadingPatientDB );
//    CPPUNIT_TEST( testLoadingMfoDB );
//    CPPUNIT_TEST( testCompareMfoAndMfoDB );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void testImageWriterJPG();
    void testPatientDBReaderJPG();
    void testSaveLoadInr();
    void testLoadInr();

//    void testLoadingPatientDB();
//    void testLoadingMfoDB();
//    void testCompareMfoAndMfoDB();

private :

    void executeService( ::fwTools::Object::sptr obj, std::string srvType, std::string srvImpl, ::fwRuntime::EConfigurationElement::sptr cfg );

};

#endif //_IOITK_TEST_TU_IOITKTEST_HPP_
