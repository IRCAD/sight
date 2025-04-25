/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "logger_test.hpp"

#include <core/log/logger.hpp>

#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::log::ut::logger_test);

namespace sight::core::log::ut
{

//------------------------------------------------------------------------------

void logger_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void logger_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void logger_test::simple_logger_test()
{
    core::log::logger::sptr logger = std::make_shared<core::log::logger>();

    const std::string info1     = "This is an information message.";
    const std::string info2     = "This is another information message.";
    const std::string info3     = "This is the last information message.";
    const std::string warning1  = "This is a warning message.";
    const std::string warning2  = "This is another warning message.";
    const std::string critical1 = "This is a critical message.";
    const std::string critical2 = "This is another critical message.";
    const std::string critical3 = "This is almost the last critical message.";
    const std::string critical4 = "This is the last critical message.";

    // Fill logger
    logger->information(info1);
    logger->critical(critical1);
    logger->warning(warning1);
    logger->information(info2);
    logger->critical(critical2);
    logger->critical(critical3);
    logger->information(info3);
    logger->warning(warning2);
    logger->critical(critical4);

    // Check number of logs
    std::size_t expected = 9;
    CPPUNIT_ASSERT_EQUAL(expected, logger->count());
    expected = 3;
    CPPUNIT_ASSERT_EQUAL(expected, logger->count(core::log::log::information));
    expected = 2;
    CPPUNIT_ASSERT_EQUAL(expected, logger->count(core::log::log::warning));
    expected = 4;
    CPPUNIT_ASSERT_EQUAL(expected, logger->count(core::log::log::critical));

    // Check message values
    CPPUNIT_ASSERT_EQUAL(info1, logger->get_log(0).get_message());
    CPPUNIT_ASSERT_EQUAL(critical1, logger->get_log(1).get_message());
    CPPUNIT_ASSERT_EQUAL(warning1, logger->get_log(2).get_message());
    CPPUNIT_ASSERT_EQUAL(info2, logger->get_log(3).get_message());
    CPPUNIT_ASSERT_EQUAL(critical2, logger->get_log(4).get_message());
    CPPUNIT_ASSERT_EQUAL(critical3, logger->get_log(5).get_message());
    CPPUNIT_ASSERT_EQUAL(info3, logger->get_log(6).get_message());
    CPPUNIT_ASSERT_EQUAL(warning2, logger->get_log(7).get_message());
    CPPUNIT_ASSERT_EQUAL(critical4, logger->get_log(8).get_message());

    // Check level types
    auto it = logger->begin();
    CPPUNIT_ASSERT_EQUAL(core::log::log::information, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::critical, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::warning, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::information, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::critical, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::critical, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::information, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::warning, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::critical, (it++)->get_level());

    // Sort logs
    logger->sort();

    // Check level types
    it = logger->begin();
    CPPUNIT_ASSERT_EQUAL(core::log::log::critical, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::critical, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::critical, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::critical, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::warning, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::warning, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::information, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::information, (it++)->get_level());
    CPPUNIT_ASSERT_EQUAL(core::log::log::information, (it++)->get_level());

    // Clear logs
    logger->clear();
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), logger->count());
}

//------------------------------------------------------------------------------

} // namespace sight::core::log::ut
