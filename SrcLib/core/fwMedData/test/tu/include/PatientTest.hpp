/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_UT_PATIENTTEST_HPP__
#define __FWMEDDATA_UT_PATIENTTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedData
{
namespace ut
{

class PatientTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( PatientTest );
CPPUNIT_TEST( nameTest );
CPPUNIT_TEST( idTest );
CPPUNIT_TEST( birthdateTest );
CPPUNIT_TEST( sexTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void nameTest();
    void idTest();
    void birthdateTest();
    void sexTest();

private:
    Patient::sptr m_patient;
};

} //namespace ut
} //namespace fwMedData

#endif //__FWMEDDATA_UT_PATIENTTEST_HPP__

