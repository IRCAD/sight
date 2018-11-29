/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
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

#include "TypeTest.hpp"

#include <cvIO/Type.hpp>

#include <opencv2/core.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::cvIO::ut::TypeTest );

namespace cvIO
{
namespace ut
{

//------------------------------------------------------------------------------

void TypeTest::setUp()
{
    // Set up context before running a test.
    std::srand(101101);
}

//------------------------------------------------------------------------------

void TypeTest::tearDown()
{
}

//------------------------------------------------------------------------------

template<typename _T, size_t _NUM_COMPONENTS>
void testToCv(std::int32_t _expectedType)
{
    ::fwTools::Type type = ::fwTools::Type::create<_T>();
    const auto imageFormat = ::cvIO::Type::toCv(type, _NUM_COMPONENTS);
    CPPUNIT_ASSERT_EQUAL(_expectedType, imageFormat);
}

//------------------------------------------------------------------------------

template<typename _EXPECTED_T, uint8_t _EXPECTED_NUM_COMPONENTS>
void testFromCv(std::int32_t _cvType)
{
    ::fwTools::Type expectedType = ::fwTools::Type::create<_EXPECTED_T>();
    const auto format = ::cvIO::Type::fromCv(_cvType);
    const auto type   = format.first;
    const auto comp   = format.second;

    CPPUNIT_ASSERT_EQUAL(expectedType, type);
    CPPUNIT_ASSERT_EQUAL(_EXPECTED_NUM_COMPONENTS, comp);
}

//------------------------------------------------------------------------------

void TypeTest::toCv()
{
    testToCv<std::uint8_t, 1>(CV_8UC1);
    testToCv<std::uint8_t, 2>(CV_8UC2);
    testToCv<std::uint8_t, 3>(CV_8UC3);
    testToCv<std::uint8_t, 4>(CV_8UC4);

    testToCv<std::int8_t, 1>(CV_8SC1);
    testToCv<std::int8_t, 2>(CV_8SC2);
    testToCv<std::int8_t, 3>(CV_8SC3);
    testToCv<std::int8_t, 4>(CV_8SC4);

    testToCv<std::uint16_t, 1>(CV_16UC1);
    testToCv<std::uint16_t, 2>(CV_16UC2);
    testToCv<std::uint16_t, 3>(CV_16UC3);
    testToCv<std::uint16_t, 4>(CV_16UC4);

    testToCv<std::int16_t, 1>(CV_16SC1);
    testToCv<std::int16_t, 2>(CV_16SC2);
    testToCv<std::int16_t, 3>(CV_16SC3);
    testToCv<std::int16_t, 4>(CV_16SC4);

    testToCv<std::int32_t, 1>(CV_32SC1);
    testToCv<std::int32_t, 2>(CV_32SC2);
    testToCv<std::int32_t, 3>(CV_32SC3);
    testToCv<std::int32_t, 4>(CV_32SC4);

    testToCv<float, 1>(CV_32FC1);
    testToCv<float, 2>(CV_32FC2);
    testToCv<float, 3>(CV_32FC3);
    testToCv<float, 4>(CV_32FC4);

    testToCv<double, 1>(CV_64FC1);
    testToCv<double, 2>(CV_64FC2);
    testToCv<double, 3>(CV_64FC3);
    testToCv<double, 4>(CV_64FC4);
}

//------------------------------------------------------------------------------

void TypeTest::fromCv()
{
    testFromCv<std::uint8_t, 1>(CV_8UC1);
    testFromCv<std::uint8_t, 2>(CV_8UC2);
    testFromCv<std::uint8_t, 3>(CV_8UC3);
    testFromCv<std::uint8_t, 4>(CV_8UC4);

    testFromCv<std::int8_t, 1>(CV_8SC1);
    testFromCv<std::int8_t, 2>(CV_8SC2);
    testFromCv<std::int8_t, 3>(CV_8SC3);
    testFromCv<std::int8_t, 4>(CV_8SC4);

    testFromCv<std::uint16_t, 1>(CV_16UC1);
    testFromCv<std::uint16_t, 2>(CV_16UC2);
    testFromCv<std::uint16_t, 3>(CV_16UC3);
    testFromCv<std::uint16_t, 4>(CV_16UC4);

    testFromCv<std::int16_t, 1>(CV_16SC1);
    testFromCv<std::int16_t, 2>(CV_16SC2);
    testFromCv<std::int16_t, 3>(CV_16SC3);
    testFromCv<std::int16_t, 4>(CV_16SC4);

    testFromCv<std::int32_t, 1>(CV_32SC1);
    testFromCv<std::int32_t, 2>(CV_32SC2);
    testFromCv<std::int32_t, 3>(CV_32SC3);
    testFromCv<std::int32_t, 4>(CV_32SC4);

    testFromCv<float, 1>(CV_32FC1);
    testFromCv<float, 2>(CV_32FC2);
    testFromCv<float, 3>(CV_32FC3);
    testFromCv<float, 4>(CV_32FC4);

    testFromCv<double, 1>(CV_64FC1);
    testFromCv<double, 2>(CV_64FC2);
    testFromCv<double, 3>(CV_64FC3);
    testFromCv<double, 4>(CV_64FC4);
}

}// namespace ut

}// namespace cvIO
