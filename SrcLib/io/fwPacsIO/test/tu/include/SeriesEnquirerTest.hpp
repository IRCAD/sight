/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPACSIO_UT_SERIESENQUIRERTEST_HPP__
#define __FWPACSIO_UT_SERIESENQUIRERTEST_HPP__

#include <fwPacsIO/SeriesEnquirer.hpp>
#include <fwPacsIO/SeriesRetriever.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <boost/filesystem.hpp>

namespace fwPacsIO
{
namespace ut
{

class SeriesEnquirerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesEnquirerTest );
// Disabled until PACS available
// CPPUNIT_TEST( initializeConnection );
// CPPUNIT_TEST( pushSeries );
// CPPUNIT_TEST( pullSeriesUsingMoveRetrieveMethod );
// CPPUNIT_TEST( pullSeriesUsingGetRetrieveMethod );
// CPPUNIT_TEST( pullInstanceUsingMoveRetrieveMethod );
// CPPUNIT_TEST( pullInstanceUsingGetRetrieveMethod );
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
    void pushSeries();

protected:
    ::fwPacsIO::SeriesEnquirer::sptr m_seriesEnquirer;
    ::fwPacsIO::SeriesRetriever::sptr m_seriesRetriever;

    /// Local application title
    std::string m_localApplicationTitle;

    /// Pacs host name
    std::string m_pacsHostName;

    /// Pacs application title
    std::string m_pacsApplicationTitle;

    /// Pacs application port
    unsigned short m_pacsApplicationPort;

    /// Move application title
    std::string m_moveApplicationTitle;

    /// Move application port
    unsigned short m_moveApplicationPort;

};

} // namespace ut
} // namespace fwPacsIO

#endif // __FWPACSIO_UT_SERIESENQUIRERTEST_HPP__

