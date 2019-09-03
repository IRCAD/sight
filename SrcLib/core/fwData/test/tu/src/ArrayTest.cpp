/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ArrayTest.hpp"

#include <fwData/Array.hpp>
#include <fwData/Exception.hpp>

#include <fwDataTools/helper/Array.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ArrayTest );

namespace fwData
{
namespace ut
{

//-----------------------------------------------------------------------------

void ArrayTest::setUp()
{
    // Set up context before running a test.

}

//-----------------------------------------------------------------------------

void ArrayTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void ArrayTest::allocation()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();
    auto lock = array->lock();

    CPPUNIT_ASSERT(array->empty());
    CPPUNIT_ASSERT(array->getBuffer() == nullptr);
    CPPUNIT_ASSERT(array->getSize().empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), array->getSizeInBytes());

    ::fwData::Array::SizeType size = {10, 100};

    // deprecated: replace by array->resize(size, ::fwTools::Type::s_UINT32, true)
    array->resize("uint32", size, 1, true);
    CPPUNIT_ASSERT(array->getBuffer() != nullptr);
    CPPUNIT_ASSERT(!array->empty());

    CPPUNIT_ASSERT_EQUAL(size.size(), array->getNumberOfDimensions());
    CPPUNIT_ASSERT_EQUAL(size[0], array->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(size[1], array->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(true, array->getIsBufferOwner());
    {
        ::fwData::Array::OffsetType stride = {4, 40};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT32, array->getType());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), array->getType().sizeOf());

    array->clear();
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(0), array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(0), array->getNumberOfComponents()); //deprecated
    CPPUNIT_ASSERT(  array->empty() );
    CPPUNIT_ASSERT(  array->getBuffer() == nullptr );
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UNSPECIFIED_TYPE, array->getType());

    std::uint16_t* buffer = new std::uint16_t[1000];

    for (std::uint16_t i = 0; i < 1000; i++)
    {
        buffer[i] = i;
    }

    array->setBuffer(buffer, false, size, ::fwTools::Type::s_UINT16, ::fwMemory::BufferNewPolicy::New());

    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(2), array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(2*10*100), array->getSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {2, 20};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(  buffer[0], array->at< std::uint16_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(  buffer[10], array->at< std::uint16_t >({0, 1}));
    CPPUNIT_ASSERT_EQUAL(  buffer[999], array->at< std::uint16_t >({9, 99}));
    CPPUNIT_ASSERT_EQUAL(  buffer[326], array->at< std::uint16_t >({6, 32}));
    CPPUNIT_ASSERT_EQUAL(  buffer[947], array->at< std::uint16_t >({7, 94}));
    CPPUNIT_ASSERT_EQUAL(  buffer[238], array->at< std::uint16_t >({8, 23}));
    CPPUNIT_ASSERT_EQUAL(false, array->getIsBufferOwner());

    array->clear();

    delete[] buffer;
}

//-----------------------------------------------------------------------------

