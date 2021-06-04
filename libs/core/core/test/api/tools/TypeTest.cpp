/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/tools/Type.hpp>

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

void TypeTest::typeTest()
{
    core::tools::Type INT8(core::tools::Type::s_INT8_TYPENAME);
    core::tools::Type INT16(core::tools::Type::s_INT16_TYPENAME);
    core::tools::Type INT32(core::tools::Type::s_INT32_TYPENAME);
    core::tools::Type INT64(core::tools::Type::s_INT64_TYPENAME);

    core::tools::Type UINT8(core::tools::Type::s_UINT8_TYPENAME);
    core::tools::Type UINT16(core::tools::Type::s_UINT16_TYPENAME);
    core::tools::Type UINT32(core::tools::Type::s_UINT32_TYPENAME);
    core::tools::Type UINT64(core::tools::Type::s_UINT64_TYPENAME);

    core::tools::Type FLOAT(core::tools::Type::s_FLOAT_TYPENAME);
    core::tools::Type DOUBLE(core::tools::Type::s_DOUBLE_TYPENAME);

    core::tools::Type t;

    CPPUNIT_ASSERT_EQUAL(std::string("int8"), INT8.string());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), INT16.string());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), INT32.string());
    CPPUNIT_ASSERT_EQUAL(std::string("int64"), INT64.string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), UINT8.string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), UINT16.string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), UINT32.string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint64"), UINT64.string());
    CPPUNIT_ASSERT_EQUAL(std::string("float"), FLOAT.string());
    CPPUNIT_ASSERT_EQUAL(std::string("double"), DOUBLE.string());

    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::tools::Type::create<signed char>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::tools::Type::create<char>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), core::tools::Type::create<short>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), core::tools::Type::create<int>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("int64"), core::tools::Type::create<long long>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), core::tools::Type::create<unsigned char>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), core::tools::Type::create<unsigned short>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), core::tools::Type::create<unsigned int>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint64"), core::tools::Type::create<unsigned long long>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("float"), core::tools::Type::create<float>().string());
    CPPUNIT_ASSERT_EQUAL(std::string("double"), core::tools::Type::create<double>().string());

    CPPUNIT_ASSERT_EQUAL(std::string("int8"), core::tools::Type::create("int8").string());
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), core::tools::Type::create("int16").string());
    CPPUNIT_ASSERT_EQUAL(std::string("int32"), core::tools::Type::create("int32").string());
    CPPUNIT_ASSERT_EQUAL(std::string("int64"), core::tools::Type::create("int64").string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), core::tools::Type::create("uint8").string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint16"), core::tools::Type::create("uint16").string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint32"), core::tools::Type::create("uint32").string());
    CPPUNIT_ASSERT_EQUAL(std::string("uint64"), core::tools::Type::create("uint64").string());
    CPPUNIT_ASSERT_EQUAL(std::string("float"), core::tools::Type::create("float").string());
    CPPUNIT_ASSERT_EQUAL(std::string("double"), core::tools::Type::create("double").string());

    CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_UNSPECIFIED_TYPE, core::tools::Type());

    CPPUNIT_ASSERT_EQUAL(INT8, core::tools::Type::create<signed char>());
    CPPUNIT_ASSERT_EQUAL(INT8, core::tools::Type::create<char>());
    CPPUNIT_ASSERT_EQUAL(INT16, core::tools::Type::create<short>());
    CPPUNIT_ASSERT_EQUAL(INT32, core::tools::Type::create<int>());
    CPPUNIT_ASSERT_EQUAL(INT64, core::tools::Type::create<long long>());

    CPPUNIT_ASSERT_EQUAL(UINT8, core::tools::Type::create<unsigned char>());
    CPPUNIT_ASSERT_EQUAL(UINT16, core::tools::Type::create<unsigned short>());
    CPPUNIT_ASSERT_EQUAL(UINT32, core::tools::Type::create<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(UINT64, core::tools::Type::create<unsigned long long>());

    CPPUNIT_ASSERT_EQUAL(FLOAT, core::tools::Type::create<float>());
    CPPUNIT_ASSERT_EQUAL(DOUBLE, core::tools::Type::create<double>());

    CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_UNSPECIFIED_TYPE, t);

    t.setType<void>();

    t.setType<signed char>();
    CPPUNIT_ASSERT_EQUAL(INT8, t);
    t.setType<char>();
    CPPUNIT_ASSERT_EQUAL(INT8, t);
    t.setType<short>();
    CPPUNIT_ASSERT_EQUAL(INT16, t);
    t.setType<int>();
    CPPUNIT_ASSERT_EQUAL(INT32, t);
    t.setType<long long>();
    CPPUNIT_ASSERT_EQUAL(INT64, t);
    t.setType<unsigned char>();
    CPPUNIT_ASSERT_EQUAL(UINT8, t);
    t.setType<unsigned short>();
    CPPUNIT_ASSERT_EQUAL(UINT16, t);
    t.setType<unsigned int>();
    CPPUNIT_ASSERT_EQUAL(UINT32, t);
    t.setType<unsigned long long>();
    CPPUNIT_ASSERT_EQUAL(UINT64, t);
    t.setType<float>();
    CPPUNIT_ASSERT_EQUAL(FLOAT, t);
    t.setType<double>();
    CPPUNIT_ASSERT_EQUAL(DOUBLE, t);

    CPPUNIT_ASSERT(typeid(void) == core::tools::Type::s_UNSPECIFIED_TYPE.typeId());

    CPPUNIT_ASSERT(typeid(signed char) == INT8.typeId());
    CPPUNIT_ASSERT(typeid(short) == INT16.typeId());
    CPPUNIT_ASSERT(typeid(int) == INT32.typeId());
    CPPUNIT_ASSERT(typeid(long long) == INT64.typeId());

    CPPUNIT_ASSERT(typeid(unsigned char) == UINT8.typeId());
    CPPUNIT_ASSERT(typeid(unsigned short) == UINT16.typeId());
    CPPUNIT_ASSERT(typeid(unsigned int) == UINT32.typeId());
    CPPUNIT_ASSERT(typeid(unsigned long long) == UINT64.typeId());

    CPPUNIT_ASSERT(typeid(float) == FLOAT.typeId());
    CPPUNIT_ASSERT(typeid(double) == DOUBLE.typeId());
}

} // namespace ut

} // namespace sight::core::tools
