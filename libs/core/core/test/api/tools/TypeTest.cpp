/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "TypeTest.hpp"

#include <core/Type.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::TypeTest);

namespace sight::core::tools
{

namespace ut
{

//------------------------------------------------------------------------------

void TypeTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void TypeTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

class A
{
};

//------------------------------------------------------------------------------

void TypeTest::typeTest()
{
    core::Type INT8  = core::Type::INT8;
    core::Type INT16 = core::Type::INT16;
    core::Type INT32 = core::Type::INT32;
    core::Type INT64 = core::Type::INT64;

    core::Type UINT8  = core::Type::UINT8;
    core::Type UINT16 = core::Type::UINT16;
    core::Type UINT32 = core::Type::UINT32;
    core::Type UINT64 = core::Type::UINT64;

    core::Type FLOAT  = core::Type::FLOAT;
    core::Type DOUBLE = core::Type::DOUBLE;

    CPPUNIT_ASSERT_EQUAL(std::string("int8"), INT8.name());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), INT16.name());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), INT32.name());
    CPPUNIT_ASSERT_EQUAL(std::string("int64"), INT64.name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), UINT8.name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), UINT16.name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), UINT32.name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint64"), UINT64.name());
    CPPUNIT_ASSERT_EQUAL(std::string("float"), FLOAT.name());
    CPPUNIT_ASSERT_EQUAL(std::string("double"), DOUBLE.name());

    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::Type::get<signed char>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::Type::get<char>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::Type::get<std::int8_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), core::Type::get<short>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), core::Type::get<std::int16_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), core::Type::get<int>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), core::Type::get<std::int32_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int64"), core::Type::get<std::int64_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), core::Type::get<unsigned char>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), core::Type::get<std::uint8_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), core::Type::get<unsigned short>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), core::Type::get<std::uint16_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), core::Type::get<unsigned int>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), core::Type::get<std::uint32_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint64"), core::Type::get<std::uint64_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("float"), core::Type::get<float>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("double"), core::Type::get<double>().name());

    CPPUNIT_ASSERT_EQUAL(core::Type::NONE, core::Type());

    CPPUNIT_ASSERT_EQUAL(INT8, core::Type::get<signed char>());
    CPPUNIT_ASSERT_EQUAL(INT8, core::Type::get<char>());
    CPPUNIT_ASSERT_EQUAL(INT16, core::Type::get<short>());
    CPPUNIT_ASSERT_EQUAL(INT32, core::Type::get<int>());
    CPPUNIT_ASSERT_EQUAL(INT64, core::Type::get<std::int64_t>());

    CPPUNIT_ASSERT_EQUAL(UINT8, core::Type::get<unsigned char>());
    CPPUNIT_ASSERT_EQUAL(UINT16, core::Type::get<unsigned short>());
    CPPUNIT_ASSERT_EQUAL(UINT32, core::Type::get<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(UINT64, core::Type::get<std::uint64_t>());

    CPPUNIT_ASSERT_EQUAL(FLOAT, core::Type::get<float>());
    CPPUNIT_ASSERT_EQUAL(DOUBLE, core::Type::get<double>());
}

//------------------------------------------------------------------------------

void TypeTest::typeToStringTest()
{
    using namespace std::literals::string_literals;

    int integer     = 1;
    double floating = 2.;

    CPPUNIT_ASSERT_EQUAL("1"s, core::Type::INT32.toString(&integer));
    CPPUNIT_ASSERT_EQUAL("2.000000"s, core::Type::DOUBLE.toString(&floating));
}

} // namespace ut

} // namespace sight::core::tools
