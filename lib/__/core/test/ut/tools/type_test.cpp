/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/type.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::tools::type")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        CHECK_EQ(std::string("int8"), sight::core::type::INT8.name());
        CHECK_EQ(std::string("int16"), sight::core::type::INT16.name());
        CHECK_EQ(std::string("int32"), sight::core::type::INT32.name());
        CHECK_EQ(std::string("int64"), sight::core::type::INT64.name());
        CHECK_EQ(std::string("uint8"), sight::core::type::UINT8.name());
        CHECK_EQ(std::string("uint16"), sight::core::type::UINT16.name());
        CHECK_EQ(std::string("uint32"), sight::core::type::UINT32.name());
        CHECK_EQ(std::string("uint64"), sight::core::type::UINT64.name());
        CHECK_EQ(std::string("float"), sight::core::type::FLOAT.name());
        CHECK_EQ(std::string("double"), sight::core::type::DOUBLE.name());

        CHECK_EQ(std::string("int8"), sight::core::type::get<signed char>().name());
        CHECK_EQ(std::string("int8"), sight::core::type::get<char>().name());
        CHECK_EQ(std::string("int8"), sight::core::type::get<std::int8_t>().name());
        CHECK_EQ(std::string("int16"), sight::core::type::get<short>().name());
        CHECK_EQ(std::string("int16"), sight::core::type::get<std::int16_t>().name());
        CHECK_EQ(std::string("int32"), sight::core::type::get<int>().name());
        CHECK_EQ(std::string("int32"), sight::core::type::get<std::int32_t>().name());
        CHECK_EQ(std::string("int64"), sight::core::type::get<std::int64_t>().name());
        CHECK_EQ(std::string("uint8"), sight::core::type::get<unsigned char>().name());
        CHECK_EQ(std::string("uint8"), sight::core::type::get<std::uint8_t>().name());
        CHECK_EQ(std::string("uint16"), sight::core::type::get<unsigned short>().name());
        CHECK_EQ(std::string("uint16"), sight::core::type::get<std::uint16_t>().name());
        CHECK_EQ(std::string("uint32"), sight::core::type::get<unsigned int>().name());
        CHECK_EQ(std::string("uint32"), sight::core::type::get<std::uint32_t>().name());
        CHECK_EQ(std::string("uint64"), sight::core::type::get<std::uint64_t>().name());
        CHECK_EQ(std::string("float"), sight::core::type::get<float>().name());
        CHECK_EQ(std::string("double"), sight::core::type::get<double>().name());

        CHECK_EQ(sight::core::type::NONE, sight::core::type());

        CHECK_EQ(sight::core::type::INT8, sight::core::type::get<signed char>());
        CHECK_EQ(sight::core::type::INT8, sight::core::type::get<char>());
        CHECK_EQ(sight::core::type::INT16, sight::core::type::get<short>());
        CHECK_EQ(sight::core::type::INT32, sight::core::type::get<int>());
        CHECK_EQ(sight::core::type::INT64, sight::core::type::get<std::int64_t>());

        CHECK_EQ(sight::core::type::UINT8, sight::core::type::get<unsigned char>());
        CHECK_EQ(sight::core::type::UINT16, sight::core::type::get<unsigned short>());
        CHECK_EQ(sight::core::type::UINT32, sight::core::type::get<unsigned int>());
        CHECK_EQ(sight::core::type::UINT64, sight::core::type::get<std::uint64_t>());

        CHECK_EQ(sight::core::type::FLOAT, sight::core::type::get<float>());
        CHECK_EQ(sight::core::type::DOUBLE, sight::core::type::get<double>());
    }

//------------------------------------------------------------------------------

    TEST_CASE("type_to_string")
    {
        using namespace std::literals::string_literals;

        std::uint64_t integer  = 1;
        float floating         = 2.;
        double double_floating = 3.;

        CHECK_EQ("1"s, sight::core::type::INT8.to_string(&integer));
        CHECK_EQ("1"s, sight::core::type::INT16.to_string(&integer));
        CHECK_EQ("1"s, sight::core::type::INT32.to_string(&integer));
        CHECK_EQ("1"s, sight::core::type::INT64.to_string(&integer));
        CHECK_EQ("1"s, sight::core::type::UINT8.to_string(&integer));
        CHECK_EQ("1"s, sight::core::type::UINT16.to_string(&integer));
        CHECK_EQ("1"s, sight::core::type::UINT32.to_string(&integer));
        CHECK_EQ("1"s, sight::core::type::UINT64.to_string(&integer));
        CHECK_EQ("2.000000"s, sight::core::type::FLOAT.to_string(&floating));
        CHECK_EQ("3.000000"s, sight::core::type::DOUBLE.to_string(&double_floating));
        CHECK_EQ(""s, sight::core::type::NONE.to_string(&integer));
    }

//------------------------------------------------------------------------------

    TEST_CASE("type_switch")
    {
        static constexpr auto s_TYPES = {
            sight::core::type::INT8,
            sight::core::type::INT16,
            sight::core::type::INT32,
            sight::core::type::INT64,
            sight::core::type::UINT8,
            sight::core::type::UINT16,
            sight::core::type::UINT32,
            sight::core::type::UINT64,
            sight::core::type::FLOAT,
            sight::core::type::DOUBLE
        };

        std::map<sight::core::type, bool> found {
            {sight::core::type::INT8, false},
            {sight::core::type::INT16, false},
            {sight::core::type::INT32, false},
            {sight::core::type::INT64, false},
            {sight::core::type::UINT8, false},
            {sight::core::type::UINT16, false},
            {sight::core::type::UINT32, false},
            {sight::core::type::UINT64, false},
            {sight::core::type::FLOAT, false},
            {sight::core::type::DOUBLE, false},
        };

        for(const auto& type : s_TYPES)
        {
            switch(type)
            {
                case sight::core::type::INT8:
                    found[sight::core::type::INT8] = true;
                    break;

                case sight::core::type::UINT8:
                    found[sight::core::type::UINT8] = true;
                    break;

                case sight::core::type::INT16:
                    found[sight::core::type::INT16] = true;
                    break;

                case sight::core::type::UINT16:
                    found[sight::core::type::UINT16] = true;
                    break;

                case sight::core::type::INT32:
                    found[sight::core::type::INT32] = true;
                    break;

                case sight::core::type::UINT32:
                    found[sight::core::type::UINT32] = true;
                    break;

                case sight::core::type::INT64:
                    found[sight::core::type::INT64] = true;
                    break;

                case sight::core::type::UINT64:
                    found[sight::core::type::UINT64] = true;
                    break;

                case sight::core::type::FLOAT:
                    found[sight::core::type::FLOAT] = true;
                    break;

                case sight::core::type::DOUBLE:
                    found[sight::core::type::DOUBLE] = true;
                    break;

                default:
                    FAIL("Unknown type");
                    break;
            }
        }

        for(const auto& pair : found)
        {
            CHECK_MESSAGE(pair.second, "Type ", pair.first.name(), " not found");
        }
    }
} // TEST_SUITE
