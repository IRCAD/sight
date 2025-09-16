/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/log/logger.hpp>

#include <doctest/doctest.h>

#include <string>

TEST_SUITE("sight::core::log::logger")
{
//-----------------------------------------------------------------------------

    TEST_CASE("simple_logger")
    {
        auto logger = std::make_shared<sight::core::log::logger>();

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
        CHECK_EQ(expected, logger->count());
        expected = 3;
        CHECK_EQ(expected, logger->count(sight::core::log::log::information));
        expected = 2;
        CHECK_EQ(expected, logger->count(sight::core::log::log::warning));
        expected = 4;
        CHECK_EQ(expected, logger->count(sight::core::log::log::critical));

        // Check message values
        CHECK_EQ(info1, logger->get_log(0).get_message());
        CHECK_EQ(critical1, logger->get_log(1).get_message());
        CHECK_EQ(warning1, logger->get_log(2).get_message());
        CHECK_EQ(info2, logger->get_log(3).get_message());
        CHECK_EQ(critical2, logger->get_log(4).get_message());
        CHECK_EQ(critical3, logger->get_log(5).get_message());
        CHECK_EQ(info3, logger->get_log(6).get_message());
        CHECK_EQ(warning2, logger->get_log(7).get_message());
        CHECK_EQ(critical4, logger->get_log(8).get_message());

        // Check level types
        auto it = logger->begin();
        CHECK_EQ(sight::core::log::log::information, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::critical, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::warning, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::information, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::critical, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::critical, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::information, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::warning, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::critical, (it++)->get_level());

        // Sort logs
        logger->sort();

        // Check level types
        it = logger->begin();
        CHECK_EQ(sight::core::log::log::critical, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::critical, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::critical, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::critical, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::warning, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::warning, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::information, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::information, (it++)->get_level());
        CHECK_EQ(sight::core::log::log::information, (it++)->get_level());

        // Clear logs
        logger->clear();
        CHECK_EQ(std::size_t(0), logger->count());
    }
} // TEST_SUITE
