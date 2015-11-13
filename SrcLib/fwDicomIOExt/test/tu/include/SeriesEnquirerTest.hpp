/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_UT_SERIESENQUIRERTEST_HPP__
#define __FWDICOMIOEXT_UT_SERIESENQUIRERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwDicomIOExt/dcmtk/SeriesEnquirer.hpp>
#include <fwDicomIOExt/dcmtk/SeriesRetriever.hpp>

#include <boost/filesystem.hpp>

namespace fwDicomIOExt
{
namespace ut
{

class SeriesEnquirerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesEnquirerTest );
CPPUNIT_TEST( initializeConnection );
CPPUNIT_TEST( pullSeriesUsingMoveRetrieveMethod );
CPPUNIT_TEST( pullSeriesUsingGetRetrieveMethod );
CPPUNIT_TEST( pullInstanceUsingMoveRetrieveMethod );
CPPUNIT_TEST( pullInstanceUsingGetRetrieveMethod );
CPPUNIT_TEST_SUITE_END();

public:

    // Interface
    void setUp();
    void tearDown();

    // Test functions
    void initializeConnection();
    void pullSeriesUsingMoveRetrieveMethod();
    void pullSeriesUsingGetRetrieveMethod();
    void pullInstanceUsingMoveRetrieveMethod();
    void pullInstanceUsingGetRetrieveMethod();


protected:
    ::fwDicomIOExt::dcmtk::SeriesEnquirer::sptr m_seriesEnquirer;
    ::fwDicomIOExt::dcmtk::SeriesRetriever::sptr m_seriesRetriever;
    std::string m_moveApplicationTitle;

};

} // namespace ut
} // namespace fwDicomIOExt

#endif // __FWDICOMIOEXT_UT_SERIESENQUIRERTEST_HPP__

