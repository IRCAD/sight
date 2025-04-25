/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "type_test.hpp"

#include <core/type.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::type_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void type_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void type_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void type_test::basic_test()
{
    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::type::INT8.name());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), core::type::INT16.name());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), core::type::INT32.name());
    CPPUNIT_ASSERT_EQUAL(std::string("int64"), core::type::INT64.name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), core::type::UINT8.name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), core::type::UINT16.name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), core::type::UINT32.name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint64"), core::type::UINT64.name());
    CPPUNIT_ASSERT_EQUAL(std::string("float"), core::type::FLOAT.name());
    CPPUNIT_ASSERT_EQUAL(std::string("double"), core::type::DOUBLE.name());

    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::type::get<signed char>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::type::get<char>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::type::get<std::int8_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), core::type::get<short>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), core::type::get<std::int16_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), core::type::get<int>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), core::type::get<std::int32_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("int64"), core::type::get<std::int64_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), core::type::get<unsigned char>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), core::type::get<std::uint8_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), core::type::get<unsigned short>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), core::type::get<std::uint16_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), core::type::get<unsigned int>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), core::type::get<std::uint32_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("uint64"), core::type::get<std::uint64_t>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("float"), core::type::get<float>().name());
    CPPUNIT_ASSERT_EQUAL(std::string("double"), core::type::get<double>().name());

    CPPUNIT_ASSERT_EQUAL(core::type::NONE, core::type());

    CPPUNIT_ASSERT_EQUAL(core::type::INT8, core::type::get<signed char>());
    CPPUNIT_ASSERT_EQUAL(core::type::INT8, core::type::get<char>());
    CPPUNIT_ASSERT_EQUAL(core::type::INT16, core::type::get<short>());
    CPPUNIT_ASSERT_EQUAL(core::type::INT32, core::type::get<int>());
    CPPUNIT_ASSERT_EQUAL(core::type::INT64, core::type::get<std::int64_t>());

    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, core::type::get<unsigned char>());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT16, core::type::get<unsigned short>());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT32, core::type::get<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT64, core::type::get<std::uint64_t>());

    CPPUNIT_ASSERT_EQUAL(core::type::FLOAT, core::type::get<float>());
    CPPUNIT_ASSERT_EQUAL(core::type::DOUBLE, core::type::get<double>());
}

//------------------------------------------------------------------------------

void type_test::type_to_string_test()
{
    using namespace std::literals::string_literals;

    std::uint64_t integer  = 1;
    float floating         = 2.;
    double double_floating = 3.;

    CPPUNIT_ASSERT_EQUAL("1"s, core::type::INT8.to_string(&integer));
    CPPUNIT_ASSERT_EQUAL("1"s, core::type::INT16.to_string(&integer));
    CPPUNIT_ASSERT_EQUAL("1"s, core::type::INT32.to_string(&integer));
    CPPUNIT_ASSERT_EQUAL("1"s, core::type::INT64.to_string(&integer));
    CPPUNIT_ASSERT_EQUAL("1"s, core::type::UINT8.to_string(&integer));
    CPPUNIT_ASSERT_EQUAL("1"s, core::type::UINT16.to_string(&integer));
    CPPUNIT_ASSERT_EQUAL("1"s, core::type::UINT32.to_string(&integer));
    CPPUNIT_ASSERT_EQUAL("1"s, core::type::UINT64.to_string(&integer));
    CPPUNIT_ASSERT_EQUAL("2.000000"s, core::type::FLOAT.to_string(&floating));
    CPPUNIT_ASSERT_EQUAL("3.000000"s, core::type::DOUBLE.to_string(&double_floating));
    CPPUNIT_ASSERT_EQUAL(""s, core::type::NONE.to_string(&integer));
}

//------------------------------------------------------------------------------

void type_test::type_switch_test()
{
    static constexpr auto s_TYPES = {
        core::type::INT8,
        core::type::INT16,
        core::type::INT32,
        core::type::INT64,
        core::type::UINT8,
        core::type::UINT16,
        core::type::UINT32,
        core::type::UINT64,
        core::type::FLOAT,
        core::type::DOUBLE
    };

    std::map<core::type, bool> found {
        {core::type::INT8, false},
        {core::type::INT16, false},
        {core::type::INT32, false},
        {core::type::INT64, false},
        {core::type::UINT8, false},
        {core::type::UINT16, false},
        {core::type::UINT32, false},
        {core::type::UINT64, false},
        {core::type::FLOAT, false},
        {core::type::DOUBLE, false},
    };

    for(const auto& type : s_TYPES)
    {
        switch(type)
        {
            case core::type::INT8:
                found[core::type::INT8] = true;
                break;

            case core::type::UINT8:
                found[core::type::UINT8] = true;
                break;

            case core::type::INT16:
                found[core::type::INT16] = true;
                break;

            case core::type::UINT16:
                found[core::type::UINT16] = true;
                break;

            case core::type::INT32:
                found[core::type::INT32] = true;
                break;

            case core::type::UINT32:
                found[core::type::UINT32] = true;
                break;

            case core::type::INT64:
                found[core::type::INT64] = true;
                break;

            case core::type::UINT64:
                found[core::type::UINT64] = true;
                break;

            case core::type::FLOAT:
                found[core::type::FLOAT] = true;
                break;

            case core::type::DOUBLE:
                found[core::type::DOUBLE] = true;
                break;

            default:
                CPPUNIT_FAIL("Unknown type");
                break;
        }
    }

    for(const auto& pair : found)
    {
        CPPUNIT_ASSERT_MESSAGE("Type " + pair.first.name() + " not found", pair.second);
    }
}

} // namespace sight::core::tools::ut
