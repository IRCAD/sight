#ifndef _FWXML_TEST_TU_FWXMLTEST_HPP_
#define _FWXML_TEST_TU_FWXMLTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/PatientDB.hpp>

class FwXMLTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( FwXMLTest );
    CPPUNIT_TEST( testFwXML );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void testFwXML();

private:
    ::fwData::PatientDB::sptr createPatientDB();

};

#endif // _FWXML_TEST_TU_FWXMLTEST_HPP_
