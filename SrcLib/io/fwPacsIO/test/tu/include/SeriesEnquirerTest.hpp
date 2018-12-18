/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

