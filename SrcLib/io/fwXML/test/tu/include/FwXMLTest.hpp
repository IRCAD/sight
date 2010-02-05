/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
