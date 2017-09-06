/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTKGDCM_UT_IOVTKGDCMTEST_HPP__
#define __IOVTKGDCM_UT_IOVTKGDCMTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ioVtkGdcm
{
namespace ut
{

/**
 * @brief Test the reading of Dicom file with vtkGdcm.
 */
class IoVtkGdcmTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( IoVtkGdcmTest );
CPPUNIT_TEST( seriesDBReaderTest );
CPPUNIT_TEST( seriesDBLazyReaderTest );
CPPUNIT_TEST( imageSeriesWriterTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void seriesDBReaderTest();
    void seriesDBLazyReaderTest();
    void imageSeriesWriterTest();

    void readerDicomTest( std::string srvImpl );

    // HACK: force link with fwMedDataCamp. Needed when calling ::fwDataCamp::visitor::CompareObjects::compare.
    int m_medDataCampVersion;
};

} //namespace ut
} //namespace ioVtkGdcm

#endif //__IOVTKGDCM_UT_IOVTKGDCMTEST_HPP__
