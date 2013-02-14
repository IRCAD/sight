/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATACAMP_UT_SERIESPROPERTIESTEST_HPP__
#define __FWMEDDATACAMP_UT_SERIESPROPERTIESTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedDataCamp
{
namespace ut
{

class SeriesPropertiesTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SeriesPropertiesTest );
    CPPUNIT_TEST( equipmentPropertiesTest );
    CPPUNIT_TEST( imageSeriesPropertiesTest );
    CPPUNIT_TEST( modelSeriesPropertiesTest );
    CPPUNIT_TEST( patientPropertiesTest );
    CPPUNIT_TEST( studyPropertiesTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void equipmentPropertiesTest();
    void imageSeriesPropertiesTest();
    void modelSeriesPropertiesTest();
    void patientPropertiesTest();
    void studyPropertiesTest();
};

} //namespace ut
} //namespace fwMedDataCamp

#endif // __FWMEDDATACAMP_UT_SERIESPROPERTIESTEST_HPP__
