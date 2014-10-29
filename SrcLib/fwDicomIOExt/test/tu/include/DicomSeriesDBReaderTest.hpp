/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_TEST_TU_DICOMSERIESDBREADERTEST_HPP__
#define __FWDICOMIOEXT_TEST_TU_DICOMSERIESDBREADERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

namespace fwMedData
{
    class SeriesDB;
}

namespace fwDicomIOExt
{
namespace ut
{

class DicomSeriesDBReaderTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DicomSeriesDBReaderTest );
    CPPUNIT_TEST( readDicomSeriesDBGDCMTest );
    CPPUNIT_TEST( readDicomSeriesDBDCMTKTest );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void readDicomSeriesDBGDCMTest();
    void readDicomSeriesDBDCMTKTest();

private:
    ::fwMedData::SeriesDB::sptr m_seriesDB;

};

} // namespace ut
} // namespace fwDicomIOExt

#endif // __FWDICOMIOEXT_TEST_TU_DICOMSERIESDBREADERTEST_HPP__

