/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "DateAndTimeTest.hpp"

#include <core/tools/dateAndTime.hpp>
#include <core/tools/fromIsoExtendedString.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::DateAndTimeTest);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void DateAndTimeTest::dateTest()
{
    const boost::gregorian::date date(2022, 2, 4);
    const boost::gregorian::date defaultDate(1900, 1, 1);

    CPPUNIT_ASSERT_EQUAL(date, core::tools::strToBoostDate("20220204"));
    CPPUNIT_ASSERT_EQUAL(date, core::tools::strToBoostDate("20220204 Too many characters"));
    CPPUNIT_ASSERT_EQUAL(defaultDate, core::tools::strToBoostDate("2022020"));
    CPPUNIT_ASSERT_EQUAL(defaultDate, core::tools::strToBoostDate("elephant"));

    CPPUNIT_ASSERT_EQUAL(boost::posix_time::ptime(date), core::tools::fromDateInXsdFormat("2022-02-04"));
}

//------------------------------------------------------------------------------

void DateAndTimeTest::timeTest()
{
    const boost::posix_time::time_duration time(12, 6, 3);
    const boost::posix_time::time_duration defaultTime(0, 0, 0);

    CPPUNIT_ASSERT_EQUAL(time, core::tools::strToBoostTime("120603"));
    CPPUNIT_ASSERT_EQUAL(time, core::tools::strToBoostTime("120603 Too many characters"));
    CPPUNIT_ASSERT_EQUAL(defaultTime, core::tools::strToBoostTime("12060"));
    CPPUNIT_ASSERT_EQUAL(defaultTime, core::tools::strToBoostTime("rabbit"));
}

//------------------------------------------------------------------------------

void DateAndTimeTest::dateAndTimeTest()
{
    const boost::gregorian::date date(2022, 2, 4);
    const boost::posix_time::time_duration time(12, 6, 3);
    const boost::posix_time::ptime dateAndTime(date, time);

    CPPUNIT_ASSERT_EQUAL(dateAndTime, core::tools::strToBoostDateAndTime("20220204", "120603"));
    CPPUNIT_ASSERT_EQUAL(dateAndTime, core::tools::fromIsoExtendedString("2022-02-04T12:06:03"));
}

//------------------------------------------------------------------------------

void DateAndTimeTest::toStringTest()
{
    using namespace std::literals::string_literals;

    const boost::gregorian::date date(2022, 2, 4);
    const boost::posix_time::time_duration time(12, 6, 3);
    const boost::posix_time::ptime dateAndTime(date, time);

    CPPUNIT_ASSERT_EQUAL("20220204"s, core::tools::getDate(dateAndTime));
    CPPUNIT_ASSERT_EQUAL("120603"s, core::tools::getTime(dateAndTime));
    CPPUNIT_ASSERT_EQUAL("2022-02-04T12:06:03"s, core::tools::toIsoExtendedString(dateAndTime));
    CPPUNIT_ASSERT_EQUAL("2022-02-04"s, core::tools::getDateInXsdFormat(dateAndTime));
}

} // namespace sight::core::tools::ut
