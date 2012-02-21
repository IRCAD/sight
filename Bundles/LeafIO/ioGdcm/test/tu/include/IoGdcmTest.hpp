
#ifndef _IOITK_TEST_TU_IOITKTEST_HPP_
#define _IOITK_TEST_TU_IOITKTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Patient.hpp>

/**
 * @brief Test many methods to create mesh.
 */
class IoGdcmTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IoGdcmTest );
    CPPUNIT_TEST( testPatientDBReaderDicom );
    CPPUNIT_TEST( testDicomPatientWriter );
    CPPUNIT_TEST( testDicomPatientDBWriter );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void testImageWriterJPG();
    void testPatientDBReaderJPG();
    void testSaveLoadInr();
    void testLoadInr();
    void testPatientDBReaderDicom();

    void testDicomImageWriter();
    void testDicomPatientWriter();
    void testDicomPatientDBWriter();

private :

    void executeService( ::fwTools::Object::sptr obj, std::string srvType, std::string srvImpl, ::fwRuntime::EConfigurationElement::sptr cfg );

};

#endif //_IOITK_TEST_TU_IOITKTEST_HPP_
