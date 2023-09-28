/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "date_and_time_test.hpp"

#include <core/tools/date_and_time.hpp>
#include <core/tools/from_iso_extended_string.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::date_and_time_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void date_and_time_test::date_test()
{
    const boost::gregorian::date date(2022, 2, 4);
    const boost::gregorian::date default_date(1900, 1, 1);

    CPPUNIT_ASSERT_EQUAL(date, core::tools::str_to_boost_date("20220204"));
    CPPUNIT_ASSERT_EQUAL(date, core::tools::str_to_boost_date("20220204 Too many characters"));
    CPPUNIT_ASSERT_EQUAL(default_date, core::tools::str_to_boost_date("2022020"));
    CPPUNIT_ASSERT_EQUAL(default_date, core::tools::str_to_boost_date("elephant"));

    CPPUNIT_ASSERT_EQUAL(boost::posix_time::ptime(date), core::tools::from_date_in_xsd_format("2022-02-04"));
}

//------------------------------------------------------------------------------

void date_and_time_test::time_test()
{
    const boost::posix_time::time_duration time(12, 6, 3);
    const boost::posix_time::time_duration default_time(0, 0, 0);

    CPPUNIT_ASSERT_EQUAL(time, core::tools::str_to_boost_time("120603"));
    CPPUNIT_ASSERT_EQUAL(time, core::tools::str_to_boost_time("120603 Too many characters"));
    CPPUNIT_ASSERT_EQUAL(default_time, core::tools::str_to_boost_time("12060"));
    CPPUNIT_ASSERT_EQUAL(default_time, core::tools::str_to_boost_time("rabbit"));
}

//------------------------------------------------------------------------------

void date_and_time_test::date_time_test()
{
    const boost::gregorian::date date(2022, 2, 4);
    const boost::posix_time::time_duration time(12, 6, 3);
    const boost::posix_time::ptime date_and_time(date, time);

    CPPUNIT_ASSERT_EQUAL(date_and_time, core::tools::str_to_boost_date_and_time("20220204", "120603"));
    CPPUNIT_ASSERT_EQUAL(date_and_time, core::tools::from_iso_extended_string("2022-02-04T12:06:03"));
}

//------------------------------------------------------------------------------

void date_and_time_test::to_string_test()
{
    using namespace std::literals::string_literals;

    const boost::gregorian::date date(2022, 2, 4);
    const boost::posix_time::time_duration time(12, 6, 3);
    const boost::posix_time::ptime date_and_time(date, time);

    CPPUNIT_ASSERT_EQUAL("20220204"s, core::tools::get_date(date_and_time));
    CPPUNIT_ASSERT_EQUAL("120603"s, core::tools::get_time(date_and_time));
    CPPUNIT_ASSERT_EQUAL("2022-02-04T12:06:03"s, core::tools::to_iso_extended_string(date_and_time));
    CPPUNIT_ASSERT_EQUAL("2022-02-04"s, core::tools::get_date_in_xsd_format(date_and_time));
}

} // namespace sight::core::tools::ut
