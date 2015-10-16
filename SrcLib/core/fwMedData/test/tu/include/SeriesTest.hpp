/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_UT_SERIESTEST_HPP__
#define __FWMEDDATA_UT_SERIESTEST_HPP__


#include "fwMedData/ImageSeries.hpp"
#include "fwMedData/ModelSeries.hpp"

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedData
{

namespace ut
{

class SeriesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesTest );

CPPUNIT_TEST(attrPatientTest);
CPPUNIT_TEST(attrStudyTest);
CPPUNIT_TEST(attrEquipmentTest);
CPPUNIT_TEST(attrInstanceUIDTest);
CPPUNIT_TEST(attrModalityTest);
CPPUNIT_TEST(attrDateTest);
CPPUNIT_TEST(attrTimeTest);
CPPUNIT_TEST(attrPerformingPhysiciansNameTest);
CPPUNIT_TEST(attrDescriptionTest);

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void attrPatientTest();
    void attrStudyTest();
    void attrEquipmentTest();
    void attrInstanceUIDTest();
    void attrModalityTest();
    void attrDateTest();
    void attrTimeTest();
    void attrPerformingPhysiciansNameTest();
    void attrDescriptionTest();

protected:
    typedef std::vector < SPTR(::fwMedData::Series) > SeriesVectorType;

    SeriesVectorType m_series;

};

} //namespace ut
} //namespace fwMedData

#endif //__FWMEDDATA_UT_SERIESTEST_HPP__

