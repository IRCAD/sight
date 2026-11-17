/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <io/opencv/type.hpp>

#include <doctest/doctest.h>
#include <opencv2/core/hal/interface.h>

//------------------------------------------------------------------------------

template<typename T, std::size_t NUM_COMPONENTS>
static void test_to_cv(std::int32_t _expected_type)
{
    sight::core::type type  = sight::core::type::get<T>();
    const auto image_format = sight::io::opencv::type::to_cv(type, NUM_COMPONENTS);
    CHECK_EQ(_expected_type, image_format);
}

//------------------------------------------------------------------------------

template<typename EXPECTED_T, uint8_t EXPECTED_NUM_COMPONENTS>
static void test_from_cv(std::int32_t _cv_type)
{
    sight::core::type expected_type = sight::core::type::get<EXPECTED_T>();
    const auto format               = sight::io::opencv::type::from_cv(_cv_type);
    const auto type                 = format.first;
    const auto comp                 = format.second;

    CHECK_EQ(expected_type, type);
    CHECK_EQ(EXPECTED_NUM_COMPONENTS, comp);
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::io::opencv::type")
{
    TEST_CASE("type2str")
    {
        CHECK_EQ("8UC1", sight::io::opencv::type::type2str(CV_8UC1));
        CHECK_EQ("8SC2", sight::io::opencv::type::type2str(CV_8SC2));
        CHECK_EQ("16UC3", sight::io::opencv::type::type2str(CV_16UC3));
        CHECK_EQ("32SC4", sight::io::opencv::type::type2str(CV_32SC4));
        CHECK_EQ("32FC2", sight::io::opencv::type::type2str(CV_32FC2));
        CHECK_EQ("64FC1", sight::io::opencv::type::type2str(CV_64FC1));

        // Unknown depth falls back to "User" while preserving the number of channels.
        const auto custom_type = CV_MAKETYPE(7, 2);
        CHECK_EQ("UserC2", sight::io::opencv::type::type2str(custom_type));
    }

    TEST_CASE("to_cv")
    {
        test_to_cv<std::uint8_t, 1>(CV_8UC1);
        test_to_cv<std::uint8_t, 2>(CV_8UC2);
        test_to_cv<std::uint8_t, 3>(CV_8UC3);
        test_to_cv<std::uint8_t, 4>(CV_8UC4);

        test_to_cv<std::int8_t, 1>(CV_8SC1);
        test_to_cv<std::int8_t, 2>(CV_8SC2);
        test_to_cv<std::int8_t, 3>(CV_8SC3);
        test_to_cv<std::int8_t, 4>(CV_8SC4);

        test_to_cv<std::uint16_t, 1>(CV_16UC1);
        test_to_cv<std::uint16_t, 2>(CV_16UC2);
        test_to_cv<std::uint16_t, 3>(CV_16UC3);
        test_to_cv<std::uint16_t, 4>(CV_16UC4);

        test_to_cv<std::int16_t, 1>(CV_16SC1);
        test_to_cv<std::int16_t, 2>(CV_16SC2);
        test_to_cv<std::int16_t, 3>(CV_16SC3);
        test_to_cv<std::int16_t, 4>(CV_16SC4);

        test_to_cv<std::int32_t, 1>(CV_32SC1);
        test_to_cv<std::int32_t, 2>(CV_32SC2);
        test_to_cv<std::int32_t, 3>(CV_32SC3);
        test_to_cv<std::int32_t, 4>(CV_32SC4);

        test_to_cv<float, 1>(CV_32FC1);
        test_to_cv<float, 2>(CV_32FC2);
        test_to_cv<float, 3>(CV_32FC3);
        test_to_cv<float, 4>(CV_32FC4);

        test_to_cv<double, 1>(CV_64FC1);
        test_to_cv<double, 2>(CV_64FC2);
        test_to_cv<double, 3>(CV_64FC3);
        test_to_cv<double, 4>(CV_64FC4);
    }

//------------------------------------------------------------------------------

    TEST_CASE("from_cv")
    {
        test_from_cv<std::uint8_t, 1>(CV_8UC1);
        test_from_cv<std::uint8_t, 2>(CV_8UC2);
        test_from_cv<std::uint8_t, 3>(CV_8UC3);
        test_from_cv<std::uint8_t, 4>(CV_8UC4);

        test_from_cv<std::int8_t, 1>(CV_8SC1);
        test_from_cv<std::int8_t, 2>(CV_8SC2);
        test_from_cv<std::int8_t, 3>(CV_8SC3);
        test_from_cv<std::int8_t, 4>(CV_8SC4);

        test_from_cv<std::uint16_t, 1>(CV_16UC1);
        test_from_cv<std::uint16_t, 2>(CV_16UC2);
        test_from_cv<std::uint16_t, 3>(CV_16UC3);
        test_from_cv<std::uint16_t, 4>(CV_16UC4);

        test_from_cv<std::int16_t, 1>(CV_16SC1);
        test_from_cv<std::int16_t, 2>(CV_16SC2);
        test_from_cv<std::int16_t, 3>(CV_16SC3);
        test_from_cv<std::int16_t, 4>(CV_16SC4);

        test_from_cv<std::int32_t, 1>(CV_32SC1);
        test_from_cv<std::int32_t, 2>(CV_32SC2);
        test_from_cv<std::int32_t, 3>(CV_32SC3);
        test_from_cv<std::int32_t, 4>(CV_32SC4);

        test_from_cv<float, 1>(CV_32FC1);
        test_from_cv<float, 2>(CV_32FC2);
        test_from_cv<float, 3>(CV_32FC3);
        test_from_cv<float, 4>(CV_32FC4);

        test_from_cv<double, 1>(CV_64FC1);
        test_from_cv<double, 2>(CV_64FC2);
        test_from_cv<double, 3>(CV_64FC3);
        test_from_cv<double, 4>(CV_64FC4);
    }
}
