/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include <io/dimse/SeriesEnquirer.hpp>
#include <io/dimse/SeriesRetriever.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <filesystem>

namespace sight::io::dimse::ut
{

class series_enquirer_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(series_enquirer_test);
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
    void setUp() override;
    void tearDown() override;

    // Test functions
    void initialize_connection();
    void pull_series_using_move_retrieve_method();
    void pull_series_using_get_retrieve_method();
    void pull_instance_using_move_retrieve_method();
    void pull_instance_using_get_retrieve_method();
    void push_series();

protected:

    io::dimse::series_enquirer::sptr m_series_enquirer;
    io::dimse::series_retriever::sptr m_series_retriever;

    /// Local application title
    std::string m_local_application_title;

    /// Pacs host name
    std::string m_pacs_host_name;

    /// Pacs application title
    std::string m_pacs_application_title;

    /// Pacs application port
    std::uint16_t m_pacs_application_port {};

    /// Move application title
    std::string m_move_application_title;

    /// Move application port
    std::uint16_t m_move_application_port {};
};

} // namespace sight::io::dimse::ut
