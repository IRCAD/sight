/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWVTKGDCMIO_TEST_TU_DICOMTEST_HPP_
#define _FWVTKGDCMIO_TEST_TU_DICOMTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/PatientDB.hpp>

class DicomTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DicomTest );
    CPPUNIT_TEST( methode1 );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void methode1();

private:
    ::fwData::PatientDB::sptr createPatientDB();

};

#endif // _FWVTKGDCMIO_TEST_TU_DICOMTEST_HPP_