void ArrayTest::resize()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();

    ::fwData::Array::SizeType size {10, 100};

    array->resize(size, ::fwTools::Type::s_UINT32, true);
    auto lock = array->lock();

    CPPUNIT_ASSERT(array->getBuffer() != nullptr);

    std::uint32_t count = 0;

    ::fwData::Array::Iterator<std::uint32_t> iter = array->begin<std::uint32_t>();

    for (; iter != array->end<std::uint32_t>(); ++iter)
    {
        *iter = count++;
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4*10*100), array->getSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {4, 40};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at< std::uint32_t >({9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at< std::uint32_t >({6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at< std::uint32_t >({7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at< std::uint32_t >({8, 23}));

    ::fwData::Array::SizeType newSize = {100, 10};

    array->resize(newSize);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4*10*100), array->getSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {4, 400};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at< std::uint32_t >({10, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at< std::uint32_t >({99, 9}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at< std::uint32_t >({26, 3}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at< std::uint32_t >({47, 9}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at< std::uint32_t >({38, 2}));

    newSize.clear();
    newSize = {25, 40};

    array->resize(newSize);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), array->getElementSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {4, 100};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at< std::uint32_t >({10, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at< std::uint32_t >({24, 39}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at< std::uint32_t >({1, 13}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at< std::uint32_t >({22, 37}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at< std::uint32_t >({13, 9}));

    newSize.clear();

    newSize = {2, 10, 100};

    array->resize(newSize, ::fwTools::Type::s_UINT16, false);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2*100*10*2), array->getSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {2, 4, 40};
        CPPUNIT_ASSERT(array->getStrides() == stride );
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at< std::uint32_t >({0, 9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at< std::uint32_t >({0, 6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at< std::uint32_t >({0, 7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at< std::uint32_t >({0, 8, 23}));

    //test deprecated API with component
    newSize = {10, 100};

    array->resize(::fwTools::Type::s_UINT16, newSize, 2, false);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2*100*10*2), array->getSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {4, 40};
        CPPUNIT_ASSERT(array->getStrides() == stride );
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at< std::uint32_t >({9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at< std::uint32_t >({6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at< std::uint32_t >({7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at< std::uint32_t >({8, 23}));
}

//-----------------------------------------------------------------------------

void ArrayTest::reallocate()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();

    ::fwData::Array::SizeType size = {10, 100};

    array->resize(size, ::fwTools::Type::s_UINT32, true);
    auto lock = array->lock();

    std::uint32_t count                              = 0;
    ::fwData::Array::Iterator<std::uint32_t> iter    = array->begin<std::uint32_t>();
    ::fwData::Array::Iterator<std::uint32_t> iterEnd = array->end<std::uint32_t>();
    for (; iter != iterEnd; ++iter)
    {
        *iter = count++;
    }
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(4*10*100), array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 1}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(999), array->at< std::uint32_t >({9, 99}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(326), array->at< std::uint32_t >({6, 32}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(947), array->at< std::uint32_t >({7, 94}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(238), array->at< std::uint32_t >({8, 23}));

    ::fwData::Array::SizeType newSize = {100, 100};

    array->resize(newSize, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(4*100*100), array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(10), array->at< std::uint32_t >({10, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(999), array->at< std::uint32_t >({99, 9}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(326), array->at< std::uint32_t >({26, 3}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(947), array->at< std::uint32_t >({47, 9}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(238), array->at< std::uint32_t >({38, 2}));

    std::uint32_t value = 1859;
    array->at<std::uint32_t>({50, 90}) = value;
    CPPUNIT_ASSERT_EQUAL(  value, array->at< std::uint32_t >({50, 90}));

    std::uint32_t value2 = 25464;
    array->at<std::uint32_t>({99, 99}) = value2;
    CPPUNIT_ASSERT_EQUAL(  value2, array->at< std::uint32_t >({99, 99}));

    {
        // deprecated API
        array->resize(newSize, 2, true);
        CPPUNIT_ASSERT(newSize == array->getSize());
        CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(4*100*100*2), array->getSizeInBytes());
        CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
        CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(10), array->at< std::uint32_t >({5, 0}));
        CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(998), array->at< std::uint32_t >({99, 4}));
        CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(326), array->at< std::uint32_t >({63, 1}));
        CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(946), array->at< std::uint32_t >({73, 4}));
        CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(238), array->at< std::uint32_t >({19, 1}));

        CPPUNIT_ASSERT_EQUAL(  value, array->at< std::uint32_t >({25, 45}));

        std::uint32_t value3 = 45643;
        array->at<std::uint32_t>({35, 48}) = value3;
        CPPUNIT_ASSERT_EQUAL(  value3, array->at< std::uint32_t >({35, 48}));

        std::uint32_t value4 = 16165;
        array->at<std::uint32_t>({99, 99}) = value4;
        CPPUNIT_ASSERT_EQUAL(  value4, array->at< std::uint32_t >({99, 99}));

        newSize.clear();
    }

    newSize = {2, 100, 100};
    array->resize(newSize, ::fwTools::Type::s_UINT32, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(4*2*100*100), array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 5, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(999), array->at< std::uint32_t >({1, 99, 4}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(326), array->at< std::uint32_t >({0, 63, 1}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(947), array->at< std::uint32_t >({1, 73, 4}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(238), array->at< std::uint32_t >({0, 19, 1}));

    CPPUNIT_ASSERT_EQUAL(  value, array->at< std::uint32_t >({0, 25, 45}));

    CPPUNIT_ASSERT_EQUAL(  value2, array->at< std::uint32_t >({1, 99, 49}));

    std::uint32_t value3 = 2156;
    array->at<std::uint32_t>({0, 35, 48}) = value3;
    CPPUNIT_ASSERT_EQUAL(  value3, array->at< std::uint32_t >({0, 35, 48}));

    std::uint32_t value4 = 105;
    array->at<std::uint32_t>({1, 99, 99}) = value4;
    CPPUNIT_ASSERT_EQUAL(  value4, array->at< std::uint32_t >({1, 99, 99}));

    newSize.clear();

    newSize = {10, 100};

    array->resize(newSize, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(4), array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(4*10*100), array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 1}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(999), array->at< std::uint32_t >({9, 99}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(326), array->at< std::uint32_t >({6, 32}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(947), array->at< std::uint32_t >({7, 94}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(238), array->at< std::uint32_t >({8, 23}));

    newSize = {2, 10, 100};
    array->resize(newSize, ::fwTools::Type::s_UINT16, true);
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(2), array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(2*10*100*2), array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 0, 1}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(999), array->at< std::uint32_t >({0, 9, 99}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(326), array->at< std::uint32_t >({0, 6, 32}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(947), array->at< std::uint32_t >({0, 7, 94}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(238), array->at< std::uint32_t >({0, 8, 23}));

    newSize = {10, 100};
    array->resize(newSize, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(2), array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(2*10*100), array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 2}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(326), array->at< std::uint32_t >({2, 65}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(238), array->at< std::uint32_t >({6, 47}));

    array->clear();
    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(0), array->getSizeInBytes());
}

//-----------------------------------------------------------------------------

void ArrayTest::copy()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();

    ::fwData::Array::SizeType size = {10, 100};

    array->resize(size, fwTools::Type::s_UINT32, true);
    auto arrayLock = array->lock();
    CPPUNIT_ASSERT(array->getBuffer() != nullptr);

    std::uint32_t count                              = 0;
    ::fwData::Array::Iterator<std::uint32_t> iter    = array->begin<std::uint32_t>();
    ::fwData::Array::Iterator<std::uint32_t> iterEnd = array->end<std::uint32_t>();

    for (; iter != iterEnd; ++iter)
    {
        *iter = count++;
    }

    ::fwData::Array::sptr deepCopyArray = ::fwData::Object::copy(array);
    auto deepCopyArrayLock = deepCopyArray->lock();

    // check deepCopy
    CPPUNIT_ASSERT_EQUAL(  array->getElementSizeInBytes(), deepCopyArray->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  array->getSizeInBytes(), deepCopyArray->getSizeInBytes());
    CPPUNIT_ASSERT(array->getStrides() == deepCopyArray->getStrides());
    CPPUNIT_ASSERT(array->getSize() == deepCopyArray->getSize());
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({0, 0}),
                          deepCopyArray->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({0, 1}),
                          deepCopyArray->at< std::uint32_t >({0, 1}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({9, 99}),
                          deepCopyArray->at< std::uint32_t >({9, 99}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({6, 32}),
                          deepCopyArray->at< std::uint32_t >({6, 32}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({7, 94}),
                          deepCopyArray->at< std::uint32_t >({7, 94}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({8, 23}),
                          deepCopyArray->at< std::uint32_t >({8, 23}));
    CPPUNIT_ASSERT_EQUAL( true, deepCopyArray->getIsBufferOwner());

    ::fwData::Array::csptr deepCopyArray2 = ::fwData::Object::copy(array);
    auto deepCopyArrayLock2 = deepCopyArray->lock();

    CPPUNIT_ASSERT_EQUAL(  array->getElementSizeInBytes(), deepCopyArray->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  array->getSizeInBytes(), deepCopyArray->getSizeInBytes());
    CPPUNIT_ASSERT(array->getStrides() == deepCopyArray->getStrides());
    CPPUNIT_ASSERT(array->getSize() == deepCopyArray->getSize());
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({0, 0}),
                          deepCopyArray->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({0, 1}),
                          deepCopyArray->at< std::uint32_t >({0, 1}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({9, 99}),
                          deepCopyArray->at< std::uint32_t >({9, 99}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({6, 32}),
                          deepCopyArray->at< std::uint32_t >({6, 32}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({7, 94}),
                          deepCopyArray->at< std::uint32_t >({7, 94}));
    CPPUNIT_ASSERT_EQUAL( array->at< std::uint32_t >({8, 23}),
                          deepCopyArray->at< std::uint32_t >({8, 23}));
    CPPUNIT_ASSERT_EQUAL( true, deepCopyArray->getIsBufferOwner());
}

//-----------------------------------------------------------------------------

void ArrayTest::dumpLockTest()
{
    ::fwData::Array::SizeType size = {10, 100};
    ::fwData::Array::sptr array    = ::fwData::Array::New();
    array->resize(::fwTools::Type::s_INT16, {12, 15}, true);

    CPPUNIT_ASSERT_THROW(array->getBuffer(), ::fwData::Exception);

    auto lock = array->lock();
    CPPUNIT_ASSERT_NO_THROW(array->getBuffer());
}

//-----------------------------------------------------------------------------

void ArrayTest::bufferAccessTest()
{
    // test getBuffer
    ::fwData::Array::sptr array = ::fwData::Array::New();

    ::fwData::Array::SizeType size = {10, 100};

    array->resize(size, ::fwTools::Type::s_UINT32, true);
    auto lock = array->lock();

    std::uint32_t count                              = 0;
    ::fwData::Array::Iterator<std::uint32_t> iter    = array->begin<std::uint32_t>();
    ::fwData::Array::Iterator<std::uint32_t> iterEnd = array->end<std::uint32_t>();

    for (; iter != iterEnd; ++iter)
    {
        *iter = count++;
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(99), array->at<std::uint32_t>({9, 9}));

    CPPUNIT_ASSERT_THROW(array->at<std::uint32_t>({10, 0}), ::fwData::Exception);
    CPPUNIT_ASSERT_THROW(array->at<std::uint32_t>({0, 100}), ::fwData::Exception);

    {
        Array::Iterator<std::uint32_t> itr = array->begin<std::uint32_t>();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), *itr);
        itr++;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(1), *itr);
        ++itr;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(2), *itr);
        itr = itr + 3;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(5), *itr);
        itr += 5;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), *itr);

        std::uint32_t val = 10;
        const auto endItr = array->end<std::uint32_t>();
        while(itr != endItr)
        {
            CPPUNIT_ASSERT_EQUAL(val, *itr);
            CPPUNIT_ASSERT_NO_THROW(++itr);
            ++val;
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(1000), val);
    }
    {
        Array::ConstIterator<std::uint32_t> itr = array->begin<std::uint32_t>();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), *itr);
        itr++;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(1), *itr);
        ++itr;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(2), *itr);
        itr = itr + 3;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(5), *itr);
        itr += 5;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), *itr);

        std::uint32_t val = 10;
        const auto endItr = array->end<std::uint32_t>();
        while(itr != endItr)
        {
            CPPUNIT_ASSERT_EQUAL(val, *itr);
            CPPUNIT_ASSERT_NO_THROW(++itr);
            ++val;
        }
    }
}

//-----------------------------------------------------------------------------

void ArrayTest::constArrayTest()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();

    ::fwData::Array::SizeType size = {10, 100};

    array->resize(size, ::fwTools::Type::s_UINT32, true);
    auto lock = array->lock();

    std::uint32_t count                           = 0;
    ::fwData::Array::Iterator<std::uint32_t> iter = array->begin<std::uint32_t>();
    for (; iter != array->end<std::uint32_t>(); ++iter)
    {
        *iter = count++;
    }

    ::fwData::Array::csptr array2 = ::fwData::Object::copy(array);

    CPPUNIT_ASSERT_EQUAL(  static_cast<size_t>(4*10*100), array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(0), array->at< std::uint32_t >({0, 0}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(10), array->at< std::uint32_t >({0, 1}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(999), array->at< std::uint32_t >({9, 99}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(326), array->at< std::uint32_t >({6, 32}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(947), array->at< std::uint32_t >({7, 94}));
    CPPUNIT_ASSERT_EQUAL(  static_cast<std::uint32_t>(238), array->at< std::uint32_t >({8, 23}));

    auto it1 = array->begin< std::uint32_t >();
    auto it2 = array2->begin< std::uint32_t >();

    while(it1 != array->end< std::uint32_t >() && it2 != array2->end< std::uint32_t >())
    {
        CPPUNIT_ASSERT_EQUAL(*it1, *it2);
        CPPUNIT_ASSERT_NO_THROW(++it1);
        CPPUNIT_ASSERT_NO_THROW(++it2);
    }
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwData
