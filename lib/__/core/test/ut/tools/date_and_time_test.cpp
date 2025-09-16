/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include <core/tools/date_and_time.hpp>
#include <core/tools/from_iso_extended_string.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::tools::date_and_time")
{
//------------------------------------------------------------------------------

    TEST_CASE("date")
    {
        const boost::gregorian::date date(2022, 2, 4);
        const boost::gregorian::date default_date(1900, 1, 1);

        CHECK_EQ(date, sight::core::tools::str_to_boost_date("20220204"));
        CHECK_EQ(date, sight::core::tools::str_to_boost_date("20220204 Too many characters"));
        CHECK_EQ(default_date, sight::core::tools::str_to_boost_date("2022020"));
        CHECK_EQ(default_date, sight::core::tools::str_to_boost_date("elephant"));

        CHECK_EQ(boost::posix_time::ptime(date), sight::core::tools::from_date_in_xsd_format("2022-02-04"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("time")
    {
        const boost::posix_time::time_duration time(12, 6, 3);
        const boost::posix_time::time_duration default_time(0, 0, 0);

        CHECK_EQ(time, sight::core::tools::str_to_boost_time("120603"));
        CHECK_EQ(time, sight::core::tools::str_to_boost_time("120603 Too many characters"));
        CHECK_EQ(default_time, sight::core::tools::str_to_boost_time("12060"));
        CHECK_EQ(default_time, sight::core::tools::str_to_boost_time("rabbit"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("date_time")
    {
        const boost::gregorian::date date(2022, 2, 4);
        const boost::posix_time::time_duration time(12, 6, 3);
        const boost::posix_time::ptime date_and_time(date, time);

        CHECK_EQ(date_and_time, sight::core::tools::str_to_boost_date_and_time("20220204", "120603"));
        CHECK_EQ(date_and_time, sight::core::tools::from_iso_extended_string("2022-02-04T12:06:03"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("to_string")
    {
        using namespace std::literals::string_literals;

        const boost::gregorian::date date(2022, 2, 4);
        const boost::posix_time::time_duration time(12, 6, 3);
        const boost::posix_time::ptime date_and_time(date, time);

        CHECK_EQ("20220204"s, sight::core::tools::get_date(date_and_time));
        CHECK_EQ("120603"s, sight::core::tools::get_time(date_and_time));
        CHECK_EQ("2022-02-04T12:06:03"s, sight::core::tools::to_iso_extended_string(date_and_time));
        CHECK_EQ("2022-02-04"s, sight::core::tools::get_date_in_xsd_format(date_and_time));
    }
} // TEST_SUITE
